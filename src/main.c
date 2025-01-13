#include "lite_engine.h"
#include "platform_x11.h"
#include "lgl.h"

int main() {
	lite_engine_context_t *engine = lite_engine_start();

	lgl_render_data_t data = lgl_cube_alloc();
	data.position_z = 2;

	GLuint vertex_shader = lgl_shader_compile( "res/shaders/unlit_vertex.glsl", GL_VERTEX_SHADER);
	GLuint fragment_shader = lgl_shader_compile( "res/shaders/unlit_fragment.glsl", GL_FRAGMENT_SHADER);
	data.shader = lgl_shader_link(vertex_shader, fragment_shader);

	data.diffuseMap = lgl_texture_alloc("res/textures/lite-engine-icon.png");

	double time = 0;
	while (engine->is_running) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		data.position_x = sinf(time);
		lgl_draw(1, &data);
		engine_end_frame(engine);
		time += 0.05;
	}

	engine_stop(engine);

	return 0;
}
