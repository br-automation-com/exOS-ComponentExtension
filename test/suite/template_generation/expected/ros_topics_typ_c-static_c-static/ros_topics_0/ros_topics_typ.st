
PROGRAM _INIT

END_PROGRAM

PROGRAM _CYCLIC
    
    //Auto connect:
    //ros_topics_typCyclic_0.Enable := ExComponentInfo_0.Operational; // Component has been deployed and started up successfully
    
    ros_topics_typCyclic_0(pros_topics_typ := ADR(ros_topics_typ_0));
    
    ExComponentInfo_0(ExTargetLink := ADR(gTarget_0), ExComponentLink := ADR(gros_topics_typ_0), Enable := TRUE);
    
    ExDatamodelInfo_0(ExTargetLink := ADR(gTarget_0), Enable := TRUE, InstanceName := 'ros_topics_typ_0');
    

END_PROGRAM

PROGRAM _EXIT

    ros_topics_typCyclic_0(Enable := FALSE);

END_PROGRAM
