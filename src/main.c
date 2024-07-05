#include "gl.h"
#include <stdio.h>
#include <GLFW/glfw3.h>

#include "blib/b_list.h"
B_LIST_IMPLEMENTATION 

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

vec3 cameraPosition = VEC3_ZERO;
vec3 cameraEulers = VEC3_ZERO;
float cameraLookSensitivity = 10;
#define NUM_POINT_LIGHTS 10
vec3 pointLightPositions[NUM_POINT_LIGHTS] = {[0 ... NUM_POINT_LIGHTS- 1] = VEC3_ZERO};
vec3 pointLightColors[NUM_POINT_LIGHTS] = {[0 ... NUM_POINT_LIGHTS- 1] = VEC3_ZERO};

#define NUM_CUBES 10
vec3 cubePositions[NUM_CUBES] = {[0 ... NUM_CUBES- 1] = VEC3_ZERO};
vec3 cubeEulers[NUM_CUBES] = {[0 ... NUM_CUBES- 1] = VEC3_ZERO};
vec3 cubeColors[NUM_CUBES] = {[0 ... NUM_CUBES- 1] = VEC3_ZERO};

static float currentTime = 0, lastTime = 0, deltaTime = 0, FPS = 0;
  
void camera_mouseLook(float xoffset, float yoffset) {
	(void)yoffset;
	cameraEulers.y -= xoffset * deltaTime * cameraLookSensitivity;
}

static float lastX = 0;
static float lastY = 0;

void mouse_callback(GLFWwindow* windowData, double xposIn, double yposIn) {
	(void)windowData;
	static bool firstMouse = true;
  float xpos = (float)xposIn;
  float ypos = (float)yposIn;
  
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }
  
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
  lastX = xpos;
  lastY = ypos;
  
  camera_mouseLook(xoffset, yoffset);
}

int main() {

  printf("Rev up those fryers!\n");

  window windowData = window_create();
  glfwSetCursorPosCallback(windowData.glfwWindow, mouse_callback);
  
  GLuint diffuseShader = shader_create("res/shaders/diffuse.vs.glsl",
                                       "res/shaders/diffuse.fs.glsl");
  GLuint unlitShader =
      shader_create("res/shaders/unlit.vs.glsl", "res/shaders/unlit.fs.glsl");

  GLuint containerDiffuse = texture_create("res/textures/container2.png");
  GLuint lampDiffuse = texture_create("res/textures/glowstone.png");
  GLuint containerSpecular =
      texture_create("res/textures/container2_specular.png");

  cameraPosition.x = 4;
  cameraPosition.y = 2;
  cameraPosition.z = -10;

  //cubes
  for (int i = 0; i < 10; i++) {
    mesh_createCube(i);

    cubePositions[i].x = i * 2;
    cubePositions[i].y = 0;
    cubePositions[i].z = 0;

    cubeEulers[i].x = 0;
    cubeEulers[i].y = 0;
    cubeEulers[i].z = 0;

    cubeColors[i].x = 1;
    cubeColors[i].y = 1;
    cubeColors[i].z = 1;
  }

  //point lights
  for (int i = 0; i < 4; i++) {
    pointLightColors[i].x = 0.5f;
    pointLightColors[i].y = 0.5f;
    pointLightColors[i].z = 0.5f;

    pointLightPositions[i].x = i * 16;
    pointLightPositions[i].y = -2;
    pointLightPositions[i].z = -2;
  }

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  (void)FPS;
  mat4 view = MAT4_IDENTITY;
  mat4 model = MAT4_IDENTITY;
  float aspect;
  mat4 projection = MAT4_IDENTITY;
  vec3 ambientLight = vec3_scale(VEC3_ONE, 0.2f);

  while (!glfwWindowShouldClose(windowData.glfwWindow)) {
    { // TIME
      currentTime = glfwGetTime();
      deltaTime = currentTime - lastTime;
      lastTime = currentTime;

      FPS = 1 / deltaTime;
      printf("============FRAME=START==============\n");
      printf("delta %f : FPS %f\n", deltaTime, FPS);
    }

    { // INPUT

      // camera
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

			velocity = mat4_multiplyVec3(velocity, model);

			cameraPosition = vec3_add(cameraPosition, velocity);

    }

    glfwGetWindowSize(windowData.glfwWindow, &windowData.width,
                      &windowData.height);
    aspect = (float)windowData.width / (float)windowData.height;
    projection = mat4_perspective(deg2rad(60), aspect, 0.1f, 1000.0f);

    { // draw
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // view matrix
      view = mat4_translateVec3(vec3_negate(cameraPosition));
      mat4 viewPitch = mat4_rotate(cameraEulers.x, VEC3_RIGHT);
      mat4 viewYaw = mat4_rotate(cameraEulers.y, VEC3_UP);
      mat4 viewRoll = mat4_rotate(cameraEulers.z, VEC3_FORWARD);
      mat4 viewRotation = MAT4_IDENTITY;
      viewRotation = mat4_multiply(viewPitch, mat4_multiply(viewYaw, viewRoll));
      view = mat4_multiply(view, viewRotation);

      glUseProgram(diffuseShader);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, containerDiffuse);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, containerSpecular);

      //camera
      shader_setUniformV3(diffuseShader, "u_cameraPos", cameraPosition);

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
                          pointLightPositions[0]);
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
                          cameraPosition);
      shader_setUniformV3(diffuseShader, "u_spotLight.direction", VEC3_FORWARD);
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
                          pointLightPositions[1]);
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
                          pointLightPositions[2]);
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
                          pointLightPositions[3]);
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
        model = MAT4_IDENTITY;
        model = mat4_translateVec3(cubePositions[i]);
        mat4 pitch = mat4_rotate(cubeEulers[i].x, VEC3_RIGHT);
        mat4 yaw = mat4_rotate(cubeEulers[i].y, VEC3_UP);
        mat4 roll = mat4_rotate(cubeEulers[i].z, VEC3_FORWARD);
        mat4 rotation = MAT4_IDENTITY;
        rotation = mat4_multiply(pitch, mat4_multiply(yaw, roll));
        model = mat4_multiply(rotation, model);
        shader_setUniformM4(diffuseShader, "u_modelMatrix", &model);

        glBindVertexArray(mesh_getVAO(0));
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
        model = MAT4_IDENTITY;
        model = mat4_translateVec3(pointLightPositions[i]);
        shader_setUniformM4(unlitShader, "u_modelMatrix", &model);

        // color
        shader_setUniformV3(unlitShader, "u_color", pointLightColors[i]);

        glBindVertexArray(mesh_getVAO(0));
        glDrawElements(GL_TRIANGLES, MESH_CUBE_NUM_INDICES, GL_UNSIGNED_INT, 0);
      }

      glfwSwapBuffers(windowData.glfwWindow);
      glfwPollEvents();
    }
  }

  glfwTerminate();
  return 0;
}
