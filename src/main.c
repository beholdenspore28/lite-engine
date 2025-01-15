#include "lite_engine.h"
#include "platform_x11.h"
#include "lgl.h"

int main() {
	lite_engine_context_t *engine = lite_engine_start();

	lgl_render_data_t cube	= lgl_cube_alloc	();
	GLuint vertex_shader	= lgl_shader_compile	("res/shaders/phong_diffuse_vertex.glsl",   GL_VERTEX_SHADER);
	GLuint fragment_shader	= lgl_shader_compile	("res/shaders/phong_diffuse_fragment.glsl", GL_FRAGMENT_SHADER);
	cube.shader		= lgl_shader_link	(vertex_shader, fragment_shader);
	cube.diffuseMap		= lgl_texture_alloc	("res/textures/lite-engine-icon.png");
	cube.position.z		= 2;

	while (engine->is_running) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cube.position.x = sin(engine->time_current);
		lgl_draw(1, &cube);
		lite_engine_end_frame(engine);
	}

	lite_engine_free(engine);

	return 0;
}
