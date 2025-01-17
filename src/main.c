#include "lite_engine.h"
#include "platform_x11.h"
#include "lgl.h"

int main() {
	lite_engine_context_t *engine = lite_engine_start();

	lgl_render_data_t objects[1024] = {0};

	enum {
		FLOOR,
		CUBE,
	};

	objects[FLOOR] = lgl_cube_alloc(); {
		GLuint vertex_shader = lgl_shader_compile(
				"res/shaders/phong_diffuse_vertex.glsl",
				GL_VERTEX_SHADER);

		GLuint fragment_shader = lgl_shader_compile(
				"res/shaders/phong_diffuse_fragment.glsl",
				GL_FRAGMENT_SHADER);

		objects[FLOOR].shader		= lgl_shader_link(vertex_shader, fragment_shader);
		objects[FLOOR].diffuse_map	= lgl_texture_alloc("res/textures/test.png");
		objects[FLOOR].specular_map	= lgl_texture_alloc("res/textures/test.png");
		objects[FLOOR].position.y	= -1;
		objects[FLOOR].scale		= (lgl_3f_t) {100, 1, 100};
		objects[FLOOR].texture_scale	= lgl_2f_one(10.0);
	}

	objects[CUBE] = lgl_cube_alloc(); {
		GLuint vertex_shader = lgl_shader_compile(
				"res/shaders/phong_diffuse_vertex.glsl",
				GL_VERTEX_SHADER);

		GLuint fragment_shader = lgl_shader_compile(
				"res/shaders/phong_diffuse_fragment.glsl",
				GL_FRAGMENT_SHADER);

		objects[CUBE]	.shader		= lgl_shader_link(vertex_shader, fragment_shader);
		objects[CUBE]	.diffuse_map	= lgl_texture_alloc("res/textures/lite-engine-icon.png");
		objects[CUBE]	.position.z	= 2;
	}

	while (engine->is_running) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		objects[FLOOR]	.texture_offset.x	+= engine->time_delta;
		objects[FLOOR]	.texture_offset.x	= loop(objects[FLOOR].texture_offset.x, 1);
		objects[CUBE]	.position.x		= sin(engine->time_current);
		objects[CUBE]	.position.y		= cos(engine->time_current)+1;

		lgl_draw		(2, objects);
		lite_engine_end_frame	(engine);
	}

	lite_engine_free(engine);

	return 0;
}
