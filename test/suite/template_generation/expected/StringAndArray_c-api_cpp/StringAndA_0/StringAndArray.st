
PROGRAM _INIT

    StringAndArrayInit_0();

END_PROGRAM

PROGRAM _CYCLIC
    
    //Auto connect:
    //StringAndArrayCyclic_0.Enable := ExComponentInfo_0.Operational; // Component has been deployed and started up successfully
    
    StringAndArrayCyclic_0(Handle := StringAndArrayInit_0.Handle, pStringAndArray := ADR(StringAndArray_0));
    
    ExComponentInfo_0(ExTargetLink := ADR(gTarget_0), ExComponentLink := ADR(gStringAndArray_0), Enable := TRUE);
    
    ExDatamodelInfo_0(ExTargetLink := ADR(gTarget_0), Enable := TRUE, InstanceName := 'StringAndArray_0');
    
END_PROGRAM

PROGRAM _EXIT

    StringAndArrayExit_0(Handle := StringAndArrayInit_0.Handle);

END_PROGRAM
