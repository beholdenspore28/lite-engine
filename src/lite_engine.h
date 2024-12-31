#ifndef LITE_ENGINE_H
#define LITE_ENGINE_H

#include "lite_engine_debug.h"
#include "blib/blib.h"
#include "blib/blib_math3d.h"
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

enum {
	LITE_ENGINE_RENDERER_NONE,
	LITE_ENGINE_RENDERER_GL,
};

enum {
	LITE_ENGINE_ENTITIES_MAX = 10,
};

typedef uint8_t  ui8;
typedef uint16_t ui16;
typedef uint32_t ui32;
typedef uint64_t ui64;

DECLARE_LIST(ui8)
DECLARE_LIST(ui16)
DECLARE_LIST(ui32)
DECLARE_LIST(ui64)

typedef struct {
	float          constant;
	float          linear;
	float          quadratic;
	vector3_t      diffuse;
	vector3_t      specular;
} point_light_t;
DECLARE_LIST(point_light_t)

typedef struct {
	vector3_t      position;
	vector2_t      texCoord;
	vector3_t      normal;
} vertex_t;
DECLARE_LIST(vertex_t)

typedef struct {
	ui8            enabled;
	ui8            use_wire_frame;
	ui32           VAO;
	ui32           VBO;
	ui32           EBO;
	list_vertex_t  vertices;
	list_ui32      indices;
} mesh_t;
DECLARE_LIST(mesh_t)

typedef struct {
	ui32           shader;
	ui32           diffuseMap;
	ui32           specularMap;
} material_t;
DECLARE_LIST(material_t)

typedef struct {
  matrix4_t        matrix;
  vector3_t        position;
  quaternion_t     rotation;
  vector3_t        scale;
} transform_t;
DECLARE_LIST(transform_t)

typedef struct {
	matrix4_t      projection;
	float          lastX;
	float          lastY;
} camera_t;
DECLARE_LIST(camera_t)

typedef struct {
	material_t    *materials;
	mesh_t        *meshes;
	transform_t   *transforms;
	point_light_t *lights;
	camera_t      *cameras;
} object_pool_t;

void   lite_engine_start                 (void);
void   lite_engine_use_render_api        (ui8 api);
ui8    lite_engine_is_running            (void);
void   lite_engine_update                (void);
void   lite_engine_stop                  (void);
double lite_engine_get_time_delta        (void);

void   lite_engine_ECS_update            (void);
ui64   lite_engine_entity_create         (void);
void   lite_engine_component_add         (uint64_t ID,
	                                      uint64_t component_flag);
ui8    lite_engine_entity_has_component  (uint64_t entity_ID,
                                          uint64_t component_flag);

#endif
