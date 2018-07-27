/** Modifies the linker so that it uses proper symbol names in MSVC. **/
#ifdef OS_Windows
#pragma comment(linker, "/EXPORT:create=create")
#pragma comment(linker, "/EXPORT:load=load")
#pragma comment(linker, "/EXPORT:update=update")
#pragma comment(linker, "/EXPORT:render=render")
#pragma comment(linker, "/EXPORT:unload=unload")
#pragma comment(linker, "/EXPORT:destroy=destroy")
#endif

#include <hap.h>

#include <stdlib.h>
#include <stdio.h>

#include "window_management.h"


void* create(HAPEngine *engine, HAPConfigurationSection *configuration) {
	void *window = window_create(engine, 640, 480);

	if (window == NULL) {
		(*engine).log_error(engine, "Could not create a window.\n");
		return NULL;
	}

	return window;
}


void load(HAPEngine *engine, void *state, char *identifier) {
	(void)engine;      // Mark variable as used to avoid compiler warnings
	(void)state;       // Mark variable as used to avoid compiler warnings
	(void)identifier;  // Mark variable as used to avoid compiler warnings
}


HAPTime update(HAPEngine *engine, void *state) {
	return 0;
}


void unload(HAPEngine *engine, void *state) {
	(void)engine;      // Mark variable as used to avoid compiler warnings
	(void)state;       // Mark variable as used to avoid compiler warnings
}


void render(HAPEngine *engine, void *state) {
	(void)engine;      // Mark variable as used to avoid compiler warnings

	if (window_update(state) >= 0)
		window_render(state);
}


void destroy(HAPEngine *engine, void *state) {
	(void)engine;      // Mark variable as used to avoid compiler warnings

	window_close(state);
}
