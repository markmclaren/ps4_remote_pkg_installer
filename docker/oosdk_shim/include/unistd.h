#pragma once

#define syscall __openorbis_libc_syscall
#include_next <unistd.h>
#undef syscall

