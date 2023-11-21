#ifndef LITE_GL_H
#define LITE_GL_H

#include "glad/glad.h"
#include "stb_image.h"
#include "blib_file.h"
#include "blib_math.h"
#include "lite.h"

//SHADER

GLuint lite_gl_shader_create();

//TEXTURE

GLuint lite_gl_texture_create(const char* imageFile);

//MESH

typedef struct lite_gl_mesh_t lite_gl_mesh_t;
struct lite_gl_mesh_t {
	GLfloat* vertexData;
	GLuint numVertices;
	GLuint* indexData;
	GLuint numIndices;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
};

lite_gl_mesh_t lite_gl_mesh_createFromRawData(
		GLuint numIndices, GLuint numVertices, 
		GLuint* indexData, GLfloat* vertexData);
lite_gl_mesh_t lite_gl_mesh_createFromGLTF(const char* fileName);
void lite_gl_mesh_render(lite_gl_mesh_t* pMesh);

//TRANSFORM

typedef struct lite_gl_transform_t lite_gl_transform_t;

struct lite_gl_transform_t {
	blib_vec3f_t position;
	blib_vec3f_t eulerAngles;
	blib_vec3f_t scale;

};

lite_gl_transform_t lite_gl_transform_create();

//TODO this might be an inefficient way to get directions. consider using the 
//cross product of forward and up
blib_vec3f_t lite_transform_getLocalForward(lite_gl_transform_t* t);
blib_vec3f_t lite_transform_getLocalUp(lite_gl_transform_t* t);
blib_vec3f_t lite_transform_getLocalRight(lite_gl_transform_t* t);
blib_mat4_t lite_gl_transform_GetMatrix(lite_gl_transform_t* t);
void lite_gl_transform_rotate(lite_gl_transform_t* t, blib_vec3f_t rotation);

//CAMERA

typedef struct lite_gl_camera_t lite_gl_camera_t;
struct lite_gl_camera_t {
	lite_gl_transform_t transform;
	blib_vec3f_t U;
	blib_vec3f_t V;
	blib_vec3f_t N;
	blib_mat4_t projectionMatrix;
	blib_mat4_t viewMatrix;
	float fov;
};

blib_mat4_t lite_gl_camera_GetViewMatrix(lite_gl_transform_t* t);
void lite_gl_camera_setProjectionMatrix(lite_gl_camera_t* cam, float aspect);
lite_gl_camera_t lite_gl_camera_create(float fov);
void lite_gl_camera_update(lite_gl_camera_t* cam, lite_engine_instance_t* instance);

// PRIMITIVE CUBE //===========================================================

typedef struct lite_gl_cube_t lite_gl_cube_t;
struct lite_gl_cube_t {
	lite_gl_transform_t transform;
	lite_gl_mesh_t mesh;
	GLuint shader;
	GLuint texture;
	bool active;
};

lite_gl_cube_t lite_gl_cube_create();

void lite_gl_cube_update(
		lite_gl_cube_t* go, lite_engine_instance_t* instance);

/*TODO GET RID OF THESE!!*/
extern lite_gl_cube_t TESTcube;
extern lite_gl_camera_t TESTcamera;
//ENGINE INSTANCE

void lite_gl_initialize(lite_engine_instance_t* instance);

#endif  /*LITE_GL_H*/
