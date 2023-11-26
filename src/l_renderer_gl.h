#ifndef L_RENDERER_GL_H
#define L_RENDERER_GL_H

#include "glad/glad.h"
#include "blib_math.h"
#include "GLFW/glfw3.h"

/*RUNTIME*********************************************************************/

typedef struct {
	GLFWwindow* window;
	int windowWidth;
	int windowHeight;
	float aspectRatio;

	double frameStartTime;
	double frameEndTime;
	double deltaTime;
}l_runtime_data;

l_runtime_data l_runtime_init(void);
void l_runtime_cleanup(l_runtime_data* d);

/*MESH************************************************************************/

#define _L_CUBE_NUM_VERTS 64
#define _L_CUBE_NUM_INDICES 36

typedef struct {
	GLfloat* vertexData;
	GLuint* indexData;
	GLuint numVertices;
	GLuint numIndices;
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
} l_mesh;

l_mesh l_mesh_create(
		GLuint numIndices,
		GLuint numVertices,
		GLuint* indexData, 
		GLfloat* vertexData
		);
l_mesh l_mesh_createCube();
void l_mesh_render(l_mesh* m);

/*SHADER**********************************************************************/

GLuint l_shader_create();

/*TEXTURE*********************************************************************/

//TODO make a destroy texture func to clean this up
GLuint lite_gl_texture_create(const char* imageFile);

/*TRANSFORM*******************************************************************/

typedef struct {
	blib_vec3f_t position;
	blib_vec3f_t eulerAngles;
	blib_vec3f_t scale;

} lite_gl_transform_t;

lite_gl_transform_t lite_gl_transform_create();
blib_mat4_t lite_gl_transform_GetMatrix(lite_gl_transform_t* t);
blib_vec3f_t lite_transform_getLocalForward(lite_gl_transform_t* t);
blib_vec3f_t lite_transform_getLocalUp(lite_gl_transform_t* t);
blib_vec3f_t lite_transform_getLocalRight(lite_gl_transform_t* t);
void lite_gl_transform_rotate(lite_gl_transform_t* t, blib_vec3f_t rotation);

/*CAMERA**********************************************************************/

typedef struct lite_gl_camera_t lite_gl_camera_t;
struct lite_gl_camera_t {
	lite_gl_transform_t transform;
	blib_mat4_t projectionMatrix;
	blib_mat4_t viewMatrix;
	float fov;
};

lite_gl_camera_t lite_gl_camera_create(float fov);
blib_mat4_t lite_gl_camera_GetViewMatrix(lite_gl_transform_t* t);
void lite_gl_camera_setProjectionMatrix(lite_gl_camera_t* cam, float aspect);
void lite_gl_camera_update(lite_gl_camera_t* cam,l_runtime_data* d);

#endif /*L_RENDERER_H*/
