
TYPE
	BufferFubSetup_typ : 	STRUCT 
		sampleCount : UDINT; (*number of samples sent each time cmdSendBurst is set*)
		sampleDelay : UDINT; (*ms between samples*)
	END_STRUCT;
	BufferFub : 	STRUCT 
		bufferedSample : UDINT; (*SUB BUFFERED*)
		setup : BufferFubSetup_typ; (*PUB*)
		cmdSendBurst : BOOL; (*PUB*)
	END_STRUCT;
END_TYPE
