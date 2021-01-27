/* Automation Studio generated header file */
/* Do not edit ! */
/* ExTarget 1.00.2 */

#ifndef _EXTARGET_
#define _EXTARGET_
#ifdef __cplusplus
extern "C" 
{
#endif
#ifndef _ExTarget_VERSION
#define _ExTarget_VERSION 1.00.2
#endif

#include <bur/plctypes.h>

#ifndef _BUR_PUBLIC
#define _BUR_PUBLIC
#endif
#ifdef _SG3
		#include "ExApi.h"
#endif

#ifdef _SG4
		#include "ExApi.h"
#endif

#ifdef _SGC
		#include "ExApi.h"
#endif



/* Datatypes and datatypes of function blocks */
typedef struct ExTargetDeploymentInfoCounters
{	unsigned short ComponentsUninstalled;
	unsigned short ServicesRemoved;
	unsigned short ComponentsInstalled;
	unsigned short ServicesCreated;
	unsigned short FilesDeployed;
	unsigned short ServicesCompleted;
	unsigned short ComponentsRunning;
	unsigned short ServicesRunning;
} ExTargetDeploymentInfoCounters;

typedef struct ExTargetLinkType
{	unsigned long INTERNAL;
} ExTargetLinkType;

typedef struct ExTargetDeploymentInfo
{
	/* VAR_INPUT (analog) */
	unsigned long ExTargetLink;
	/* VAR_OUTPUT (analog) */
	unsigned long Failures;
	struct ExTargetDeploymentInfoCounters Counters;
	/* VAR_INPUT (digital) */
	plcbit Enable;
	/* VAR_OUTPUT (digital) */
	plcbit Active;
	plcbit DeploymentOn;
	plcbit Initialized;
	plcbit Connected;
	plcbit Stopped;
	plcbit Deinstalled;
	plcbit Transferred;
	plcbit Installed;
	plcbit Done;
	plcbit Restarting;
	plcbit Failed;
} ExTargetDeploymentInfo_typ;



/* Prototyping of functions and function blocks */
_BUR_PUBLIC void ExTargetDeploymentInfo(struct ExTargetDeploymentInfo* inst);


#ifdef __cplusplus
};
#endif
#endif /* _EXTARGET_ */

