
(*Read a single change on BufferFub2BufferUpdate_0.BufferHandle per scan*)
ACTION UpdateBufferSampleSingle:
    BufferFub2BufferUpdate_0(UpdateDataset := TRUE);
    
    IF BufferFub2BufferUpdate_0.DatasetUpdated THEN
    
        (* Your code here *)
        
    END_IF
END_ACTION


(*Read all changes on BufferFub2BufferUpdate_0.BufferHandle within a single scan*)
ACTION UpdateBufferSampleAll:
    
    BufferFub2BufferUpdate_0(UpdateDataset := FALSE); // update PendingUpdates without requesting UpdateDataset
    
    WHILE BufferFub2BufferUpdate_0.PendingUpdates > 0 DO
    
        BufferFub2BufferUpdate_0(UpdateDataset := TRUE);
        
        (* Your code here *)
        
    END_WHILE
END_ACTION
