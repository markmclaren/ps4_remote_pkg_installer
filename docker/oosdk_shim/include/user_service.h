#pragma once

#include <stdint.h>

int sceUserServiceInitialize(void* params);
int sceUserServiceTerminate(void);
int sceUserServiceGetForegroundUser(int* user_id);

