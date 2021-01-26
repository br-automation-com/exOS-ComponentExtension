
FUNCTION_BLOCK ExLogConfigChange
	VAR_INPUT
		ExLink : UDINT;
		Enable : BOOL;
		LogConfig : REFERENCE TO ExLogConfigType;
		ChangeConfig : BOOL;
		ListenConfigUpdates : BOOL;
	END_VAR
	VAR_OUTPUT
		Active : BOOL;
		Busy : BOOL;
		Error : BOOL;
		ErrorID : UINT;
		ConfigChanged : BOOL;
		ConfigUpdated : BOOL;
	END_VAR
	VAR
		Internal : UDINT;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK ExApiSystemInfo
	VAR_INPUT
		ExLink : UDINT;
		Enable : BOOL;
		Reset : BOOL;
	END_VAR
	VAR_OUTPUT
		Active : BOOL;
		CycleTime : ExApiSystemInfoCycleType;
		TimeSync : ExApiSystemInfoSyncType;
	END_VAR
END_FUNCTION_BLOCK


































































































