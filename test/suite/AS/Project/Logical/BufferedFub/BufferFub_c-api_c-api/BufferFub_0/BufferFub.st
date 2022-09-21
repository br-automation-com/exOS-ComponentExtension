
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
			// startup
			BufferFubCyclic_0.Enable := TRUE;
			IF BufferFubCyclic_0.Connected THEN
				BufferFubCyclic_0.Start := TRUE;
				testState := 10;
			END_IF
		10:
			IF BufferFubCyclic_0.Operational THEN
				testState := 20;
			END_IF
		20:
			// tell linux to burst
			BufferFub_0.setup.sampleCount := 12;
			BufferFub_0.setup.sampleDelay := 0;
			BufferFub_0.cmdSendBurst := TRUE;
			testState := 25;
		30:
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
