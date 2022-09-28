
(*Read a single change on BufferFubBufferUpdate_0.BufferHandle per scan*)
ACTION UpdateBufferSampleSingle:
	BufferFubBufferUpdate_0(UpdateDataset := TRUE);
	
	IF BufferFubBufferUpdate_0.DatasetUpdated THEN
	
		(* Your code here *)
		
	END_IF
END_ACTION


(*Read all changes on BufferFubBufferUpdate_0.BufferHandle within a single scan*)
ACTION UpdateBufferSampleAll:
	
	BufferFubBufferUpdate_0(UpdateDataset := FALSE); // update PendingUpdates without requesting UpdateDataset
	
	WHILE BufferFubBufferUpdate_0.PendingUpdates > 0 DO
	
		BufferFubBufferUpdate_0(UpdateDataset := TRUE);
				
		(* Your code here *)
		
	END_WHILE
END_ACTION