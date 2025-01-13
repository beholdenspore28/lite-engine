#include "lite_engine.h"
#include "platform_x11.h"

#define LGL_LOG_FUNC(...)	debug_log(__VA_ARGS__);
#define LGL_WARN_FUNC(...)	debug_log(__VA_ARGS__);
#define LGL_ERROR_FUNC(...)	debug_log(__VA_ARGS__);
#include "lgl.h"

int main() {
	lite_engine_context_t *engine = lite_engine_start();

	// create a cube
	lgl_render_data_t cube	= lgl_cube_alloc	();
	GLuint vertex_shader	= lgl_shader_compile	("res/shaders/unlit_vertex.glsl",   GL_VERTEX_SHADER);
	GLuint fragment_shader	= lgl_shader_compile	("res/shaders/unlit_fragment.glsl", GL_FRAGMENT_SHADER);
	cube.shader		= lgl_shader_link	(vertex_shader, fragment_shader);
	cube.diffuseMap		= lgl_texture_alloc	("res/textures/lite-engine-icon.png");
	cube.position_z		= 2;

	double time = 0;
	while (engine->is_running) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cube.position_x = sinf(time);
		lgl_draw(1, &cube);
		lite_engine_end_frame(engine);
		time += 0.05;
	}

	lite_engine_stop(engine);

	return 0;
}
