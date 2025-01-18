#include "lgl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "blib/blib_file.h"

static float lgl__viewport_width = 640;
static float lgl__viewport_height = 480;

static const float
	LGL__LEFT	= -0.5,
	LGL__RIGHT	=  0.5,
	LGL__UP		=  0.5,
	LGL__DOWN	= -0.5,
	LGL__FORWARD	=  0.5,
	LGL__BACK	= -0.5;


void lgl_viewport_set(const float width, const float height) {
	lgl__viewport_width	= width;
	lgl__viewport_height	= height;
	glViewport(0, 0, width, height);
}

/*Multiplies a 4x4 matrix with another 4x4 matrix*/
static inline void lgl__mat4_multiply(
		float *result,
		const float *a,
		const float *b) {

	// row 0
	result[ 0] = a[ 0] * b[ 0] + a[ 4] * b[ 1] + a[ 8] * b[ 2] + a[12] * b[ 3];
	result[ 1] = a[ 0] * b[ 1] + a[ 1] * b[ 5] + a[ 2] * b[ 9] + a[ 3] * b[13];
	result[ 2] = a[ 0] * b[ 2] + a[ 1] * b[ 6] + a[ 2] * b[10] + a[ 3] * b[14];
	result[ 3] = a[ 0] * b[ 3] + a[ 1] * b[ 7] + a[ 2] * b[11] + a[ 3] * b[15];

	// row 1
	result[ 4] = a[ 4] * b[ 0] + a[ 5] * b[ 4] + a[ 6] * b[ 8] + a[ 7] * b[12];
	result[ 5] = a[ 4] * b[ 1] + a[ 5] * b[ 5] + a[ 6] * b[ 9] + a[ 7] * b[13];
	result[ 6] = a[ 4] * b[ 2] + a[ 5] * b[ 6] + a[ 6] * b[10] + a[ 7] * b[14];
	result[ 7] = a[ 4] * b[ 3] + a[ 5] * b[ 7] + a[ 6] * b[11] + a[ 7] * b[15];

	// row 2
	result[ 8] = a[ 8] * b[ 0] + a[ 9] * b[ 4] + a[10] * b[ 8] + a[11] * b[12];
	result[ 9] = a[ 8] * b[ 1] + a[ 9] * b[ 5] + a[10] * b[ 9] + a[11] * b[13];
	result[10] = a[ 8] * b[ 2] + a[ 9] * b[ 6] + a[10] * b[10] + a[11] * b[14];
	result[11] = a[ 8] * b[ 3] + a[ 9] * b[ 7] + a[10] * b[11] + a[11] * b[15];

	// row 3
	result[12] = a[12] * b[ 0] + a[13] * b[ 4] + a[14] * b[ 8] + a[15] * b[12];
	result[13] = a[12] * b[ 1] + a[13] * b[ 5] + a[14] * b[ 9] + a[15] * b[13];
	result[14] = a[12] * b[ 2] + a[13] * b[ 6] + a[14] * b[10] + a[15] * b[14];
	result[15] = a[12] * b[ 3] + a[13] * b[ 7] + a[14] * b[11] + a[15] * b[15];
}   

void lgl_perspective(
		float *mat,
		const float fov,
		const float aspect,
		const float near,
		const float far) {

	const float cotan = (1.0 / tanf(fov * 0.5));
	
	mat[0]	= (cotan / aspect);
	mat[5]	= cotan;
	mat[10]	= -((near + far) / (near - far));
	mat[11]	= 1.0f;
	mat[14]	= ((2.0 * near * far) / (near - far));
}

GLuint lgl_texture_alloc(const char *imageFile) {
	debug_log("Loading texture from '%s'", imageFile);

	/*create texture*/
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	/*set parameters*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	/*load texture data from file*/
	int width,
	    height,
	    numChannels;

	stbi_set_flip_vertically_on_load(1);
	unsigned char *data = stbi_load(imageFile, &width, &height, &numChannels, 0);

	/*error check*/
	if (data) {
		if (numChannels == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
					0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		} else if (numChannels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
					GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	} else {
		debug_error("Failed to load texture from '%s'\n", imageFile);
	}

	/*cleanup*/
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

GLuint lgl_shader_compile(const char *file_path, GLenum type) {
	file_buffer fb = file_buffer_alloc(file_path);
	if (fb.error) { // error check
		debug_error("failed to read shader from '%s'\n", file_path);
	}

	const char* shader_source = fb.text;

	GLuint shader = glCreateShader(type);
	glShaderSource	(shader, 1, &shader_source, NULL);
	glCompileShader	(shader);

	file_buffer_free(fb);

	{ // error check
		int	success;
		char	infoLog[512];

		glGetShaderiv	(shader, GL_COMPILE_STATUS, &success);

		if(!success) {
			glGetShaderInfoLog	(shader, 512, NULL, infoLog);
			debug_error		("Error shader compilation failed:\n%s", infoLog);
		}
	}

	return shader;
}

GLuint lgl_shader_link (GLuint vertex_shader, GLuint fragment_shader) {
	GLuint shader = glCreateProgram();
	glAttachShader	(shader, vertex_shader);
	glAttachShader	(shader, fragment_shader);
	glLinkProgram	(shader);
	glDetachShader	(shader, vertex_shader);
	glDetachShader	(shader, fragment_shader);
	glDeleteShader	(vertex_shader);
	glDeleteShader	(fragment_shader);
	return shader;
}

void lgl__buffer_vertex_array (lgl_render_data_t *data) {
	glGenVertexArrays(1, &data->VAO);
	glBindVertexArray(data->VAO);

	glGenBuffers(1, &data->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, data->VBO);

	glBufferData(	GL_ARRAY_BUFFER, 
			data->vertex_count*sizeof(lgl_vertex_t), 
			data->vertices, 
			GL_STATIC_DRAW);

	glVertexAttribPointer(
			0, 3, GL_FLOAT, GL_FALSE, sizeof(lgl_vertex_t),
			(void*)offsetof(lgl_vertex_t, position));

	glVertexAttribPointer(
			1, 3, GL_FLOAT, GL_FALSE, sizeof(lgl_vertex_t),
			(void*)offsetof(lgl_vertex_t, normal));

	glVertexAttribPointer(
			2, 2, GL_FLOAT, GL_FALSE, sizeof(lgl_vertex_t),
			(void*)offsetof(lgl_vertex_t, texture_coordinates));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

void lgl_draw(size_t data_length, lgl_render_data_t *data) {
	for(size_t i = 0; i < data_length; i++) {
		glUseProgram(data[i].shader);

		GLfloat projection[16] = {
			1.0,	0.0,	0.0,	0.0,
			0.0,	1.0,	0.0,	0.0,
			0.0,	0.0,	1.0,	0.0,
			0.0,	0.0,	0.0,	1.0,
		};

		const float aspect = lgl__viewport_width / lgl__viewport_height;
		lgl_perspective(projection, 80 * (3.14159/180.0), aspect, 0.001, 1000);

		GLfloat model[16] = {
			data[i].scale.x,    0.0,                0.0,                0.0,
			0.0,                data[i].scale.y,    0.0,                0.0,
			0.0,                0.0,                data[i].scale.z,    0.0,
			data[i].position.x, data[i].position.y, data[i].position.z, 1.0,
		};

		GLfloat mvp[16] = {
			1.0,	0.0,	0.0,	0.0,
			0.0,	1.0,	0.0,	0.0,
			0.0,	0.0,	1.0,	0.0,
			0.0,	0.0,	0.0,	1.0,
		};

		lgl__mat4_multiply(mvp, model, projection);

		GLint mvp_location = glGetUniformLocation(data[i].shader, "u_mvp");
		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, mvp);

		// textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, data[i].diffuse_map);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, data[i].specular_map);

		glUniform2f(glGetUniformLocation(data[i].shader, "u_texture_offset"),
				data[i].texture_offset.x,
				data[i].texture_offset.y);

		glUniform2f(glGetUniformLocation(data[i].shader, "u_texture_scale"),
				data[i].texture_scale.x,
				data[i].texture_scale.y);

		// other material properties
		glUniform1i(glGetUniformLocation(data[i].shader, "u_material.diffuse"), 0);
		glUniform1i(glGetUniformLocation(data[i].shader, "u_material.specular"), 1);
		glUniform1f(glGetUniformLocation(data[i].shader, "u_material.shininess"), 8.0f);

		glUniform3f(
			glGetUniformLocation(data[i].shader, "u_ambientLight"), 
			0.4,
			0.4,
			0.4);

		debug_log("%d light count:", data[i].lights_count);

		// lighting uniforms
		for(GLuint light = 0; light < 1; light++) {
			glUniform1ui(glGetUniformLocation(data[i].shader, "u_lights_count"),
					data[i].lights_count);

			glUniform1i(glGetUniformLocation(data[i].shader, "u_lights[0].type"),
					data[i].lights[light].type);

			glUniform3f(glGetUniformLocation(data[i].shader, "u_lights[0].position"),
					data[i].lights[light].position.y,
					data[i].lights[light].position.x,
					data[i].lights[light].position.z);

			glUniform3f(glGetUniformLocation(data[i].shader, "u_lights[0].direction"),
					data[i].lights[light].direction.x,
					data[i].lights[light].direction.y,
					data[i].lights[light].direction.z);

			glUniform1f(glGetUniformLocation(data[i].shader, "u_lights[0].cut_off"),
					data[i].lights[light].cut_off);

			glUniform1f(glGetUniformLocation(data[i].shader, "u_lights[0].outer_cut_off"),
					data[i].lights[light].outer_cut_off);

			glUniform1f(glGetUniformLocation(data[i].shader, "u_lights[0].constant"),
					data[i].lights[light].constant);

			glUniform1f(glGetUniformLocation(data[i].shader, "u_lights[0].linear"),
					data[i].lights[light].linear);

			glUniform1f(glGetUniformLocation(data[i].shader, "u_lights[0].quadratic"),
					data[i].lights[light].quadratic);

			glUniform3f(glGetUniformLocation(data[i].shader, "u_lights[0].diffuse"),
					data[i].lights[light].diffuse.x,
					data[i].lights[light].diffuse.y,
					data[i].lights[light].diffuse.z);

			glUniform3f(glGetUniformLocation(data[i].shader, "u_lights[0].specular"),
					data[i].lights[light].specular.x,
					data[i].lights[light].specular.y,
					data[i].lights[light].specular.z);
		}

		glBindVertexArray(data[i].VAO);
		glDrawArrays(GL_TRIANGLES, 0, data[i].vertex_count);

		glUseProgram(0);
	}
}

lgl_render_data_t lgl_quad_alloc(void) {
	lgl_render_data_t quad = {0};

	enum { quad_vertices_count = 6 };
	lgl_vertex_t quad_vertices[quad_vertices_count] = { 
		//position                        //normal          //tex coord
		{ { LGL__LEFT,  LGL__DOWN, 0.0 }, lgl_3f_forward(1.0), { 0.0, 0.0 } },
		{ { LGL__RIGHT, LGL__DOWN, 0.0 }, lgl_3f_forward(1.0), { 1.0, 0.0 } },
		{ { LGL__RIGHT, LGL__UP,   0.0 }, lgl_3f_forward(1.0), { 1.0, 1.0 } },

		{ { LGL__LEFT,  LGL__UP,   0.0 }, lgl_3f_forward(1.0), { 0.0, 1.0 } },
		{ { LGL__LEFT,  LGL__DOWN, 0.0 }, lgl_3f_forward(1.0), { 0.0, 0.0 } },
		{ { LGL__RIGHT, LGL__UP,   0.0 }, lgl_3f_forward(1.0), { 1.0, 1.0 } },
	};

	quad.vertices		= quad_vertices;
	quad.vertex_count	= quad_vertices_count;
	quad.scale		= lgl_3f_one(1.0);
	quad.position		= lgl_3f_zero();
	quad.rotation		= lgl_4f_zero();
	quad.texture_offset	= lgl_2f_zero();
	quad.texture_scale	= lgl_2f_one(1.0);

	lgl__buffer_vertex_array(&quad);
	return quad;
}

lgl_render_data_t lgl_cube_alloc(void) {
	lgl_render_data_t cube = {0};

	enum { cube_vertices_count = 36 };
	lgl_vertex_t cube_vertices[cube_vertices_count] = { 
		//position                                 //normal             //tex coord
		{ { LGL__LEFT,  LGL__DOWN, LGL__BACK    }, lgl_3f_back(1.0),    { 0.0, 0.0 } },
		{ { LGL__RIGHT, LGL__DOWN, LGL__BACK    }, lgl_3f_back(1.0),    { 1.0, 0.0 } },
       		{ { LGL__RIGHT, LGL__UP,   LGL__BACK    }, lgl_3f_back(1.0),    { 1.0, 1.0 } },

       		{ { LGL__RIGHT, LGL__UP,   LGL__BACK    }, lgl_3f_back(1.0),    { 1.0, 1.0 } },
       		{ { LGL__LEFT,  LGL__UP,   LGL__BACK    }, lgl_3f_back(1.0),    { 0.0, 1.0 } },
       		{ { LGL__LEFT,  LGL__DOWN, LGL__BACK    }, lgl_3f_back(1.0),    { 0.0, 0.0 } },

       		{ { LGL__RIGHT, LGL__UP,   LGL__FORWARD }, lgl_3f_forward(1.0), { 1.0, 1.0 } },
       		{ { LGL__RIGHT, LGL__DOWN, LGL__FORWARD }, lgl_3f_forward(1.0), { 1.0, 0.0 } },
       		{ { LGL__LEFT,  LGL__DOWN, LGL__FORWARD }, lgl_3f_forward(1.0), { 0.0, 0.0 } },

       		{ { LGL__LEFT,  LGL__DOWN, LGL__FORWARD }, lgl_3f_forward(1.0), { 0.0, 0.0 } },
       		{ { LGL__LEFT,  LGL__UP,   LGL__FORWARD }, lgl_3f_forward(1.0), { 0.0, 1.0 } },
       		{ { LGL__RIGHT, LGL__UP,   LGL__FORWARD }, lgl_3f_forward(1.0), { 1.0, 1.0 } },

       		{ { LGL__LEFT,  LGL__DOWN, LGL__BACK    }, lgl_3f_left(1.0),    { 0.0, 1.0 } },
       		{ { LGL__LEFT,  LGL__UP,   LGL__BACK    }, lgl_3f_left(1.0),    { 1.0, 1.0 } },
       		{ { LGL__LEFT,  LGL__UP,   LGL__FORWARD }, lgl_3f_left(1.0),    { 1.0, 0.0 } },

       		{ { LGL__LEFT,  LGL__UP,   LGL__FORWARD }, lgl_3f_left(1.0),    { 1.0, 0.0 } },
       		{ { LGL__LEFT,  LGL__DOWN, LGL__FORWARD }, lgl_3f_left(1.0),    { 0.0, 0.0 } },
       		{ { LGL__LEFT,  LGL__DOWN, LGL__BACK    }, lgl_3f_left(1.0),    { 0.0, 1.0 } },

       		{ { LGL__RIGHT, LGL__UP,   LGL__FORWARD }, lgl_3f_right(1.0),   { 1.0, 0.0 } },
       		{ { LGL__RIGHT, LGL__UP,   LGL__BACK    }, lgl_3f_right(1.0),   { 1.0, 1.0 } },
       		{ { LGL__RIGHT, LGL__DOWN, LGL__BACK    }, lgl_3f_right(1.0),   { 0.0, 1.0 } },

       		{ { LGL__RIGHT, LGL__DOWN, LGL__BACK    }, lgl_3f_right(1.0),   { 0.0, 1.0 } },
       		{ { LGL__RIGHT, LGL__DOWN, LGL__FORWARD }, lgl_3f_right(1.0),   { 0.0, 0.0 } },
       		{ { LGL__RIGHT, LGL__UP,   LGL__FORWARD }, lgl_3f_right(1.0),   { 1.0, 0.0 } },

       		{ { LGL__RIGHT, LGL__DOWN, LGL__FORWARD }, lgl_3f_down(1.0),    { 1.0, 0.0 } },
       		{ { LGL__RIGHT, LGL__DOWN, LGL__BACK    }, lgl_3f_down(1.0),    { 1.0, 1.0 } },
       		{ { LGL__LEFT,  LGL__DOWN, LGL__BACK    }, lgl_3f_down(1.0),    { 0.0, 1.0 } },

       		{ { LGL__LEFT,  LGL__DOWN, LGL__BACK    }, lgl_3f_down(1.0),    { 0.0, 1.0 } },
       		{ { LGL__LEFT,  LGL__DOWN, LGL__FORWARD }, lgl_3f_down(1.0),    { 0.0, 0.0 } },
       		{ { LGL__RIGHT, LGL__DOWN, LGL__FORWARD }, lgl_3f_down(1.0),    { 1.0, 0.0 } },

       		{ { LGL__LEFT,  LGL__UP,   LGL__BACK    }, lgl_3f_up(1.0),      { 0.0, 1.0 } },
       		{ { LGL__RIGHT, LGL__UP,   LGL__BACK    }, lgl_3f_up(1.0),      { 1.0, 1.0 } },
       		{ { LGL__RIGHT, LGL__UP,   LGL__FORWARD }, lgl_3f_up(1.0),      { 1.0, 0.0 } },

       		{ { LGL__RIGHT, LGL__UP,   LGL__FORWARD }, lgl_3f_up(1.0),      { 1.0, 0.0 } },
       		{ { LGL__LEFT,  LGL__UP,   LGL__FORWARD }, lgl_3f_up(1.0),      { 0.0, 0.0 } },
       		{ { LGL__LEFT,  LGL__UP,   LGL__BACK    }, lgl_3f_up(1.0),      { 0.0, 1.0 } },
	};

	cube.vertices		= cube_vertices;
	cube.vertex_count	= cube_vertices_count;
	cube.scale		= lgl_3f_one(1.0);
	cube.position		= lgl_3f_zero();
	cube.rotation		= lgl_4f_zero();
	cube.texture_offset	= lgl_2f_zero();
	cube.texture_scale	= lgl_2f_one(1.0);

	lgl__buffer_vertex_array(&cube);
	return cube;
}
