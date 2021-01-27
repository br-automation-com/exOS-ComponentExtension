
PROGRAM _INIT

	SimPanelInit_0();

END_PROGRAM

PROGRAM _CYCLIC
	SimPanelCyclic_0.Knobs := ADR(Knobs);
	SimPanelCyclic_0(Handle := SimPanelInit_0.Handle);
	ExTargetDeploymentInfo_0();
END_PROGRAM

PROGRAM _EXIT

	SimPanelExit_0(Handle := SimPanelInit_0.Handle);

END_PROGRAM
