#pragma once

#include <stddef.h>
#include <stdint.h>

int sceNpUtilJsonEscape(char* out, size_t max_out_size, const char* in, size_t in_size);

