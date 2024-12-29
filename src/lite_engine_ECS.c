#include "lite_engine.h"

static ui64 internal_entities_count;

// An archetype is defined as a list of components.
// They are often used to define which components are 
// required by a given system.
typedef ui8 *archetype_t;

enum {
	COMPONENT_TRANSFORM,
	COMPONENT_MESH,
	COMPONENT_MATERIAL,
	COMPONENT_LIGHT,
	COMPONENT_CAMERA,
};

typedef struct {
	ui8 ID;
	void (*system) (archetype_t a);	
} component_t;

ui64 lite_engine_entity_create(void) {
	internal_entities_count++;
	return internal_entities_count;	
}
