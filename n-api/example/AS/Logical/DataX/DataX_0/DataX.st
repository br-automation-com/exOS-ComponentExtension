
PROGRAM _INIT

    DataXInit_0();

END_PROGRAM

PROGRAM _CYCLIC

    DataXCyclic_0(Handle := DataXInit_0.Handle);

END_PROGRAM

PROGRAM _EXIT

    DataXExit_0(Handle := DataXInit_0.Handle);

END_PROGRAM
