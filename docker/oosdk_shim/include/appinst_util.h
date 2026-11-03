#pragma once

#include <stdint.h>
#include <stdbool.h>

int sceAppInstUtilInitialize(void);
int sceAppInstUtilTerminate(void);
int sceAppInstUtilAppUnInstall(const char* title_id);
int sceAppInstUtilAppUnInstallAddcont(const char* title_id, const char* addcont_name);
int sceAppInstUtilAppUnInstallPat(const char* title_id);
int sceAppInstUtilAppUnInstallTheme(const char* content_id);
int sceAppInstUtilAppExists(const char* title_id, int* flag);
int sceAppInstUtilAppGetSize(const char* title_id, unsigned long* size);

