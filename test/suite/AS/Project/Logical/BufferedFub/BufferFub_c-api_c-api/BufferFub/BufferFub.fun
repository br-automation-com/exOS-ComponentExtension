FUNCTION_BLOCK BufferFubInit
	VAR_OUTPUT
		Handle : UDINT;
		bufferedSampleBufferHandle : UDINT; (*Initialized BufferHandle for bufferedSample. Bufferhandle is linked to data in Cyclic*)
	END_VAR
	VAR
		_state : USINT;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK BufferFubCyclic
	VAR_INPUT
		Enable : BOOL;
		Handle : UDINT;
		Start : BOOL;
		pBufferFub : REFERENCE TO BufferFub;
	END_VAR
	VAR_OUTPUT
		Active : BOOL;
		Error : BOOL;
		Disconnected : BOOL;
		Connected : BOOL;
		Operational : BOOL;
		Aborted : BOOL;
	END_VAR
	VAR
		_state : USINT;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK BufferFubExit
	VAR_INPUT
		Handle : UDINT;
	END_VAR
	VAR
		_state : USINT;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK BufferFubBufferUpdate (*Must be called in same TC as Cyclic. This function block finalizes in one scan and can be used for several different buffers*)
	VAR_INPUT
		BufferHandle : UDINT; (*Set this to the correct bufferhandle (from Init function block) before calling action UpdateBufferSampleSingle or UpdateBufferSampleAll*)
		UpdateDataset : BOOL; (*Set to TRUE to update dataset, PendingUpdates and OverflowErrors. Set to FALSE to only update PendingUpdates and OverflowErrors for BufferHandle without updating dataset*)
	END_VAR
	VAR_OUTPUT
		Error : BOOL; (*Set in case BufferHandle is invalid*)
		DatasetUpdated : BOOL; (*Set if dataset was updated successfully. Only when UpdateDataset is set to TRUE and there is something in the buffer*)
		PendingUpdates : UDINT; (*Number of datasets waiting in the buffer*)
		OverflowErrors : UDINT; (*Number of overflow errors which occurs when the buffer is full and new data arrives from GPOS*)
	END_VAR
END_FUNCTION_BLOCK
