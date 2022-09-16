
PROGRAM _INIT

    BufferFubInit_0();

	// test code begin
	BufferedSampleRecvIdx := 0;
	// test code end
	
END_PROGRAM

PROGRAM _CYCLIC
    
    //Auto connect:
    //BufferFubCyclic_0.Enable := ExComponentInfo_0.Operational; // Component has been deployed and started up successfully
    
    BufferFubCyclic_0(Handle := BufferFubInit_0.Handle, pBufferFub := ADR(BufferFub_0));
    
	BufferFubBufferUpdate_0.Enable := TRUE;
	BufferFubBufferUpdate_0.BufferHandle := BufferFubInit_0.bufferedSampleBufferHandle;
	//BufferFubBufferUpdate_0.Address := ADR(BufferFub_0.bufferedSample);
	
	// test code begin
	CASE testState OF
		0:
			// nothing
		5:
			BufferFubBufferUpdate_0.testAddRandomDatasets := 5;
			testState := 10;
		10:
			//fill data
			BufferFubBufferUpdate_0(UpdateDataset := FALSE);
			BufferFubBufferUpdate_0.testAddRandomDatasets := 0;
			testState := 0;
		20:
			//update single
			UpdateBufferSampleSingle;
			testState := 0;
		30:
			//update single
			UpdateBufferSampleAll;
			testState := 0;
		 
	
	END_CASE;
	// test code end
	
	
	//UpdateBufferSampleSingle;
	//UpdateBufferSampleAll;
	
    ExComponentInfo_0(ExTargetLink := ADR(gTarget_0), ExComponentLink := ADR(gBufferFub_0), Enable := TRUE);
    
    ExDatamodelInfo_0(ExTargetLink := ADR(gTarget_0), Enable := TRUE, InstanceName := 'BufferFub_0');
    
END_PROGRAM

PROGRAM _EXIT

    BufferFubExit_0(Handle := BufferFubInit_0.Handle);

END_PROGRAM
