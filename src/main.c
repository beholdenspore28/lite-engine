#include "lite_engine.h"

int main(const int argc, const char** argv) {
	(void)argc; (void) argv;

	lite_engine_start();
	lite_engine_update();
	lite_engine_stop();

	debug_log("this is a test log");
	debug_warn("this is a test warning");
	debug_error("this is a test error");
}
