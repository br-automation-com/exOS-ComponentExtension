
PROGRAM _INIT

    BufferFub2Init_0();

END_PROGRAM

PROGRAM _CYCLIC
    
    //Auto connect:
    //BufferFub2Cyclic_0.Enable := ExComponentInfo_0.Operational; // Component has been deployed and started up successfully
    
    BufferFub2Cyclic_0(Handle := BufferFub2Init_0.Handle, pBufferFub2 := ADR(BufferFub2_0));
    
    BufferFub2BufferUpdate_0.BufferHandle := BufferFub2Init_0.bufferedSampleBufferHandle;
    UpdateBufferSampleSingle;
    //UpdateBufferSampleAll;
    
    ExComponentInfo_0(ExTargetLink := ADR(gTarget_0), ExComponentLink := ADR(gBufferFub2_0), Enable := TRUE);
    
    ExDatamodelInfo_0(ExTargetLink := ADR(gTarget_0), Enable := TRUE, InstanceName := 'BufferFub2_0');
    
END_PROGRAM

PROGRAM _EXIT

    BufferFub2Exit_0(Handle := BufferFub2Init_0.Handle);

END_PROGRAM
