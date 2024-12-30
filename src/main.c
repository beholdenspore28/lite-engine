#include "lite_engine.h"

int main() {
	lite_engine_start();

	while (lite_engine_is_running()) {
		lite_engine_update();
	}

	return 0;
}
