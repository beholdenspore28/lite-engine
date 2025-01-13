#include "lite_engine.h"
#include "platform_x11.h"
#include "lgl.h"

void viewport_size_callback(
		const unsigned int width,
		const unsigned int height) {
	lgl_viewport_set(width, height);
}

int main() {
	x_data_t *internal_x_data = x_start("Game Window", 640, 480);
	internal_x_data->viewport_size_callback = viewport_size_callback;

	glClearColor(0.2, 0.3, 0.4, 1.0);
	glEnable(GL_DEPTH_TEST);

	lgl_render_data_t data = lgl_cube_alloc();
	data.position_z = 2;

	GLuint vertex_shader = lgl_shader_compile( "res/shaders/unlit_vertex.glsl", GL_VERTEX_SHADER);
	GLuint fragment_shader = lgl_shader_compile( "res/shaders/unlit_fragment.glsl", GL_FRAGMENT_SHADER);
	data.shader = lgl_shader_link(vertex_shader, fragment_shader);

	data.diffuseMap = lgl_texture_alloc("res/textures/lite-engine-icon.png");

	double time = 0;
	while (internal_x_data->running) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		data.position_x = sinf(time);
		lgl_draw(1, &data);
		x_end_frame(internal_x_data);
		time += 0.05;
	}

	x_stop(internal_x_data);

	return 0;
}
