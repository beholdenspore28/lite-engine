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

#include "blib/b_math.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static float currentTime = 0, lastTime = 0, deltaTime = 0, FPS = 0;

typedef struct Transform{
	Matrix4x4 modelMatrix;
	Vector3 position;
	Quaternion rotation;
} Transform;

static inline Vector3 Transform_BasisForward (Transform t, float magnitude) { 
  return Vector3_Rotate(Vector3_Forward(magnitude), t.rotation);
}

static inline Vector3 Transform_BasisUp (Transform t, float magnitude) {
  return Vector3_Rotate(Vector3_Up(magnitude), t.rotation); 
}

static inline Vector3 Transform_BasisRight (Transform t, float magnitude) {
  return Vector3_Rotate(Vector3_Right(magnitude), t.rotation);
}

static inline Vector3 Transform_BasisBack (Transform t, float magnitude) { 
  return Vector3_Rotate(Vector3_Back(magnitude), t.rotation);
}

static inline Vector3 Transform_BasisDown (Transform t, float magnitude) {
  return Vector3_Rotate(Vector3_Down(magnitude), t.rotation); 
}

static inline Vector3 Transform_BasisLeft (Transform t, float magnitude) {
  return Vector3_Rotate(Vector3_Left(magnitude), t.rotation);
}

DECLARE_LIST(Transform)
DEFINE_LIST(Transform)

static const unsigned int NUM_POINT_LIGHTS = 10;
typedef struct pointLight{
	List_Transform transforms;
  List_Vector3 colors;
  mesh meshes;
} pointLight;

static const unsigned int NUM_CUBES = 10;
typedef struct cube{
	List_Transform transforms;
  List_Vector3 colors;
  mesh meshes;
} cube;

typedef struct camera{
	Transform	transform;
  float lookSensitivity;
  float lastX;
  float lastY;
} camera;


int main(void) {
  printf("Rev up those fryers!\n");
  
#if 0 //Quaternion test
	Quaternion q = (Quaternion) { 1,2,3,4 };
  Quaternion_Print(Quaternion_Inverse(q), "inverse PASSED");
  Quaternion_Print(Quaternion_Conjugate(q), "conj PASSES");
  Quaternion_Print(Quaternion_FromEuler(Vector3_Up(1.0)), "FromEuler PASSED");
  Quaternion_Print(Quaternion_Identity(), "id PASSED");
  printf("%6f mag\n", Quaternion_Magnitude(q));
  Quaternion_Print(Quaternion_Multiply(q, q), "mul PASSED");
  Quaternion_Print(Quaternion_Normalize(q), "normalize");
  Matrix4x4_print(Quaternion_ToMatrix4x4(q), "mat");
	return 0;
#endif

  window windowData = window_create();
  glfwSetInputMode(windowData.glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  GLuint diffuseShader = shader_create("res/shaders/diffuse.vs.glsl",
                                       "res/shaders/diffuse.fs.glsl");
  GLuint unlitShader =
      shader_create("res/shaders/unlit.vs.glsl", "res/shaders/unlit.fs.glsl");

  GLuint containerDiffuse = texture_create("res/textures/container2.png");
  GLuint lampDiffuse = texture_create("res/textures/glowstone.png");
  GLuint containerSpecular =
      texture_create("res/textures/container2_specular.png");

  camera cam = {
      .transform.modelMatrix = Matrix4x4_Identity(),
      .transform.position = Vector3_Zero(),
      .transform.rotation = Quaternion_Identity(),
      .lookSensitivity = 10,
      .lastX = 0,
      .lastY = 0,
  };

  cam.transform.position = (Vector3){4, 2, -10};

  // cubes
  cube cubes;
  cubes.transforms = List_Transform_Alloc();
  cubes.colors = List_Vector3_Alloc();

  for (size_t i = 0; i < NUM_CUBES; i++) {
    mesh_allocCube(&cubes.meshes);

		Transform t = (Transform){
			.position = (Vector3){i * 2, 0, 0},
			.rotation = Quaternion_Identity(),
		};

		List_Transform_Add(&cubes.transforms, t);
    List_Vector3_Add(&cubes.colors, Vector3_One(1.0f));
  }

  // lights
  pointLight pointLights;
  pointLights.transforms = List_Transform_Alloc();
  pointLights.colors = List_Vector3_Alloc();

  for (size_t i = 0; i < NUM_POINT_LIGHTS; i++) {
    mesh_allocCube(&pointLights.meshes);

    Vector3 color = Vector3_One(0.5f);

		Transform t = (Transform) {
			.position = (Vector3){i * 16, -2, -2},
			.rotation = Quaternion_Identity(),
			.modelMatrix = Matrix4x4_Identity(),
		};

    List_Vector3_Add(&pointLights.colors, color);
		List_Transform_Add(&pointLights.transforms, t);
  }

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  (void)FPS;
  float aspect;
  Matrix4x4 projection = Matrix4x4_Identity();
  Vector3 ambientLight = Vector3_One(0.2f);

  Vector3 look = Vector3_Zero();

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
        double mouseX, mouseY;
        glfwGetCursorPos(windowData.glfwWindow, &mouseX, &mouseY);
        
        if (firstMouse) {
          cam.lastX = mouseX;
          cam.lastY = mouseY;
          firstMouse = false;
        }

        float xoffset = mouseX - cam.lastX;
        float yoffset = mouseY - cam.lastY;
				cam.lastX = mouseX;
        cam.lastY = mouseY;
        float xangle = xoffset * deltaTime * cam.lookSensitivity;
        float yangle = yoffset * deltaTime * cam.lookSensitivity;

        look.x += yangle;
        look.y += xangle;

				look.y = loop(look.y, 2*PI);
				look.x = clamp(look.x, -PI * 0.5, PI * 0.5);

				cam.transform.rotation = Quaternion_FromEuler(look);
      }

      { // movement
        float cameraSpeed = 15 * deltaTime;
#if 0 //old
        Vector3 velocity = Vector3_Zero();
        if (glfwGetKey(windowData.glfwWindow, GLFW_KEY_D)) {
					velocity = Vector3_Add(velocity, Transform_BasisRight(cam.transform, cameraSpeed));
				}

        if (glfwGetKey(windowData.glfwWindow, GLFW_KEY_A)) {
					velocity = Vector3_Add(velocity, Transform_BasisLeft(cam.transform, cameraSpeed));
				}

        if (glfwGetKey(windowData.glfwWindow, GLFW_KEY_SPACE)) {
					velocity = Vector3_Add(velocity, Transform_BasisUp(cam.transform, cameraSpeed));
				}

        if (glfwGetKey(windowData.glfwWindow, GLFW_KEY_LEFT_SHIFT)) {
					velocity = Vector3_Add(velocity, Transform_BasisDown(cam.transform, cameraSpeed));
				}

        if (glfwGetKey(windowData.glfwWindow, GLFW_KEY_W)) {
					velocity = Vector3_Add(velocity, Transform_BasisForward(cam.transform, cameraSpeed));
				}

				Vector3_Print(Transform_BasisRight(cam.transform, 1), "cam x");
				Vector3_Print(Transform_BasisUp(cam.transform, 1), "cam y");
				Vector3_Print(Transform_BasisForward(cam.transform, 1), "cam z");
				puts("");

        if (glfwGetKey(windowData.glfwWindow, GLFW_KEY_S)) {
					velocity = Vector3_Add(velocity, Transform_BasisBack(cam.transform, cameraSpeed));
				}

				cam.transform.position = Vector3_Add(cam.transform.position, velocity);
				//Vector3_Print(cam.transform.position, "position");
				//Vector3_Print(velocity, "velocity");
#else //new
				Vector3 movement = Vector3_Zero();

				movement.x = glfwGetKey(windowData.glfwWindow, GLFW_KEY_D) -
										glfwGetKey(windowData.glfwWindow, GLFW_KEY_A);
				movement.y = glfwGetKey(windowData.glfwWindow, GLFW_KEY_SPACE) -
										glfwGetKey(windowData.glfwWindow, GLFW_KEY_LEFT_SHIFT);
				movement.z = glfwGetKey(windowData.glfwWindow, GLFW_KEY_W) -
										glfwGetKey(windowData.glfwWindow, GLFW_KEY_S);

				movement = Vector3_Normalize(movement);
				movement = Vector3_Scale(movement, cameraSpeed);
				movement = Vector3_Rotate(movement, cam.transform.rotation);

				cam.transform.position = Vector3_Add(cam.transform.position, movement);
#endif
      }
    }

    glfwGetWindowSize(windowData.glfwWindow, &windowData.width,
                      &windowData.height);
    aspect = (float)windowData.width / (float)windowData.height;
    projection = Matrix4x4_Perspective(deg2rad(90), aspect, 0.1f, 1000.0f);

    { // draw
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // view matrix
      cam.transform.modelMatrix = Matrix4x4_Translation(Vector3_Negate(cam.transform.position));
      cam.transform.modelMatrix = Matrix4x4_Multiply(cam.transform.modelMatrix,
                                          Quaternion_ToMatrix4x4(Quaternion_Conjugate(cam.transform.rotation)));
      //Matrix4x4_print(cam.transform.modelMatrix, "view");

      glUseProgram(diffuseShader);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, containerDiffuse);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, containerSpecular);

      // camera
      shader_setUniformV3(diffuseShader, "u_cameraPos", cam.transform.position);

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
                          pointLights.transforms.data[0].position);
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
      shader_setUniformV3(diffuseShader, "u_spotLight.position", cam.transform.position);
      shader_setUniformV3(
          diffuseShader, "u_spotLight.direction",
          Transform_BasisForward(cam.transform, 1.0));

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
                          pointLights.transforms.data[1].position);
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
                          pointLights.transforms.data[2].position);
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
                          pointLights.transforms.data[3].position);
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
      for (unsigned int i = 0; i < NUM_CUBES; i++) {
        // projection matrix
        shader_setUniformM4(diffuseShader, "u_projectionMatrix", &projection);

        // view matrix
        shader_setUniformM4(diffuseShader, "u_viewMatrix", &cam.transform.modelMatrix);

        // model
        cubes.transforms.data[i].modelMatrix = Matrix4x4_Translation(cubes.transforms.data[i].position);
        shader_setUniformM4(diffuseShader, "u_modelMatrix",
                            &cubes.transforms.data[i].modelMatrix);

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
        shader_setUniformM4(unlitShader, "u_viewMatrix", &cam.transform.modelMatrix);

        // model matrix
        pointLights.transforms.data[i].modelMatrix = Matrix4x4_Identity();
        pointLights.transforms.data[i].modelMatrix =
            Matrix4x4_Translation(pointLights.transforms.data[i].position);

        shader_setUniformM4(unlitShader, "u_modelMatrix",
                            &pointLights.transforms.data[i].modelMatrix);

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
  List_Transform_Free(&pointLights.transforms);
  List_Vector3_Free(&pointLights.colors);

  mesh_free(&pointLights.meshes);

  // cubes
  List_Transform_Free(&cubes.transforms);
  List_Vector3_Free(&cubes.colors);

  mesh_free(&cubes.meshes);

  glfwTerminate();
  return 0;
}
