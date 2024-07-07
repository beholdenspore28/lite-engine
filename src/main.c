#include "gl.h"
#include <stdio.h>
#include <GLFW/glfw3.h>

#include "blib/b_list.h"
B_LIST_IMPLEMENTATION 
DECLARE_LIST(vec3)
DEFINE_LIST(vec3)
DECLARE_LIST(mat4)
DEFINE_LIST(mat4)

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static float currentTime = 0, lastTime = 0, deltaTime = 0, FPS = 0;

typedef struct {
	list_mat4 matrices;
	list_vec3 positions;
	list_vec3 colors;
	mesh meshes;
} pointLight;

typedef struct {
	list_mat4 matrices;
	list_vec3 positions;
	list_vec3 eulers;
	list_vec3 colors;
	mesh meshes;
} cube;
  
//TODO enclose camera data in a struct
typedef struct {
	vec3	position;
	vec3	eulers;
	float	lookSensitivity;
	float	lastX;
	float	lastY;
} camera;

int main(void) {
  printf("Rev up those fryers!\n");

  window windowData = window_create();
  //glfwSetCursorPosCallback(windowData.glfwWindow, mouse_callback);
  
  GLuint diffuseShader = shader_create("res/shaders/diffuse.vs.glsl",
                                       "res/shaders/diffuse.fs.glsl");
  GLuint unlitShader =
      shader_create("res/shaders/unlit.vs.glsl", "res/shaders/unlit.fs.glsl");

  GLuint containerDiffuse = texture_create("res/textures/container2.png");
  GLuint lampDiffuse = texture_create("res/textures/glowstone.png");
  GLuint containerSpecular =
      texture_create("res/textures/container2_specular.png");

	camera cam = {
		.position = VEC3_ZERO,
		.eulers = VEC3_ZERO,
		.lookSensitivity = 10,
		.lastX = 0,
		.lastY = 0,
	};

  cam.position = (vec3) { 4, 2, -10 };

  //cubes
	cube cubes;
	cubes.matrices = list_mat4_alloc();
	cubes.positions = list_vec3_alloc();
	cubes.eulers = list_vec3_alloc();
	cubes.colors = list_vec3_alloc();

  for (size_t i = 0; i < 10; i++) {
		mesh_allocCube(&cubes.meshes);

		vec3 pos = (vec3) {i * 2, 0, 0};
		list_vec3_add(&cubes.positions, pos);
		list_vec3_add(&cubes.eulers, VEC3_ZERO);
		list_vec3_add(&cubes.colors, VEC3_ONE);
		list_mat4_add(&cubes.matrices, MAT4_IDENTITY);
	}

	//lights
	pointLight pointLights;
	pointLights.matrices = list_mat4_alloc();
	pointLights.positions = list_vec3_alloc();
	pointLights.colors = list_vec3_alloc();

  for (size_t i = 0; i < 10; i++) {
    mesh_allocCube(&pointLights.meshes);

		vec3 color = vec3_scale(VEC3_ONE, 0.5f);
		vec3 pos = (vec3) {i * 16, -2, -2};
		list_vec3_add(&pointLights.colors, color);
		list_vec3_add(&pointLights.positions, pos);
		list_mat4_add(&pointLights.matrices, MAT4_IDENTITY);
	}

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  (void)FPS;
  mat4 view = MAT4_IDENTITY;
  float aspect;
  mat4 projection = MAT4_IDENTITY;
  vec3 ambientLight = vec3_scale(VEC3_ONE, 0.2f);

  while (!glfwWindowShouldClose(windowData.glfwWindow)) {
    { // TIME
      currentTime = glfwGetTime();
      deltaTime = currentTime - lastTime;
      lastTime = currentTime;

      FPS = 1 / deltaTime;
      //printf("============FRAME=START==============\n");
      //printf("delta %f : FPS %f\n", deltaTime, FPS);
		}

    { // INPUT
      // camera
			{ //mouse look
				static bool firstMouse = true;

				double x, y; 
				glfwGetCursorPos(windowData.glfwWindow, &x, &y);

				if (firstMouse) {
					cam.lastX = x;
					cam.lastY = y;
					firstMouse = false;
				}

				float xoffset = x - cam.lastX;
				//float yoffset = cam.lastY - y; // reversed since y-coordinates go from bottom to top
				cam.lastX = x;
				//cam.lastY = y;

				cam.eulers.y -= xoffset * deltaTime * cam.lookSensitivity;
			}

			{ // movement
				float cameraSpeed = 15 * deltaTime;
				vec3 velocity = VEC3_ZERO;

				int xaxis = glfwGetKey(windowData.glfwWindow, GLFW_KEY_D) -
					glfwGetKey(windowData.glfwWindow, GLFW_KEY_A);

				int yaxis = glfwGetKey(windowData.glfwWindow, GLFW_KEY_SPACE) -
					glfwGetKey(windowData.glfwWindow, GLFW_KEY_LEFT_SHIFT);

				int zaxis = glfwGetKey(windowData.glfwWindow, GLFW_KEY_W) -
					glfwGetKey(windowData.glfwWindow, GLFW_KEY_S);

				velocity.x = cameraSpeed * xaxis;
				velocity.y = cameraSpeed * yaxis;
				velocity.z = cameraSpeed * zaxis; 

				//velocity = mat4_multiplyVec3(velocity, view);

				cam.position = vec3_add(cam.position, velocity);
			}
    }

    glfwGetWindowSize(windowData.glfwWindow, &windowData.width,
                      &windowData.height);
    aspect = (float)windowData.width / (float)windowData.height;
    projection = mat4_perspective(deg2rad(60), aspect, 0.1f, 1000.0f);

    { // draw
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // view matrix
      view = mat4_translateVec3(vec3_negate(cam.position));

      glUseProgram(diffuseShader);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, containerDiffuse);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, containerSpecular);

      //camera
      shader_setUniformV3(diffuseShader, "u_cameraPos", cam.position);

      // directional light
      shader_setUniformV3(diffuseShader, "u_dirLight.direction",
                          (vec3){-0.2f, -1.0f, -0.3f});
      shader_setUniformV3(diffuseShader, "u_dirLight.ambient",
                          ambientLight);
      shader_setUniformV3(diffuseShader, "u_dirLight.diffuse",
                          (vec3){0.4f, 0.4f, 0.4f});
      shader_setUniformV3(diffuseShader, "u_dirLight.specular",
                          (vec3){0.5f, 0.5f, 0.5f});

      // point light 0
      shader_setUniformV3(diffuseShader, "u_pointLights[0].position",
                          pointLights.positions.data[0]);
      shader_setUniformV3(diffuseShader, "u_pointLights[0].ambient",
                          ambientLight);
      shader_setUniformV3(diffuseShader, "u_pointLights[0].diffuse",
                          (vec3){0.8f, 0.8f, 0.8f});
      shader_setUniformV3(diffuseShader, "u_pointLights[0].specular",
                          (vec3){1.0f, 1.0f, 1.0f});
      shader_setUniformFloat(diffuseShader, "u_pointLights[0].constant", 1.0f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[0].linear", 0.09f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[0].quadratic", 0.032f);

      // spot light
      shader_setUniformV3(diffuseShader, "u_spotLight.position",
                          cam.position);
      shader_setUniformV3(diffuseShader, "u_spotLight.direction", VEC3_BACK);
      shader_setUniformV3(diffuseShader, "u_spotLight.ambient",
                          ambientLight);
      shader_setUniformV3(diffuseShader, "u_spotLight.diffuse",
                          (vec3){1.0f, 1.0f, 1.0f});
      shader_setUniformV3(diffuseShader, "u_spotLight.specular",
                          (vec3){1.0f, 1.0f, 1.0f});
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
                          (vec3){0.8f, 0.8f, 0.8f});
      shader_setUniformV3(diffuseShader, "u_pointLights[1].specular",
                          (vec3){1.0f, 1.0f, 1.0f});
      shader_setUniformFloat(diffuseShader, "u_pointLights[1].constant", 1.0f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[1].linear", 0.09f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[1].quadratic", 0.032f);

      // point light 2
      shader_setUniformV3(diffuseShader, "u_pointLights[2].position",
                          pointLights.positions.data[2]);
      shader_setUniformV3(diffuseShader, "u_pointLights[2].ambient",
                          ambientLight);
      shader_setUniformV3(diffuseShader, "u_pointLights[2].diffuse",
                          (vec3){0.8f, 0.8f, 0.8f});
      shader_setUniformV3(diffuseShader, "u_pointLights[2].specular",
                          (vec3){1.0f, 1.0f, 1.0f});
      shader_setUniformFloat(diffuseShader, "u_pointLights[2].constant", 1.0f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[2].linear", 0.09f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[2].quadratic", 0.032f);

      // point light 3
      shader_setUniformV3(diffuseShader, "u_pointLights[3].position",
                          pointLights.positions.data[3]);
      shader_setUniformV3(diffuseShader, "u_pointLights[3].ambient",
                          ambientLight);
      shader_setUniformV3(diffuseShader, "u_pointLights[3].diffuse",
                          (vec3){0.8f, 0.8f, 0.8f});
      shader_setUniformV3(diffuseShader, "u_pointLights[3].specular",
                          (vec3){1.0f, 1.0f, 1.0f});
      shader_setUniformFloat(diffuseShader, "u_pointLights[3].constant", 1.0f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[3].linear", 0.09f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[3].quadratic", 0.032f);

      //material
      shader_setUniformInt(diffuseShader, "u_material.diffuse", 0);
      shader_setUniformInt(diffuseShader, "u_material.specular", 1);
      shader_setUniformFloat(diffuseShader, "u_material.shininess", 32.0f);

      // cubes
      for (int i = 0; i < 10; i++) {
        // projection matrix
        shader_setUniformM4(diffuseShader, "u_projectionMatrix", &projection);

        // view matrix
        shader_setUniformM4(diffuseShader, "u_viewMatrix", &view);

        //model
        cubes.matrices.data[i] = mat4_translateVec3(cubes.positions.data[i]);
        shader_setUniformM4(diffuseShader, "u_modelMatrix", &cubes.matrices.data[i]);

				//printf("==========================================\n");
				//for(size_t i = 0; i < cubes.meshes.VAOs.length; i++)
				//	printf("cube vaos: idx: %ld val: %ld\n", i, cubes.meshes.VAOs.data[i]);

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
        shader_setUniformM4(unlitShader, "u_viewMatrix", &view);

        // model matrix
        pointLights.matrices.data[i] = MAT4_IDENTITY;
        pointLights.matrices.data[i] = mat4_translateVec3(pointLights.positions.data[i]);

        shader_setUniformM4(unlitShader, "u_modelMatrix", &pointLights.matrices.data[i]);

        // color
        shader_setUniformV3(unlitShader, "u_color", pointLights.colors.data[i]);

				//printf("==========================================\n");
				//for(size_t i = 0; i < pointLights.meshes.VAOs.length; i++)
				//	printf("light vaos: idx: %ld val: %ld\n", i, pointLights.meshes.VAOs.data[i]);

        glBindVertexArray(pointLights.meshes.VAOs.data[1]);
        glDrawElements(GL_TRIANGLES, MESH_CUBE_NUM_INDICES, GL_UNSIGNED_INT, 0);
      }

      glfwSwapBuffers(windowData.glfwWindow);
      glfwPollEvents();
    }
  }

	//lights
	list_vec3_free(&pointLights.positions);
	list_vec3_free(&pointLights.colors);
	list_mat4_free(&pointLights.matrices);

	mesh_free(&pointLights.meshes);

  //cubes
	list_vec3_free(&cubes.positions);
	list_vec3_free(&cubes.eulers);
	list_vec3_free(&cubes.colors);
	list_mat4_free(&cubes.matrices);

	mesh_free(&cubes.meshes);

  glfwTerminate();
  return 0;
}
