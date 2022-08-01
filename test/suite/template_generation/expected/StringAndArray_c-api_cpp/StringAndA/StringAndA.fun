FUNCTION_BLOCK StringAndArrayInit
	VAR_OUTPUT
		Handle : UDINT;
	END_VAR
	VAR
		_state : USINT;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK StringAndArrayCyclic
	VAR_INPUT
		Enable : BOOL;
		Handle : UDINT;
		Start : BOOL;
		pStringAndArray : REFERENCE TO StringAndArray;
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

FUNCTION_BLOCK StringAndArrayExit
	VAR_INPUT
		Handle : UDINT;
	END_VAR
	VAR
		_state : USINT;
	END_VAR
END_FUNCTION_BLOCK
