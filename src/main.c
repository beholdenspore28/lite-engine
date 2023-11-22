#include <stdio.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "l_glfw.h"
#include "l_mesh.h"

#define _LITE_PRIMITIVE_CUBE_VERTEX_DATA_LENGTH (GLuint)64
#define _LITE_PRIMITIVE_CUBE_INDEX_DATA_LENGTH (GLuint)36

GLfloat _LITE_PRIMITIVE_CUBE_VERTEX_DATA[_LITE_PRIMITIVE_CUBE_VERTEX_DATA_LENGTH] = {
	/*front*/
	/*position        //color           //texcoord*/
	-0.5f,-0.5f,-0.5f, 0.3f, 0.3f, 0.3f, 1.0f, 0.0f,/*bottom left*/
	0.5f,-0.5f,-0.5f, 0.3f, 0.3f, 0.3f, 0.0f, 0.0f,/*bottom right*/
	-0.5f, 0.5f,-0.5f, 0.3f, 0.3f, 0.3f, 1.0f, 1.0f,/*top left*/
	0.5f, 0.5f,-0.5f, 0.3f, 0.3f, 0.3f, 0.0f, 1.0f,/*top right*/

	/*back*/
	-0.5f,-0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f,/*bottom left*/
	0.5f,-0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f,/*bottom right*/
	-0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f, 1.0f,/*top left*/
	0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f,/*top right*/
};

GLuint _LITE_PRIMITIVE_CUBE_INDEX_DATA[_LITE_PRIMITIVE_CUBE_INDEX_DATA_LENGTH] = {
	/*front*/
	2,0,1, 3,2,1,
	/*right*/
	1,5,7, 7,3,1,
	/*back*/
	5,4,6, 5,6,7,
	/*left*/
	0,2,6, 0,6,4,
	/*top*/
	3,7,6, 2,3,6,
	/*bottom*/
	5,1,0, 0,4,5,
};

int main (int argc, char* argv[]) {
	printf("Rev up those fryers!\n");
	l_glfw_data glfwData = l_glfw_init();	

	l_mesh m = l_mesh_create( 
			_LITE_PRIMITIVE_CUBE_VERTEX_DATA, 
			_LITE_PRIMITIVE_CUBE_VERTEX_DATA_LENGTH,
			_LITE_PRIMITIVE_CUBE_INDEX_DATA,
			_LITE_PRIMITIVE_CUBE_INDEX_DATA_LENGTH);

	while (!glfwWindowShouldClose(glfwData.window)){
		l_mesh_draw(&m);
		l_glfw_update(&glfwData);
	}

	l_glfw_data_destroy(&glfwData);
	return 0;
}
