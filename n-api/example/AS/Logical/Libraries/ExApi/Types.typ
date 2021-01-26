
TYPE
	ExLogLevelEnum : 
		(
		exLOG_LEVEL_ERROR,
		exLOG_LEVEL_WARNING,
		exLOG_LEVEL_SUCCESS,
		exLOG_LEVEL_INFO,
		exLOG_LEVEL_DEBUG
		);
	ExLogConfigFilterType : 	STRUCT 
		User : BOOL;
		System : BOOL;
		Verbose : BOOL;
	END_STRUCT;
	ExLogConfigType : 	STRUCT 
		Level : ExLogLevelEnum;
		Filter : ExLogConfigFilterType;
		ExcludeModule : ARRAY[0..19]OF STRING[35];
		ApplicationConsoleOutput : BOOL;
	END_STRUCT;
	ExApiSystemInfoSyncTimestampType : 	STRUCT 
		T0_MasterSend : DINT;
		T1_SlaveReceive : DINT;
		T2_SlaveSend : DINT;
		T3_MasterReceive : DINT;
	END_STRUCT;
	ExApiSystemInfoCycleTimeType : 	STRUCT 
		CycleTime : DINT;
		IdleTime : DINT;
		CycleTimeViolations : UDINT;
		ProcessTimeStartCyclic : DINT;
		ProcessTimeEndCyclic : DINT;
		ProcessTimeMax : DINT;
	END_STRUCT;
	ExApiSystemInfoCycleType : 	STRUCT 
		AR : ExApiSystemInfoCycleTimeType;
		GPOS : ExApiSystemInfoCycleTimeType;
	END_STRUCT;
	ExApiSystemInfoSyncType : 	STRUCT 
		MasterInSync : USINT;
		SlaveInSync : USINT;
		FilteredDelay : DINT;
		CurrentDelay : DINT;
		PredictionError : DINT;
		AdjustedPrediction : DINT;
		TimeStamps : ExApiSystemInfoSyncTimestampType;
	END_STRUCT;
END_TYPE
