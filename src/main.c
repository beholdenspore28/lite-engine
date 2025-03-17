#include "lgl.h"
#define BLIB_IMPLEMENTATION
#include "blib/blib_math3d.h"

#if 1
lgl_render_data_t asteroid_mesh_alloc(void) {

	list_lgl_vertex_t vertices   = list_lgl_vertex_t_alloc();

	const float   radius     = 0.1;
	const float   resolution = 10.0;

	for(int face = 0; face < 6; face++) {
		//const int offset = resolution * resolution * face;
		for(float x = 0; x < resolution; x++) {
			for(float y = 0; y < resolution; y++) {

				vector3_t position = (vector3_t) { 
					.x = map(x, 0, resolution -1, -0.5, 0.5), 
					.y = map(y, 0, resolution -1, -0.5, 0.5), 
					.z = 0.5,
				};
				position = vector3_normalize(position);

				// 0 is front
				const vector3_t temp = position;
				switch (face) {
					case 0: { // 0 is front
						// do nothing.
						position.x = -temp.x;
					} break;
					case 1: { // 1 is rear
						position.z = -position.z;
					}break;
					case 2: { // 2 is left
						position.z = temp.x;
						position.x = temp.z;
					} break;	
					case 3: { // 3 is right
						position.z = -temp.x;
						position.x = -temp.z;
					} break;
					case 4: { // 4 is bottom
						position.y = -temp.z;
						position.z = -temp.y;
					} break;
					case 5: { // 5 is top
						position.y = temp.z;
						position.z = temp.y;
					} break;
					default: {
						fprintf(stderr, "rock generator encountered invalid face index");
					} break;
				}

				const float amplitude = 5.0;
				const float frequency = 1;
				const float noise = radius + (noise3_fbm(
							position.x * frequency,
							position.y * frequency,
							position.z * frequency) * amplitude);

				lgl_vertex_t v = (lgl_vertex_t) {
					.position = vector3_scale(position, noise),
					.normal = position,
					.texture_coordinates = vector2_zero(),
				};

        //vector3_print(position, "pos");

				list_lgl_vertex_t_add(&vertices, v);
			}
		}
	}

	lgl_render_data_t mesh;
  mesh.vertices = vertices.array;
  mesh.vertex_count = vertices.length;

	return mesh;
}
#endif

int main() {
  lgl_context_t *context = lgl_start(854, 480);

  // ---------------------------------------------------------------
  // Create shaders
  
  GLuint shader_phong = 0; {
    GLuint vertex_shader = lgl_shader_compile(
        "res/shaders/phong_vertex.glsl",
        GL_VERTEX_SHADER);

    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/phong_fragment.glsl",
        GL_FRAGMENT_SHADER);

    shader_phong = lgl_shader_link(vertex_shader, fragment_shader);
  }

  GLuint shader_framebuffer = 0; {
    GLuint vertex_shader = lgl_shader_compile(
        "res/shaders/frame_buffer_texture_vertex.glsl",
        GL_VERTEX_SHADER);

    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/frame_buffer_gaussian_fragment.glsl",
        GL_FRAGMENT_SHADER);

    shader_framebuffer = lgl_shader_link(vertex_shader, fragment_shader);
  }

  // ---------------------------------------------------------------
  // Create textures

  GLuint
    //texture_diffuse  = lgl_texture_alloc("res/textures/test.png"),
    //texture_specular = lgl_texture_alloc("res/textures/default_specular.png"),
    texture_cube     = lgl_texture_alloc("res/textures/lite-engine-cube.png");

  // ---------------------------------------------------------------
  // Create lights

  enum {
    LIGHTS_POINT_0,
    LIGHTS_POINT_1,
    LIGHTS_COUNT, // this should ALWAYS be at the end of the enum
  };
  lgl_light_t lights [LIGHTS_COUNT] = {0};

  lights[LIGHTS_POINT_0] = (lgl_light_t) {
    .type           = 0,
    .position       = {0.0, 1.0, -5.0},
    .direction      = {0.0, 0.0, 1.0},
    .cut_off        = cos(12.5),
    .outer_cut_off  = cos(15.0),
    .constant       = 1.0f,
    .linear         = 0.09f,
    .quadratic      = 0.032f,
    .diffuse        = (vector3_t){1.0, 0.0, 0.0},
    .specular       = vector3_one(0.6),
  };

  lights[LIGHTS_POINT_1] = (lgl_light_t) {
    .type           = 0,
    .position       = {0.0, 1.0, -5.0},
    .direction      = {0.0, 0.0, 1.0},
    .cut_off        = cos(12.5),
    .outer_cut_off  = cos(15.0),
    .constant       = 1.0f,
    .linear         = 0.09f,
    .quadratic      = 0.032f,
    .diffuse        = (vector3_t){0.0, 0.0, 1.0},
    .specular       = vector3_one(0.6),
  };

  // ---------------------------------------------------------------
  // Create objects

  enum {
    OBJECTS_CUBE,
    OBJECTS_ASTEROID,
    OBJECTS_COUNT  // this should ALWAYS be at the end of the enum,
  };
  lgl_render_data_t objects [OBJECTS_COUNT] = {0};

  //objects[OBJECTS_FLOOR] = lgl_cube_alloc(context); {
  //  objects[OBJECTS_FLOOR].shader        =  shader_phong;
  //  objects[OBJECTS_FLOOR].diffuse_map   =  texture_diffuse;
  //  objects[OBJECTS_FLOOR].specular_map  =  texture_specular;
  //  objects[OBJECTS_FLOOR].texture_scale =  vector2_one(10.0);
  //  objects[OBJECTS_FLOOR].position.y    = -2;
  //  objects[OBJECTS_FLOOR].scale         =  (vector3_t) {5, 0.5, 5};
  //  objects[OBJECTS_FLOOR].lights_count  =  LIGHTS_COUNT;
  //  objects[OBJECTS_FLOOR].lights        =  lights;
  //}

  objects[OBJECTS_CUBE] = lgl_cube_alloc(); {
    objects[OBJECTS_CUBE].shader         =  shader_phong;
    objects[OBJECTS_CUBE].diffuse_map    =  texture_cube;
    objects[OBJECTS_CUBE].position.z     =  1;
    objects[OBJECTS_CUBE].lights_count   =  LIGHTS_COUNT;
    objects[OBJECTS_CUBE].lights         =  lights;
    objects[OBJECTS_CUBE].render_flags  |=  LGL_FLAG_USE_STENCIL;
  }

  objects[OBJECTS_ASTEROID] = asteroid_mesh_alloc(); {
    objects[OBJECTS_ASTEROID].shader         =  shader_phong;
    objects[OBJECTS_ASTEROID].diffuse_map    =  texture_cube;
    objects[OBJECTS_ASTEROID].position.z     =  1;
    objects[OBJECTS_ASTEROID].lights_count   =  LIGHTS_COUNT;
    objects[OBJECTS_ASTEROID].lights         =  lights;
    objects[OBJECTS_ASTEROID].render_flags  |=  LGL_FLAG_USE_STENCIL;
  }

  // ---------------------------------------------------------------
  // Create framebuffer
  lgl_framebuffer_t frame;
  lgl_framebuffer_alloc(&frame, shader_framebuffer);
  lgl_active_framebuffer_set(&frame);

  // ---------------------------------------------------------------
  // game loop
  
  while(!glfwWindowShouldClose(context->GLFWwindow)) {
    { // update state
      //objects[OBJECTS_ASTEROID].position.y = cos(context->time_current)*0.2 + 0.5;
      objects[OBJECTS_ASTEROID].rotation = quaternion_multiply(
          objects[OBJECTS_ASTEROID].rotation,
          quaternion_from_euler((vector3_t) { 0, context->time_delta, 0 }));
      // TODO fix texture scrolling
      //objects[OBJECTS_FLOOR].texture_offset.y += context->time_delta;

      lights[LIGHTS_POINT_0].position.x = sin(context->time_current);
      lights[LIGHTS_POINT_0].position.z = cos(context->time_current);
      lights[LIGHTS_POINT_1].position.x = cos(context->time_current);
      lights[LIGHTS_POINT_1].position.z = sin(context->time_current);
    }

    { // draw scene to the frame
      glBindFramebuffer(GL_FRAMEBUFFER, frame.FBO);

      glClearColor(0.3,0.3,0.3,1);
      glClear(
          GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT |
          GL_STENCIL_BUFFER_BIT);

      lgl_draw(OBJECTS_COUNT, objects);

      objects[OBJECTS_CUBE].scale = vector3_one(0.01);
      //objects[OBJECTS_CUBE].render_flags |= LGL_FLAG_USE_WIREFRAME;

      for(size_t i = 0; i < objects[OBJECTS_ASTEROID].vertex_count; i++) {
        objects[OBJECTS_CUBE].position = 
          vector3_scale(objects[OBJECTS_ASTEROID].vertices[i].position, 0.1);
        lgl_draw(1, &objects[OBJECTS_CUBE]);
      }
    }

    { // draw the frame to the screen
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      glClear(
          GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT |
          GL_STENCIL_BUFFER_BIT);

      lgl_draw(1, &frame.quad);
    }

    lgl_end_frame(context);
  }

  lgl_free(context);

  return 0;
}
