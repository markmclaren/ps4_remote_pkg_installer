#pragma once

#include <stddef.h>
#include <stdint.h>

#define SCE_BGFT_INVALID_TASK_ID (-1)
#define SCE_BGFT_ERROR_SAME_APPLICATION_ALREADY_INSTALLED (0x80990088)

typedef int SceBgftTaskId;

typedef struct _SceBgftInitParams {
	void*  heap;
	size_t heapSize;
} SceBgftInitParams;

typedef enum _SceBgftTaskSubType {
	SCE_BGFT_TASK_SUB_TYPE_UNKNOWN      = 0,
	SCE_BGFT_TASK_SUB_TYPE_PHOTO        = 1,
	SCE_BGFT_TASK_SUB_TYPE_MUSIC        = 2,
	SCE_BGFT_TASK_SUB_TYPE_VIDEO        = 3,
	SCE_BGFT_TASK_SUB_TYPE_MARLIN_VIDEO = 4,
	SCE_BGFT_TASK_SUB_TYPE_UPDATA_ORBIS = 5,
	SCE_BGFT_TASK_SUB_TYPE_GAME         = 6,
	SCE_BGFT_TASK_SUB_TYPE_GAME_AC      = 7,
	SCE_BGFT_TASK_SUB_TYPE_GAME_PATCH   = 8,
	SCE_BGFT_TASK_SUB_TYPE_GAME_LICENSE = 9,
	SCE_BGFT_TASK_SUB_TYPE_SAVE_DATA    = 10,
	SCE_BGFT_TASK_SUB_TYPE_CRASH_REPORT = 11,
	SCE_BGFT_TASK_SUB_TYPE_PACKAGE      = 12,
	SCE_BGFT_TASK_SUB_TYPE_MAX          = 13,
} SceBgftTaskSubType;

typedef enum _SceBgftTaskOpt {
	SCE_BGFT_TASK_OPT_NONE                    = 0x0,
	SCE_BGFT_TASK_OPT_DELETE_AFTER_UPLOAD     = 0x1,
	SCE_BGFT_TASK_OPT_INVISIBLE               = 0x2,
	SCE_BGFT_TASK_OPT_ENABLE_PLAYGO           = 0x4,
	SCE_BGFT_TASK_OPT_FORCE_UPDATE            = 0x8,
	SCE_BGFT_TASK_OPT_REMOTE                  = 0x10,
	SCE_BGFT_TASK_OPT_COPY_CRASH_REPORT_FILES = 0x20,
	SCE_BGFT_TASK_OPT_DISABLE_INSERT_POPUP    = 0x40,
	SCE_BGFT_TASK_OPT_INTERNAL                = 0x80,
	SCE_BGFT_TASK_OPT_DISABLE_CDN_QUERY_PARAM = 0x10000,
} SceBgftTaskOpt;

typedef struct _SceBgftDownloadRegisterErrorInfo {
	uint8_t buf[0x100];
} SceBgftDownloadRegisterErrorInfo;

typedef struct _SceBgftDownloadParam {
	int            userId;
	int            entitlementType;
	const char*    id;
	const char*    contentUrl;
	const char*    contentExUrl;
	const char*    contentName;
	const char*    iconPath;
	const char*    skuId;
	SceBgftTaskOpt option;
	const char*    playgoScenarioId;
	const char*    releaseDate;
	const char*    packageType;
	const char*    packageSubType;
	unsigned long  packageSize;
} SceBgftDownloadParam;

typedef struct _SceBgftTaskProgress {
	unsigned int  bits;
	int           errorResult;
	unsigned long length;
	unsigned long transferred;
	unsigned long lengthTotal;
	unsigned long transferredTotal;
	unsigned int  numIndex;
	unsigned int  numTotal;
	unsigned int  restSec;
	unsigned int  restSecTotal;
	int           preparingPercent;
	int           localCopyPercent;
} SceBgftTaskProgress;

int sceBgftServiceIntInit(SceBgftInitParams* params);
int sceBgftServiceIntTerm(void);
int sceBgftServiceIntDownloadRegisterTask(SceBgftDownloadParam* params, SceBgftTaskId* taskId);
int sceBgftServiceIntDebugDownloadRegisterPkg(SceBgftDownloadParam* params, SceBgftTaskId* taskId);
int sceBgftServiceIntDownloadStartTask(SceBgftTaskId taskId);
int sceBgftServiceIntDownloadStopTask(SceBgftTaskId taskId);
int sceBgftServiceIntDownloadPauseTask(SceBgftTaskId taskId);
int sceBgftServiceIntDownloadResumeTask(SceBgftTaskId taskId);
int sceBgftServiceIntDownloadUnregisterTask(SceBgftTaskId taskId);
int sceBgftServiceIntDownloadReregisterTaskPatch(SceBgftTaskId oldTaskId, SceBgftTaskId* newTaskId);
int sceBgftServiceIntDownloadGetProgress(SceBgftTaskId taskId, SceBgftTaskProgress* progress);
int sceBgftServiceDownloadFindTaskByContentId(const char* contentId, SceBgftTaskSubType subType, SceBgftTaskId* taskId);

#define sceBgftInitialize                  sceBgftServiceIntInit
#define sceBgftFinalize                    sceBgftServiceIntTerm
#define sceBgftDownloadRegisterTask        sceBgftServiceIntDownloadRegisterTask
#define sceBgftDebugDownloadRegisterPkg    sceBgftServiceIntDebugDownloadRegisterPkg
#define sceBgftDownloadStartTask           sceBgftServiceIntDownloadStartTask
#define sceBgftDownloadStopTask            sceBgftServiceIntDownloadStopTask
#define sceBgftDownloadPauseTask           sceBgftServiceIntDownloadPauseTask
#define sceBgftDownloadResumeTask          sceBgftServiceIntDownloadResumeTask
#define sceBgftDownloadUnregisterTask      sceBgftServiceIntDownloadUnregisterTask
#define sceBgftDownloadReregisterTaskPatch sceBgftServiceIntDownloadReregisterTaskPatch
#define sceBgftDownloadGetProgress         sceBgftServiceIntDownloadGetProgress

