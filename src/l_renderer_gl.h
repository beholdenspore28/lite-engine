#ifndef L_RENDERER_GL_H
#define L_RENDERER_GL_H

#include "glad/glad.h"
#include "blib_math.h"
#include "GLFW/glfw3.h"

/*TYPES***********************************************************************/

typedef struct l_renderer_gl l_renderer_gl;
typedef struct l_renderer_gl_camera l_renderer_gl_camera;
typedef struct l_renderer_gl_mesh l_renderer_gl_mesh;
typedef struct l_renderer_gl_transform l_renderer_gl_transform;
typedef struct l_renderer_gl_camera l_renderer_gl_camera;

#define L_CUBE_NUM_VERTS 192
#define L_CUBE_NUM_INDICES 36

struct l_renderer_gl_mesh{
	GLfloat* vertexData;
	GLuint* indexData;
	GLuint numVertices;
	GLuint numIndices;
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
} ;

l_renderer_gl_mesh l_renderer_gl_mesh_create(
		GLuint numIndices,
		GLuint numVertices,
		GLuint* indexData, 
		GLfloat* vertexData
		);
l_renderer_gl_mesh l_renderer_gl_mesh_createCube();
void l_renderer_gl_mesh_render(l_renderer_gl_mesh* m);

/*SHADER**********************************************************************/

GLuint l_renderer_gl_shader_create(const char* vertexShaderSourcePath, const char* fragmentShaderSourcePath);
void l_renderer_gl_shader_setUniform3f(
		GLuint shader,
		const char* uniformName,
		float a, float b, float c);
void l_renderer_gl_shader_setMat4Uniform(
		GLuint shader, 
		const char* uniformName, 
		blib_mat4_t* m
		);

void l_renderer_gl_shader_useCamera(GLuint shader, l_renderer_gl_camera* cam);

/*TEXTURE*********************************************************************/

//TODO make a destroy texture func to clean this up
GLuint l_renderer_gl_texture_create(const char* imageFile);

/*TRANSFORM*******************************************************************/

struct l_renderer_gl_transform{
	blib_vec3f_t position;
	blib_vec3f_t eulerAngles;
	blib_vec3f_t scale;

} ;

l_renderer_gl_transform l_renderer_gl_transform_create();
blib_mat4_t l_renderer_gl_transform_GetMatrix(l_renderer_gl_transform* t);
blib_vec3f_t l_renderer_gl_transform_getLocalForward(
		l_renderer_gl_transform* t);
blib_vec3f_t l_renderer_gl_transform_getLocalUp(l_renderer_gl_transform* t);
blib_vec3f_t l_renderer_gl_transform_getLocalRight(l_renderer_gl_transform* t);
void l_renderer_gl_transform_rotate(
		l_renderer_gl_transform* t, blib_vec3f_t rotation);

/*CAMERA**********************************************************************/

struct l_renderer_gl_camera {
	l_renderer_gl_transform transform;
	blib_mat4_t projectionMatrix;
	blib_mat4_t viewMatrix;
	float fov;
};

l_renderer_gl_camera l_renderer_gl_camera_create(float fov);
blib_mat4_t l_renderer_gl_camera_GetViewMatrix(l_renderer_gl_transform* t);
void l_renderer_gl_camera_setProjectionMatrix(
		l_renderer_gl_camera* cam, float aspect);
void l_renderer_gl_camera_update(
		l_renderer_gl_camera* cam,l_renderer_gl* d);

/*RUNTIME*********************************************************************/

struct l_renderer_gl{
	GLFWwindow* window;
	int windowWidth;
	int windowHeight;
	float aspectRatio;

	double frameStartTime;
	double frameEndTime;
	double deltaTime;

	l_renderer_gl_camera activeCamera;
	GLuint activeShader;
};

l_renderer_gl l_renderer_gl_init(int windowWidth, int windowHeight);
void l_renderer_gl_update(l_renderer_gl* r);
void l_renderer_gl_cleanup(l_renderer_gl* d);

/*MESH************************************************************************/

#endif /*L_RENDERER_H*/
