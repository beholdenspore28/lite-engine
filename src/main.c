#include "gl.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "blib/b_list.h"
B_LIST_IMPLEMENTATION
DECLARE_LIST(Vector3)
DEFINE_LIST(Vector3)
DECLARE_LIST(Matrix4x4)
DEFINE_LIST(Matrix4x4)
DECLARE_LIST(Quaternion)
DEFINE_LIST(Quaternion)

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static float currentTime = 0, lastTime = 0, deltaTime = 0, FPS = 0;

typedef struct {
  List_Matrix4x4 matrices;
  List_Vector3 positions;
  List_Quaternion rotations;
  List_Vector3 colors;
  mesh meshes;
} pointLight;

typedef struct {
  List_Matrix4x4 matrices;
  List_Vector3 positions;
  List_Quaternion rotations;

  List_Vector3 colors;
  mesh meshes;
} cube;

typedef struct {
  Matrix4x4 viewMatrix;
  Vector3 position;
  Quaternion rotation;
  float lookSensitivity;
  float lastX;
  float lastY;
} camera;

int main(void) {
  printf("Rev up those fryers!\n");

  window windowData = window_create();
  // glfwSetCursorPosCallback(windowData.glfwWindow, mouse_callback);
  //glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  GLuint diffuseShader = shader_create("res/shaders/diffuse.vs.glsl",
                                       "res/shaders/diffuse.fs.glsl");
  GLuint unlitShader =
      shader_create("res/shaders/unlit.vs.glsl", "res/shaders/unlit.fs.glsl");

  GLuint containerDiffuse = texture_create("res/textures/container2.png");
  GLuint lampDiffuse = texture_create("res/textures/glowstone.png");
  GLuint containerSpecular =
      texture_create("res/textures/container2_specular.png");

  camera cam = {
      .viewMatrix = Matrix4x4_Identity(),
      .position = Vector3_Zero(),
      .rotation = Quaternion_Identity(),
      .lookSensitivity = 10,
      .lastX = 0,
      .lastY = 0,
  };

  cam.position = (Vector3){4, 2, -10};

  // cubes
  cube cubes;
  cubes.matrices = List_Matrix4x4_Alloc();
  cubes.positions = List_Vector3_Alloc();
  cubes.rotations = List_Quaternion_Alloc();
  cubes.colors = List_Vector3_Alloc();

  for (size_t i = 0; i < 10; i++) {
    mesh_allocCube(&cubes.meshes);

    Vector3 pos = (Vector3){i * 2, 0, 0};
    List_Vector3_Add(&cubes.positions, pos);
    List_Quaternion_Add(&cubes.rotations, Quaternion_Identity());
    List_Vector3_Add(&cubes.colors, Vector3_One(1.0f));
    List_Matrix4x4_Add(&cubes.matrices, Matrix4x4_Identity());
  }

  // lights
  pointLight pointLights;
  pointLights.matrices = List_Matrix4x4_Alloc();
  pointLights.positions = List_Vector3_Alloc();
  pointLights.rotations = List_Quaternion_Alloc();
  pointLights.colors = List_Vector3_Alloc();

  for (size_t i = 0; i < 10; i++) {
    mesh_allocCube(&pointLights.meshes);

    Vector3 color = Vector3_One(0.5f);
    Vector3 pos = (Vector3){i * 16, -2, -2};
    List_Vector3_Add(&pointLights.colors, color);
    List_Vector3_Add(&pointLights.positions, pos);
    List_Quaternion_Add(&pointLights.rotations, Quaternion_Identity());
    List_Matrix4x4_Add(&pointLights.matrices, Matrix4x4_Identity());
  }

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  (void)FPS;
  float aspect;
  Matrix4x4 projection = Matrix4x4_Identity();
  Vector3 ambientLight = Vector3_One(0.2f);

  while (!glfwWindowShouldClose(windowData.glfwWindow)) {
    { // TIME
      currentTime = glfwGetTime();
      deltaTime = currentTime - lastTime;
      lastTime = currentTime;

      FPS = 1 / deltaTime;
      // printf("============FRAME=START==============\n");
      // printf("delta %f : FPS %f\n", deltaTime, FPS);
    }

    {   // INPUT
      { // mouse look
        static bool firstMouse = true;
        double x, y;
        glfwGetCursorPos(windowData.glfwWindow, &x, &y);
        if (firstMouse) {
          cam.lastX = x;
          cam.lastY = y;
          firstMouse = false;
        }
        float xoffset = x - cam.lastX;
        cam.lastX = x;
        float angle = xoffset * deltaTime * cam.lookSensitivity;
        Quaternion rotation = Quaternion_FromEuler(Vector3_Up(angle));
        cam.rotation = Quaternion_Multiply(cam.rotation, rotation);
      }

      { // movement
        Vector3 velocity = Vector3_One(1.0f);
        float cameraSpeed = 15 * deltaTime;

        velocity.x *= glfwGetKey(windowData.glfwWindow, GLFW_KEY_D) -
                      glfwGetKey(windowData.glfwWindow, GLFW_KEY_A);
        velocity.y *= glfwGetKey(windowData.glfwWindow, GLFW_KEY_SPACE) -
                      glfwGetKey(windowData.glfwWindow, GLFW_KEY_LEFT_SHIFT);
        velocity.z *= glfwGetKey(windowData.glfwWindow, GLFW_KEY_W) -
                      glfwGetKey(windowData.glfwWindow, GLFW_KEY_S);

        velocity = Vector3_Normalize(velocity);
        velocity = Vector3_Scale(velocity, cameraSpeed);
				velocity = Vector3_Rotate(velocity, cam.rotation);
        cam.position = Vector3_Add(cam.position, velocity);
      }
    }

    glfwGetWindowSize(windowData.glfwWindow, &windowData.width,
                      &windowData.height);
    aspect = (float)windowData.width / (float)windowData.height;
    projection = Matrix4x4_perspective(deg2rad(60), aspect, 0.1f, 1000.0f);

    { // draw
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // view matrix
      cam.viewMatrix = Matrix4x4_Translation(Vector3_Negate(cam.position));
      cam.viewMatrix = Matrix4x4_Multiply(cam.viewMatrix,
                                          Quaternion_ToMatrix4x4(cam.rotation));
      Matrix4x4_print(cam.viewMatrix, "view");

      glUseProgram(diffuseShader);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, containerDiffuse);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, containerSpecular);

      // camera
      shader_setUniformV3(diffuseShader, "u_cameraPos", cam.position);

      // directional light
      shader_setUniformV3(diffuseShader, "u_dirLight.direction",
                          (Vector3){-0.2f, -1.0f, -0.3f});
      shader_setUniformV3(diffuseShader, "u_dirLight.ambient", ambientLight);
      shader_setUniformV3(diffuseShader, "u_dirLight.diffuse",
                          (Vector3){0.4f, 0.4f, 0.4f});
      shader_setUniformV3(diffuseShader, "u_dirLight.specular",
                          (Vector3){0.5f, 0.5f, 0.5f});

      // point light 0
      shader_setUniformV3(diffuseShader, "u_pointLights[0].position",
                          pointLights.positions.data[0]);
      shader_setUniformV3(diffuseShader, "u_pointLights[0].ambient",
                          ambientLight);
      shader_setUniformV3(diffuseShader, "u_pointLights[0].diffuse",
                          (Vector3){0.8f, 0.8f, 0.8f});
      shader_setUniformV3(diffuseShader, "u_pointLights[0].specular",
                          (Vector3){1.0f, 1.0f, 1.0f});
      shader_setUniformFloat(diffuseShader, "u_pointLights[0].constant", 1.0f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[0].linear", 0.09f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[0].quadratic",
                             0.032f);

      // spot light
      shader_setUniformV3(diffuseShader, "u_spotLight.position", cam.position);
      shader_setUniformV3(
          diffuseShader, "u_spotLight.direction",
          Vector3_Back(
              1.0f)); // this should be changed to the local camera forward
      shader_setUniformV3(diffuseShader, "u_spotLight.ambient", ambientLight);
      shader_setUniformV3(diffuseShader, "u_spotLight.diffuse",
                          (Vector3){1.0f, 1.0f, 1.0f});
      shader_setUniformV3(diffuseShader, "u_spotLight.specular",
                          (Vector3){1.0f, 1.0f, 1.0f});
      shader_setUniformFloat(diffuseShader, "u_spotLight.constant", 1.0f);
      shader_setUniformFloat(diffuseShader, "u_spotLight.linear", 0.09f);
      shader_setUniformFloat(diffuseShader, "u_spotLight.quadratic", 0.032f);
      shader_setUniformFloat(diffuseShader, "u_spotLight.cutOff",
                             deg2rad(12.5f));
      shader_setUniformFloat(diffuseShader, "u_spotLight.outerCutOff",
                             deg2rad(15.0f));

      // point light 1
      shader_setUniformV3(diffuseShader, "u_pointLights[1].position",
                          pointLights.positions.data[1]);
      shader_setUniformV3(diffuseShader, "u_pointLights[1].ambient",
                          ambientLight);
      shader_setUniformV3(diffuseShader, "u_pointLights[1].diffuse",
                          (Vector3){0.8f, 0.8f, 0.8f});
      shader_setUniformV3(diffuseShader, "u_pointLights[1].specular",
                          (Vector3){1.0f, 1.0f, 1.0f});
      shader_setUniformFloat(diffuseShader, "u_pointLights[1].constant", 1.0f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[1].linear", 0.09f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[1].quadratic",
                             0.032f);

      // point light 2
      shader_setUniformV3(diffuseShader, "u_pointLights[2].position",
                          pointLights.positions.data[2]);
      shader_setUniformV3(diffuseShader, "u_pointLights[2].ambient",
                          ambientLight);
      shader_setUniformV3(diffuseShader, "u_pointLights[2].diffuse",
                          (Vector3){0.8f, 0.8f, 0.8f});
      shader_setUniformV3(diffuseShader, "u_pointLights[2].specular",
                          (Vector3){1.0f, 1.0f, 1.0f});
      shader_setUniformFloat(diffuseShader, "u_pointLights[2].constant", 1.0f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[2].linear", 0.09f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[2].quadratic",
                             0.032f);

      // point light 3
      shader_setUniformV3(diffuseShader, "u_pointLights[3].position",
                          pointLights.positions.data[3]);
      shader_setUniformV3(diffuseShader, "u_pointLights[3].ambient",
                          ambientLight);
      shader_setUniformV3(diffuseShader, "u_pointLights[3].diffuse",
                          (Vector3){0.8f, 0.8f, 0.8f});
      shader_setUniformV3(diffuseShader, "u_pointLights[3].specular",
                          (Vector3){1.0f, 1.0f, 1.0f});
      shader_setUniformFloat(diffuseShader, "u_pointLights[3].constant", 1.0f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[3].linear", 0.09f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[3].quadratic",
                             0.032f);

      // material
      shader_setUniformInt(diffuseShader, "u_material.diffuse", 0);
      shader_setUniformInt(diffuseShader, "u_material.specular", 1);
      shader_setUniformFloat(diffuseShader, "u_material.shininess", 32.0f);

      // cubes
      for (int i = 0; i < 10; i++) {
        // projection matrix
        shader_setUniformM4(diffuseShader, "u_projectionMatrix", &projection);

        // view matrix
        shader_setUniformM4(diffuseShader, "u_viewMatrix", &cam.viewMatrix);

        // model
        cubes.matrices.data[i] = Matrix4x4_Translation(cubes.positions.data[i]);
        shader_setUniformM4(diffuseShader, "u_modelMatrix",
                            &cubes.matrices.data[i]);

        // printf("==========================================\n");
        // for(size_t i = 0; i < cubes.meshes.VAOs.length; i++)
        //	printf("cube vaos: idx: %ld val: %ld\n", i,
        //cubes.meshes.VAOs.data[i]);

        glBindVertexArray(cubes.meshes.VAOs.data[0]);
        glDrawElements(GL_TRIANGLES, MESH_CUBE_NUM_INDICES, GL_UNSIGNED_INT, 0);
      }

      for (int i = 0; i < 4; i++) {
        glUseProgram(unlitShader);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, lampDiffuse);

        // projection matrix
        shader_setUniformM4(unlitShader, "u_projectionMatrix", &projection);

        // view matrix
        shader_setUniformM4(unlitShader, "u_viewMatrix", &cam.viewMatrix);

        // model matrix
        pointLights.matrices.data[i] = Matrix4x4_Identity();
        pointLights.matrices.data[i] =
            Matrix4x4_Translation(pointLights.positions.data[i]);

        shader_setUniformM4(unlitShader, "u_modelMatrix",
                            &pointLights.matrices.data[i]);

        // color
        shader_setUniformV3(unlitShader, "u_color", pointLights.colors.data[i]);

        // printf("==========================================\n");
        // for(size_t i = 0; i < pointLights.meshes.VAOs.length; i++)
        //	printf("light vaos: idx: %ld val: %ld\n", i,
        //pointLights.meshes.VAOs.data[i]);

        glBindVertexArray(pointLights.meshes.VAOs.data[1]);
        glDrawElements(GL_TRIANGLES, MESH_CUBE_NUM_INDICES, GL_UNSIGNED_INT, 0);
      }

      glfwSwapBuffers(windowData.glfwWindow);
      glfwPollEvents();
    }
  }

  // lights
  List_Vector3_Free(&pointLights.positions);
  List_Vector3_Free(&pointLights.colors);
  List_Quaternion_Free(&pointLights.rotations);
  List_Matrix4x4_Free(&pointLights.matrices);

  mesh_free(&pointLights.meshes);

  // cubes
  List_Vector3_Free(&cubes.positions);
  List_Quaternion_Free(&cubes.rotations);
  List_Vector3_Free(&cubes.colors);
  List_Matrix4x4_Free(&cubes.matrices);

  mesh_free(&cubes.meshes);

  glfwTerminate();
  return 0;
}
