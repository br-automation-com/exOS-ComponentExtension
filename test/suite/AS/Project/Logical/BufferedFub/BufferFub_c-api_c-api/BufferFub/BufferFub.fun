
FUNCTION_BLOCK BufferFubInit
	VAR_OUTPUT
		Handle : UDINT;
		bufferedSampleBufferHandle : UDINT;
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

FUNCTION_BLOCK BufferFubBufferUpdate (*Must be called in same TC as BufferFubCyclic*)
	VAR_INPUT
		Enable : BOOL;
		BufferHandle : UDINT;
		UpdateDataset : BOOL;
		testAddRandomDatasets : UDINT; (*test code: Just for testing (fill buffer with the set number of numbers)*)
		Address : UDINT;
		Size : UDINT;
	END_VAR
	VAR_OUTPUT
		Active : BOOL;
		Error : BOOL;
		DatasetUpdated : BOOL;
		PendingUpdates : UDINT;
		OverflowErrors : UDINT;
	END_VAR
END_FUNCTION_BLOCK
