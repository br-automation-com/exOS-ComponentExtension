FUNCTION_BLOCK BufferFubInit
    VAR_OUTPUT
        Handle : UDINT;
    END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK BufferFubCyclic
    VAR_INPUT
        Enable : BOOL;
        Start : BOOL;
        Handle : UDINT;
        pBufferFub : REFERENCE TO BufferFub;
    END_VAR
    VAR_OUTPUT
        Connected : BOOL;
        Operational : BOOL;
        Error : BOOL;
    END_VAR
    VAR
        _Start : BOOL;
        _Enable : BOOL;
    END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK BufferFubExit
    VAR_INPUT
        Handle : UDINT;
    END_VAR
END_FUNCTION_BLOCK

