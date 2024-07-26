#include "gl.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "blib/b_list.h"
B_LIST_IMPLEMENTATION
DECLARE_LIST(vector3_t)
DEFINE_LIST(vector3_t)
DECLARE_LIST(matrix4_t)
DEFINE_LIST(matrix4_t)
DECLARE_LIST(quaternion_t)
DEFINE_LIST(quaternion_t)

#include "blib/b_math.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static float currentTime = 0, lastTime = 0, deltaTime = 0, FPS = 0;

typedef struct {
	matrix4_t modelMatrix;
	vector3_t position;
	quaternion_t rotation;
} transform_t;

static inline vector3_t transform_basis_forward (transform_t t, float magnitude) { 
  return vector3_rotate(vector3_forward(magnitude), t.rotation);
}

static inline vector3_t transform_basis_up (transform_t t, float magnitude) {
  return vector3_rotate(vector3_up(magnitude), t.rotation); 
}

static inline vector3_t transform_basis_right (transform_t t, float magnitude) {
  return vector3_rotate(vector3_right(magnitude), t.rotation);
}

static inline vector3_t transform_basis_back (transform_t t, float magnitude) { 
  return vector3_rotate(vector3_back(magnitude), t.rotation);
}

static inline vector3_t transform_basis_down (transform_t t, float magnitude) {
  return vector3_rotate(vector3_down(magnitude), t.rotation); 
}

static inline vector3_t transform_basis_left (transform_t t, float magnitude) {
  return vector3_rotate(vector3_left(magnitude), t.rotation);
}

DECLARE_LIST(transform_t)
DEFINE_LIST(transform_t)

static const unsigned int NUM_POINT_LIGHTS = 10;
typedef struct {
	list_transform_t transforms;
  list_vector3_t colors;
  mesh meshes;
} pointLight_t;

static const unsigned int NUM_CUBES = 10;
typedef struct {
	list_transform_t transforms;
  list_vector3_t colors;
  mesh meshes;
} cube_t;

typedef struct {
	transform_t	transform;
  float lookSensitivity;
  float lastX;
  float lastY;
} camera_t;


int main(void) {
  printf("Rev up those fryers!\n");
  
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

  camera_t cam = {
      .transform.modelMatrix = matrix4_identity(),
      .transform.position = vector3_zero(),
      .transform.rotation = quaternion_identity(),
      .lookSensitivity = 10,
      .lastX = 0,
      .lastY = 0,
  };

  cam.transform.position = (vector3_t){4, 2, -10};

  // cubes
  cube_t cubes;
  cubes.transforms = list_transform_t_alloc();
  cubes.colors = list_vector3_t_alloc();

  for (size_t i = 0; i < NUM_CUBES; i++) {
    mesh_allocCube(&cubes.meshes);

		transform_t t = (transform_t){
			.position = (vector3_t){i * 2, 0, 0},
			.rotation = quaternion_identity(),
		};

		list_transform_t_add(&cubes.transforms, t);
    list_vector3_t_add(&cubes.colors, vector3_one(1.0f));
  }

  // lights
  pointLight_t pointLights;
  pointLights.transforms = list_transform_t_alloc();
  pointLights.colors = list_vector3_t_alloc();

  for (size_t i = 0; i < NUM_POINT_LIGHTS; i++) {
    mesh_allocCube(&pointLights.meshes);

    vector3_t color = vector3_one(0.5f);

		transform_t t = (transform_t) {
			.position = (vector3_t){i * 16, -2, -2},
			.rotation = quaternion_identity(),
			.modelMatrix = matrix4_identity(),
		};

    list_vector3_t_add(&pointLights.colors, color);
		list_transform_t_add(&pointLights.transforms, t);
  }

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  (void)FPS;
  float aspect;
  matrix4_t projection = matrix4_identity();
  vector3_t ambientLight = vector3_one(0.2f);

  vector3_t look = vector3_zero();

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

				look.y = loop(look.y, 2 * PI);
				look.x = clamp(look.x, -PI * 0.5, PI * 0.5);

				cam.transform.rotation = quaternion_from_euler(look);
      }

      { // movement
        float cameraSpeed = 15 * deltaTime;
				vector3_t movement = vector3_zero();

				movement.x = glfwGetKey(windowData.glfwWindow, GLFW_KEY_D) -
										glfwGetKey(windowData.glfwWindow, GLFW_KEY_A);
				movement.y = glfwGetKey(windowData.glfwWindow, GLFW_KEY_SPACE) -
										glfwGetKey(windowData.glfwWindow, GLFW_KEY_LEFT_SHIFT);
				movement.z = glfwGetKey(windowData.glfwWindow, GLFW_KEY_W) -
										glfwGetKey(windowData.glfwWindow, GLFW_KEY_S);

				movement = vector3_normalize(movement);
				movement = vector3_scale(movement, cameraSpeed);
				movement = vector3_rotate(movement, cam.transform.rotation);

				cam.transform.position = vector3_add(cam.transform.position, movement);
      }
    }

    glfwGetWindowSize(windowData.glfwWindow, &windowData.width,
                      &windowData.height);
    aspect = (float)windowData.width / (float)windowData.height;
    projection = matrix4_perspective(deg2rad(90), aspect, 0.1f, 1000.0f);

    { // draw
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // view matrix
      cam.transform.modelMatrix = matrix4_translation(vector3_negate(cam.transform.position));
      cam.transform.modelMatrix = matrix4_multiply(cam.transform.modelMatrix,
                                          quaternion_to_matrix4(quaternion_conjugate(cam.transform.rotation)));
      //matrix4_print(cam.transform.modelMatrix, "view");

      glUseProgram(diffuseShader);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, containerDiffuse);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, containerSpecular);

      // camera
      shader_setUniformV3(diffuseShader, "u_cameraPos", cam.transform.position);

      // directional light
      shader_setUniformV3(diffuseShader, "u_dirLight.direction",
                          (vector3_t){-0.2f, -1.0f, -0.3f});
      shader_setUniformV3(diffuseShader, "u_dirLight.ambient", ambientLight);
      shader_setUniformV3(diffuseShader, "u_dirLight.diffuse",
                          (vector3_t){0.4f, 0.4f, 0.4f});
      shader_setUniformV3(diffuseShader, "u_dirLight.specular",
                          (vector3_t){0.5f, 0.5f, 0.5f});

      // point light 0
      shader_setUniformV3(diffuseShader, "u_pointLights[0].position",
                          pointLights.transforms.data[0].position);
      shader_setUniformV3(diffuseShader, "u_pointLights[0].ambient",
                          ambientLight);
      shader_setUniformV3(diffuseShader, "u_pointLights[0].diffuse",
                          (vector3_t){0.8f, 0.8f, 0.8f});
      shader_setUniformV3(diffuseShader, "u_pointLights[0].specular",
                          (vector3_t){1.0f, 1.0f, 1.0f});
      shader_setUniformFloat(diffuseShader, "u_pointLights[0].constant", 1.0f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[0].linear", 0.09f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[0].quadratic",
                             0.032f);

      // spot light
      shader_setUniformV3(diffuseShader, "u_spotLight.position", cam.transform.position);
      shader_setUniformV3(
          diffuseShader, "u_spotLight.direction",
          transform_basis_left(cam.transform, 1.0));

      shader_setUniformV3(diffuseShader, "u_spotLight.ambient", ambientLight);
      shader_setUniformV3(diffuseShader, "u_spotLight.diffuse",
                          (vector3_t){1.0f, 1.0f, 1.0f});
      shader_setUniformV3(diffuseShader, "u_spotLight.specular",
                          (vector3_t){1.0f, 1.0f, 1.0f});
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
                          (vector3_t){0.8f, 0.8f, 0.8f});
      shader_setUniformV3(diffuseShader, "u_pointLights[1].specular",
                          (vector3_t){1.0f, 1.0f, 1.0f});
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
                          (vector3_t){0.8f, 0.8f, 0.8f});
      shader_setUniformV3(diffuseShader, "u_pointLights[2].specular",
                          (vector3_t){1.0f, 1.0f, 1.0f});
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
                          (vector3_t){0.8f, 0.8f, 0.8f});
      shader_setUniformV3(diffuseShader, "u_pointLights[3].specular",
                          (vector3_t){1.0f, 1.0f, 1.0f});
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
        cubes.transforms.data[i].modelMatrix = matrix4_translation(cubes.transforms.data[i].position);
        shader_setUniformM4(diffuseShader, "u_modelMatrix",
                            &cubes.transforms.data[i].modelMatrix);

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
        pointLights.transforms.data[i].modelMatrix = matrix4_identity();
        pointLights.transforms.data[i].modelMatrix =
            matrix4_translation(pointLights.transforms.data[i].position);

        shader_setUniformM4(unlitShader, "u_modelMatrix",
                            &pointLights.transforms.data[i].modelMatrix);

        // color
        shader_setUniformV3(unlitShader, "u_color", pointLights.colors.data[i]);

        glBindVertexArray(pointLights.meshes.VAOs.data[1]);
        glDrawElements(GL_TRIANGLES, MESH_CUBE_NUM_INDICES, GL_UNSIGNED_INT, 0);
      }

      glfwSwapBuffers(windowData.glfwWindow);
      glfwPollEvents();
    }
  }

  // lights
  list_transform_t_free(&pointLights.transforms);
  list_vector3_t_free(&pointLights.colors);

  mesh_free(&pointLights.meshes);

  // cubes
  list_transform_t_free(&cubes.transforms);
  list_vector3_t_free(&cubes.colors);

  mesh_free(&cubes.meshes);

  glfwTerminate();
  return 0;
}
