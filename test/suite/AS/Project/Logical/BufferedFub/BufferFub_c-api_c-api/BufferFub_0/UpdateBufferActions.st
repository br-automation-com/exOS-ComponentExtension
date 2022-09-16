
(*Read a single change on bufferedSample per scan*)
ACTION UpdateBufferSampleSingle:
	BufferFubBufferUpdate_0(UpdateDataset := TRUE);
	
	
	IF BufferFubBufferUpdate_0.DatasetUpdated THEN
		(* Your code here *)
		
		// test code begin
		BufferedSampleRecv[BufferedSampleRecvIdx] := BufferFub_0.bufferedSample;
		
		BufferedSampleRecvIdx := BufferedSampleRecvIdx + 1;
		IF BufferedSampleRecvIdx > 19 THEN
			BufferedSampleRecvIdx := 0;
		END_IF
		// test code end
		
	END_IF
END_ACTION


(*Read all changes on bufferedSample within a single scan*)
ACTION UpdateBufferSampleAll:
	
	BufferFubBufferUpdate_0(UpdateDataset := FALSE); // update PendingUpdates without requesting UpdateDataset
	
	WHILE BufferFubBufferUpdate_0.PendingUpdates > 0 DO
	
		BufferFubBufferUpdate_0(UpdateDataset := TRUE);
				
	
		// test code begin
		BufferedSampleRecv[BufferedSampleRecvIdx] := BufferFub_0.bufferedSample;
		
		BufferedSampleRecvIdx := BufferedSampleRecvIdx + 1;
		IF BufferedSampleRecvIdx > 19 THEN
			BufferedSampleRecvIdx := 0;
		END_IF
		// test code end
		
	END_WHILE;
END_ACTION