enum ENTITY_ENUM { ENTITY_NULL, ENTITY_COUNT_MAX = 1024, };

enum COMPONENT_TYPE { 
	COMPONENT_NULL,
	COMPONENT_KINEMATIC_BODY, 
	COMPONENT_COLLIDER, 
	COMPONENT_TRANSFORM,
	COMPONENT_POINT_LIGHT,
	COMPONENT_MESH,
	COMPONENT_MATERIAL,
	COMPONENT_SHADER,
	COMPONENT_COUNT_MAX,
};

void ecs_alloc(void);
int ecs_entity_create(void);
int ecs_component_add(int entity, int component);
void ecs_component_remove(int entity, int component);
int ecs_component_exists(int entity, int component);

