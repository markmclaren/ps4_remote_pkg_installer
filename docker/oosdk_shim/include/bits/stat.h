#ifndef _OOSDK_SHIM_BITS_STAT_H
#define _OOSDK_SHIM_BITS_STAT_H

#include <stdint.h>
#include <stddef.h>

/* FreeBSD / Orbis OS kernel struct stat layout (matches SceKernelStat).
 * OpenOrbis toolchain's bits/alltypes.h erroneously defines mode_t as 4-byte
 * unsigned int instead of 2-byte uint16_t, shifting st_size from offset 72
 * to offset 80 (st_blocks). */
struct stat {
	dev_t st_dev;
	ino_t st_ino;
	uint16_t st_mode;
	nlink_t st_nlink;
	uid_t st_uid;
	gid_t st_gid;
	dev_t st_rdev;
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	off_t st_size;
	blkcnt_t st_blocks;
	blksize_t st_blksize;
	uint32_t st_flags;
	uint32_t st_gen;
	int32_t st_lspare;
	struct timespec st_birthtim;
};

_Static_assert(offsetof(struct stat, st_size) == 72, "PS4 struct stat st_size must be at offset 72");
_Static_assert(sizeof(struct stat) == 120, "PS4 struct stat must be 120 bytes");

#endif

