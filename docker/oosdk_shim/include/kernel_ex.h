#pragma once

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

#define SCE_KERNEL_PRIO_FIFO_DEFAULT 700
#define SCE_KERNEL_PATH_MAX          1024
#define SCE_KERNEL_MAX_NAME_LENGTH   256
#define SCE_KERNEL_MAX_MODULES       256

#define SCE_KERNEL_ERROR_ENOENT      0x80020002
#define SCE_KERNEL_ERROR_ENXIO       0x80020006
#define SCE_KERNEL_ERROR_EFAULT      0x8002000E
#define SCE_KERNEL_ERROR_EINVAL      0x80020016
#define SCE_KERNEL_ERROR_ENOSPC      0x8002001C

#define SCE_KERNEL_PROT_CPU_READ     0x01
#define SCE_KERNEL_PROT_CPU_WRITE    0x02
#define SCE_KERNEL_PROT_CPU_EXEC     0x04

#define SCE_KERNEL_DT_REG            8
#define SCE_KERNEL_S_ISREG(m)        (((m) & 0170000) == 0100000)

typedef uint16_t SceKernelMode;
typedef int32_t  SceKernelModule;

typedef struct SceKernelStat {
	dev_t           st_dev;
	ino_t           st_ino;
	SceKernelMode   st_mode;
	nlink_t         st_nlink;
	uid_t           st_uid;
	gid_t           st_gid;
	dev_t           st_rdev;
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	off_t           st_size;
	blkcnt_t        st_blocks;
	blksize_t       st_blksize;
	uint32_t        st_flags;
	uint32_t        st_gen;
	int32_t         st_lspare;
	struct timespec st_birthtim;
	unsigned int    :(8 / 2) * (16 - (int)sizeof(struct timespec));
	unsigned int    :(8 / 2) * (16 - (int)sizeof(struct timespec));
} SceKernelStat;

typedef struct SceKernelDirent {
	uint32_t d_fileno;
	uint16_t d_reclen;
	uint8_t  d_type;
	uint8_t  d_namlen;
	char     d_name[255 + 1];
} SceKernelDirent;

typedef struct SceKernelLoadModuleOpt {
	size_t size;
} SceKernelLoadModuleOpt;

typedef struct SceKernelSegmentInfo {
	void*    baseAddr;
	uint32_t size;
	int32_t  prot;
} SceKernelSegmentInfo;

typedef struct SceKernelModuleInfo {
	size_t               size;
	char                 name[256];
	SceKernelSegmentInfo segmentInfo[4];
	uint32_t             numSegments;
	uint8_t              fingerprint[20];
} SceKernelModuleInfo;

typedef struct SceKernelModuleInfoEx {
	size_t               size;
	char                 name[256];
	SceKernelModule      handle;
	uint32_t             tls_index;
	void*                tls_init_addr;
	uint32_t             tls_init_size;
	uint32_t             tls_size;
	uint32_t             tls_offset;
	uint32_t             tls_align;
	void*                init_proc_addr;
	void*                fini_proc_addr;
	uint64_t             reserved1;
	uint64_t             reserved2;
	void*                eh_frame_hdr_addr;
	void*                eh_frame_addr;
	uint32_t             eh_frame_hdr_size;
	uint32_t             eh_frame_size;
	SceKernelSegmentInfo segmentInfo[4];
	uint32_t             numSegments;
	uint32_t             ref_count;
} SceKernelModuleInfoEx;

int sceKernelStat(const char* path, SceKernelStat* sb);
int sceKernelOpen(const char* path, int flags, int mode);
int sceKernelClose(int fd);
int sceKernelGetdents(int fd, char* buf, int nbytes);
int sceKernelGettimeofday(struct timeval* tp);
const char* sceKernelGetFsSandboxRandomWord(void);
SceKernelModule sceKernelLoadStartModule(const char* moduleFileName, size_t args, const void* argp, unsigned int flags, const SceKernelLoadModuleOpt* pOpt, int* pRes);
int sceKernelGetModuleList(SceKernelModule* handles, size_t num_handles, size_t* out_count);
int sceKernelMprotect(const void* addr, size_t len, int prot);

