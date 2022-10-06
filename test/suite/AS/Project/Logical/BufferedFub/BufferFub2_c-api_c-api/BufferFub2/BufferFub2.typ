
TYPE
	BufferFub2Setup_typ : 	STRUCT 
		sampleCount : UDINT; (*number of samples sent each time cmdSendBurst is set*)
		sampleDelay : UDINT; (*ms between samples*)
	END_STRUCT;
	BufferFub2SubStruct_typ : 	STRUCT 
		moreid : UDINT; (*id number*)
		moredata : REAL; (*some data*)
	END_STRUCT;
	BufferFub2Sample_typ : 	STRUCT 
		id : UDINT; (*id number*)
		data : REAL; (*some data*)
		subStruct : BufferFub2SubStruct_typ; (*a sub structure*)
	END_STRUCT;
	BufferFub2 : 	STRUCT 
		bufferedSample : BufferFub2Sample_typ; (*SUB BUFFERED*)
		setup : BufferFub2Setup_typ; (*PUB*)
		cmdSendBurst : BOOL; (*PUB*)
	END_STRUCT;
END_TYPE
