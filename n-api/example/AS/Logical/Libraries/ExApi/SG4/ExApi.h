/* Automation Studio generated header file */
/* Do not edit ! */
/* ExApi 1.00.1 */

#ifndef _EXAPI_
#define _EXAPI_
#ifdef __cplusplus
extern "C" 
{
#endif
#ifndef _ExApi_VERSION
#define _ExApi_VERSION 1.00.1
#endif

#include <bur/plctypes.h>

#ifndef _BUR_PUBLIC
#define _BUR_PUBLIC
#endif
#ifdef _SG3
		#include "sys_lib.h"
		#include "AsTCP.h"
		#include "AsIOTime.h"
		#include "ArEventLog.h"
		#include "FileIO.h"
#endif

#ifdef _SG4
		#include "sys_lib.h"
		#include "AsTCP.h"
		#include "AsIOTime.h"
		#include "ArEventLog.h"
		#include "FileIO.h"
#endif

#ifdef _SGC
		#include "sys_lib.h"
		#include "AsTCP.h"
		#include "AsIOTime.h"
		#include "ArEventLog.h"
		#include "FileIO.h"
#endif



/* Datatypes and datatypes of function blocks */
typedef enum ExLogLevelEnum
{	exLOG_LEVEL_ERROR,
	exLOG_LEVEL_WARNING,
	exLOG_LEVEL_SUCCESS,
	exLOG_LEVEL_INFO,
	exLOG_LEVEL_DEBUG
} ExLogLevelEnum;

typedef struct ExLogConfigFilterType
{	plcbit User;
	plcbit System;
	plcbit Verbose;
} ExLogConfigFilterType;

typedef struct ExLogConfigType
{	enum ExLogLevelEnum Level;
	struct ExLogConfigFilterType Filter;
	plcstring ExcludeModule[20][36];
	plcbit ApplicationConsoleOutput;
} ExLogConfigType;

typedef struct ExApiSystemInfoSyncTimestampType
{	signed long T0_MasterSend;
	signed long T1_SlaveReceive;
	signed long T2_SlaveSend;
	signed long T3_MasterReceive;
} ExApiSystemInfoSyncTimestampType;

typedef struct ExApiSystemInfoCycleTimeType
{	signed long CycleTime;
	signed long IdleTime;
	unsigned long CycleTimeViolations;
	signed long ProcessTimeStartCyclic;
	signed long ProcessTimeEndCyclic;
	signed long ProcessTimeMax;
} ExApiSystemInfoCycleTimeType;

typedef struct ExApiSystemInfoCycleType
{	struct ExApiSystemInfoCycleTimeType AR;
	struct ExApiSystemInfoCycleTimeType GPOS;
} ExApiSystemInfoCycleType;

typedef struct ExApiSystemInfoSyncType
{	unsigned char MasterInSync;
	unsigned char SlaveInSync;
	signed long FilteredDelay;
	signed long CurrentDelay;
	signed long PredictionError;
	signed long AdjustedPrediction;
	struct ExApiSystemInfoSyncTimestampType TimeStamps;
} ExApiSystemInfoSyncType;

typedef struct ExLogConfigChange
{
	/* VAR_INPUT (analog) */
	unsigned long ExLink;
	struct ExLogConfigType* LogConfig;
	/* VAR_OUTPUT (analog) */
	unsigned short ErrorID;
	/* VAR (analog) */
	unsigned long Internal;
	/* VAR_INPUT (digital) */
	plcbit Enable;
	plcbit ChangeConfig;
	plcbit ListenConfigUpdates;
	/* VAR_OUTPUT (digital) */
	plcbit Active;
	plcbit Busy;
	plcbit Error;
	plcbit ConfigChanged;
	plcbit ConfigUpdated;
} ExLogConfigChange_typ;

typedef struct ExApiSystemInfo
{
	/* VAR_INPUT (analog) */
	unsigned long ExLink;
	/* VAR_OUTPUT (analog) */
	struct ExApiSystemInfoCycleType CycleTime;
	struct ExApiSystemInfoSyncType TimeSync;
	/* VAR_INPUT (digital) */
	plcbit Enable;
	plcbit Reset;
	/* VAR_OUTPUT (digital) */
	plcbit Active;
} ExApiSystemInfo_typ;



/* Prototyping of functions and function blocks */
_BUR_PUBLIC void ExLogConfigChange(struct ExLogConfigChange* inst);
_BUR_PUBLIC void ExApiSystemInfo(struct ExApiSystemInfo* inst);


#ifdef __cplusplus
};
#endif
#endif /* _EXAPI_ */

