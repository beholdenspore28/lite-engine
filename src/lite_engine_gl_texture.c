#include "lite_engine_gl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint lite_engine_gl_texture_create(const char *imageFile) {
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
		debug_error("Failed to load texture from '%s'", imageFile);
	}

	/*cleanup*/
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}
