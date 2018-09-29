#include <hap.h>

#include <stdlib.h>
#include <stdio.h>

#include "window_management.h"


HAP_MODULE_EXPORT void* create(HAPEngine *engine, HAPConfigurationSection *configuration) {
	void *window = window_create(engine, 640, 480);

	if (window == NULL) {
		(*engine).log_error(engine, "Could not create a window.\n");
		return NULL;
	}

	return window;
}


HAP_MODULE_EXPORT void load(HAPEngine *engine, void *state, char *identifier) {
	(void)engine;      // Mark variable as used to avoid compiler warnings
	(void)state;       // Mark variable as used to avoid compiler warnings
	(void)identifier;  // Mark variable as used to avoid compiler warnings
}


HAP_MODULE_EXPORT HAPTime update(HAPEngine *engine, void *state) {
	return 0;
}


HAP_MODULE_EXPORT void unload(HAPEngine *engine, void *state) {
	(void)engine;      // Mark variable as used to avoid compiler warnings
	(void)state;       // Mark variable as used to avoid compiler warnings
}


HAP_MODULE_EXPORT void render(HAPEngine *engine, void *state) {
	(void)engine;      // Mark variable as used to avoid compiler warnings

	if (window_update(state) >= 0)
		window_render(state);
}


HAP_MODULE_EXPORT void destroy(HAPEngine *engine, void *state) {
	(void)engine;      // Mark variable as used to avoid compiler warnings

	window_close(state);
}
