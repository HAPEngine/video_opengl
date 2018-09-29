#include <hap.h>

HAP_MODULE_EXPORT void* create(HAPEngine *engine, HAPConfigurationSection *configuration);
HAP_MODULE_EXPORT void load(HAPEngine *engine, void *state, char *identifier);
HAP_MODULE_EXPORT HAPTime update(HAPEngine *engine, void *state);
HAP_MODULE_EXPORT void render(HAPEngine *engine, void *state);
HAP_MODULE_EXPORT void unload(HAPEngine *engine, void *state);
HAP_MODULE_EXPORT void destroy(HAPEngine *engine, void *state);
