
FUNCTION_BLOCK ExTargetDeploymentInfo
	VAR_INPUT
		ExTargetLink : {REDUND_UNREPLICABLE} UDINT;
		Enable : BOOL;
	END_VAR
	VAR_OUTPUT
		Active : BOOL;
		DeploymentOn : BOOL;
		Initialized : BOOL;
		Connected : BOOL;
		Stopped : BOOL;
		Deinstalled : BOOL;
		Transferred : BOOL;
		Installed : BOOL;
		Done : BOOL;
		Restarting : BOOL;
		Failures : UDINT;
		Failed : BOOL;
		Counters : ExTargetDeploymentInfoCounters;
	END_VAR
END_FUNCTION_BLOCK
