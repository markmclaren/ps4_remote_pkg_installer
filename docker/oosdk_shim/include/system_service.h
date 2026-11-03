#pragma once

#include <stdint.h>

#define SCE_SYSTEM_SERVICE_PARAM_ID_LANG 1

int sceSystemServiceParamGetInt(int paramId, int* value);
int sceSystemServiceLoadExec(const char* path, const char* const argv[]);

