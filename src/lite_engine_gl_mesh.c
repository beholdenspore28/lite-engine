#include "lite_engine_gl.h"

#include <ctype.h>

void lite_engine_gl_mesh_update(lite_engine_gl_state_t state) {
	glEnable(GL_CULL_FACE);

	for (ui64 e = 0; e < LITE_ENGINE_ENTITIES_MAX; e++) {
		if (state.meshes[e].enabled == 0) {
			continue;
		}

		if (state.meshes[e].use_wire_frame) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		{ // draw
			glUseProgram(state.materials[e].shader);

			// TODO calculate MVP on the CPU instead of in the vertex shader

			// model matrix uniform
			lite_engine_gl_transform_calculate_matrix(
					&state.transforms[e]);

			lite_engine_gl_shader_setUniformM4(
					state.materials[e].shader, "u_modelMatrix",
					&state.transforms[e].matrix);

			// view matrix uniform
			lite_engine_gl_shader_setUniformM4(
					state.materials[e].shader, "u_viewMatrix",
					&state
					.transforms[lite_engine_gl_get_active_camera()]
					.matrix);

			// projection matrix uniform
			lite_engine_gl_shader_setUniformM4(
					state.materials[e].shader, "u_projectionMatrix",
					&state.cameras[lite_engine_gl_get_active_camera()]
					.projection);

			// camera position uniform
			lite_engine_gl_shader_setUniformV3(
					state.materials[e].shader, "u_cameraPos",
					state.transforms[lite_engine_gl_get_active_camera()]
					.position);

#if 0
			const int light = 0;
			// light uniforms
			lite_engine_gl_shader_setUniformV3(
					state.materials[e].shader, "u_light.position",
					state.transforms[light].position);

			lite_engine_gl_shader_setUniformFloat(
					state.materials[e].shader, "u_light.constant",
					state.lights[light].constant);

			lite_engine_gl_shader_setUniformFloat(
					state.materials[e].shader, "u_light.linear",
					state.lights[light].linear);

			lite_engine_gl_shader_setUniformFloat(
					state.materials[e].shader, "u_light.quadratic",
					state.lights[light].quadratic);

			lite_engine_gl_shader_setUniformV3(
					state.materials[e].shader, "u_light.diffuse",
					state.lights[light].diffuse);

			lite_engine_gl_shader_setUniformV3(
					state.materials[e].shader, "u_light.specular",
					state.lights[light].specular);
#endif

			// textures
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,
					state.materials[e].diffuseMap);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D,
					state.materials[e].specularMap);

			// other material properties
			lite_engine_gl_shader_setUniformInt(
					state.materials[e].shader, "u_material.diffuse", 0);
			lite_engine_gl_shader_setUniformInt(
					state.materials[e].shader, "u_material.specular",
					1);
			lite_engine_gl_shader_setUniformFloat(
					state.materials[e].shader, "u_material.shininess",
					32.0f);
			lite_engine_gl_shader_setUniformV3(
					state.materials[e].shader, "u_ambientLight",
					vector3_one(0.4));

			// draw
			glBindVertexArray(state.meshes[e].VAO);
			glDrawElements(GL_TRIANGLES,
					state.meshes[e].indices.length,
					GL_UNSIGNED_INT, 0);
		}
	}

	glUseProgram(0);
}

mesh_t lite_engine_gl_mesh_alloc(list_vertex_t vertices, list_ui32 indices) {
	mesh_t m	= {0};
	m.enabled	= 1;
	m.vertices	= vertices;
	m.indices	= indices;

	glGenVertexArrays(1, &m.VAO);
	glGenBuffers(1, &m.VBO);
	glGenBuffers(1, &m.EBO);

	glBindVertexArray(m.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * vertices.length,
			vertices.array, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ui32) * indices.length,
			indices.array, GL_STATIC_DRAW);

	ui32 vertStride = sizeof(vertex_t); // how many bytes per vertex?

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertStride,
			(void *)offsetof(vertex_t, position));
	glEnableVertexAttribArray(0);

	// texcoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertStride,
			(void *)offsetof(vertex_t, texCoord));
	glEnableVertexAttribArray(1);

	// normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertStride,
			(void *)offsetof(vertex_t, normal));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return m;
}

mesh_t lite_engine_gl_mesh_lmod_alloc(const char *file_path) {
	debug_log("Loading lmod file from '%s'", file_path);
	file_buffer fb = file_buffer_alloc(file_path);

	if (fb.error) {
		debug_error("Failed to open .lmod file at '%s' did you specity "
				"the correct path?",
				file_path);
		assert(0);
	}

	list_vector3_t	positions	= list_vector3_t_alloc();
	list_vector3_t	normals		= list_vector3_t_alloc();
	list_vector2_t	tex_coords	= list_vector2_t_alloc();
	list_ui32	indices		= list_ui32_alloc();

	enum {
		STATE_INITIAL = -1,
		STATE_POSITION,
		STATE_INDICES,
		STATE_NORMAL,
		STATE_TEX_COORD,
	};
	uint8_t state = STATE_INITIAL;

	for (char *c = fb.text; c < fb.text + fb.length; c++) {

		if (isspace(*c)) {
			continue;
		} else if (*c == '#') {
			while (*c != '\n' && *c != '\0') { c++; }
			continue;
		}

		char token[128];
		{
			int num_tokens = sscanf(c, "%s", token);
			if (num_tokens != 1) {
				debug_error("Failed to read token at '%s'",
						file_path);
				assert(0);
			}
		}

		// TODO The order in which a buffer is loaded from the lmod file
		// matters. need to change the importer so that it can handle
		// parsing the buffers in any order.

		if (strcmp(token, "vertex_indices:") == 0 || state == STATE_INDICES) {
			if (state != STATE_INDICES) {
				c += sizeof("vertex_indices:");
			}

			state = STATE_INDICES;

			ui32 index;
			int num_tokens = sscanf(c, "%u", &index);
			if (num_tokens != 1) {
				debug_error( "Failed to read vertex_indices at '%s'", file_path);
				assert(0);
			}

			// skip the rest of the number
			while (*c != ' ' && *c != '\0') {
				c++;
			}

			// debug_log("%u ", index);
			list_ui32_add(&indices, index);
		}

		if (strcmp(token, "vertex_texture_coordinates:") == 0 || state == STATE_TEX_COORD) {
			if (state != STATE_TEX_COORD) {
				c += sizeof("vertex_texture_coordinates:");
			}

			state = STATE_TEX_COORD;

			vector2_t tex_coord = {0};

			int num_tokens = sscanf(c, "%f %f", &tex_coord.x, &tex_coord.y);

			if (num_tokens != 2) {
				debug_error(
						"Failed to read vertex_texture_coordinates "
						"at '%s'",
						file_path);
				assert(0);
			}

#if 0
			debug_log("%f %f\n", tex_coord.x, tex_coord.y);
#endif

			list_vector2_t_add(&tex_coords, tex_coord);

			while (*c != '\n' && *c != '\0') {
				c++;
			}
		}

		if (strcmp(token, "vertex_normals:") == 0 || state == STATE_NORMAL) {
			if (state != STATE_NORMAL) {
				c += sizeof("vertex_normals:");
			}

			state = STATE_NORMAL;

			vector3_t normal     = {0};
			int	  num_tokens = sscanf(c, "%f %f %f", &normal.x,
					&normal.y, &normal.z);
			if (num_tokens != 3) {
				debug_error(
						"Failed to read vertex_normals at '%s'",
						file_path);
				assert(0);
			}

			// vector3_print(normal, "normal");
			list_vector3_t_add(&normals, normal);
			while (*c != '\n' && *c != '\0') {
				c++;
			}
		}

		if (strcmp(token, "vertex_positions:") == 0 || state == STATE_POSITION) {
			if (state != STATE_POSITION) {
				c += sizeof("vertex_positions:");
			}

			state = STATE_POSITION;

			vector3_t position = {0};
			int num_tokens	   = sscanf(c, "%f %f %f", &position.x,
					&position.y, &position.z);
			if (num_tokens != 3) {
				debug_error(
						"Failed to read vertex_positions at '%s'",
						file_path);
				assert(0);
			}

			// vector3_print(position, "position");
			list_vector3_t_add(&positions, position);
			while (*c != '\n' && *c != '\0') {
				c++;
			}
		}
	}

	file_buffer_free(fb);

	debug_log("pos: %zu norm: %zu tex: %zu", positions.length,
			normals.length, tex_coords.length);

	list_vertex_t vertices = list_vertex_t_alloc();
	for (size_t i = 0; i < positions.length; i++) {
		vertex_t vertex = {0};
		if (positions.length	> 0) {
			vertex.position	= positions.array[i];
		}
		if (normals.length	> 0) {
			vertex.normal	= normals.array[i];
		}
		if (tex_coords.length	> 0) {
			vertex.texCoord	= tex_coords.array[i];
		}
		list_vertex_t_add(&vertices, vertex);
	}

	list_vector3_t_free(&positions);
	list_vector3_t_free(&normals);
	list_vector2_t_free(&tex_coords);

	mesh_t mesh = lite_engine_gl_mesh_alloc(vertices, indices);
	return mesh;
}

void lite_engine_gl_mesh_free(mesh_t *mesh) {
	list_vertex_t_free(&mesh->vertices);
	list_ui32_free(&mesh->indices);
}
