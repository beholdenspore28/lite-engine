#include "lite_engine.h"

static ui64 internal_entities_count;

ui64 lite_engine_entity_create(void) {
	internal_entities_count++;
	return internal_entities_count;	
}
