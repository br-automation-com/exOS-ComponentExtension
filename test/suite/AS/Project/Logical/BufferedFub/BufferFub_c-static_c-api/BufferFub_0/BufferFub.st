
PROGRAM _INIT

END_PROGRAM

PROGRAM _CYCLIC
    
    //Auto connect:
    //BufferFubCyclic_0.Enable := ExComponentInfo_0.Operational; // Component has been deployed and started up successfully
    
    BufferFubCyclic_0(pBufferFub := ADR(BufferFub_0));
    
    ExComponentInfo_0(ExTargetLink := ADR(gTarget_0), ExComponentLink := ADR(gBufferFub_0), Enable := TRUE);
    
    ExDatamodelInfo_0(ExTargetLink := ADR(gTarget_0), Enable := TRUE, InstanceName := 'BufferFub_0');
    

END_PROGRAM

PROGRAM _EXIT

    BufferFubCyclic_0(Enable := FALSE);

END_PROGRAM
