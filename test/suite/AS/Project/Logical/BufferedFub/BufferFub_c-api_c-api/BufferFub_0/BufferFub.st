
PROGRAM _INIT

    BufferFubInit_0();

END_PROGRAM

PROGRAM _CYCLIC
    
    //Auto connect:
    //BufferFubCyclic_0.Enable := ExComponentInfo_0.Operational; // Component has been deployed and started up successfully
    
    BufferFubCyclic_0(Handle := BufferFubInit_0.Handle, pBufferFub := ADR(BufferFub_0));
    
    BufferFubBufferUpdate_0.BufferHandle := BufferFubInit_0.bufferedSampleBufferHandle;
    UpdateBufferSampleSingle;
    //UpdateBufferSampleAll;
    
    ExComponentInfo_0(ExTargetLink := ADR(gTarget_0), ExComponentLink := ADR(gBufferFub_0), Enable := TRUE);
    
    ExDatamodelInfo_0(ExTargetLink := ADR(gTarget_0), Enable := TRUE, InstanceName := 'BufferFub_0');
    
END_PROGRAM

PROGRAM _EXIT

    BufferFubExit_0(Handle := BufferFubInit_0.Handle);

END_PROGRAM
