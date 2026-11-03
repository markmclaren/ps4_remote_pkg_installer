#pragma once

#include <stddef.h>
#include <stdint.h>

typedef int SceNetId;

int* sceNetErrnoLoc(void);
#define sce_net_errno (*sceNetErrnoLoc())

int sceNetInit(void);
int sceNetTerm(void);
int sceNetPoolCreate(const char* name, int size, int flags);
int sceNetPoolDestroy(int memid);

int sceNetSend(SceNetId s, const void* buf, size_t len, int flags);
int sceNetRecv(SceNetId s, void* buf, size_t len, int flags);

