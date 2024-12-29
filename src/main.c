#include "lite_engine.h"

int main() {
	lite_engine_start();

	ui64 object = lite_engine_entity_create();
	debug_log("object id: %lu", object);

	while (lite_engine_is_running()) {
		lite_engine_update();
	}

	return 0;
}
