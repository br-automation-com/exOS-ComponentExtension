
PROGRAM _INIT

	MyAppInit_0();
	FOR i := 0 TO 9 DO
		MyAppCyclic_0.parameters[i].x := i + 1;
		MyAppCyclic_0.parameters[i].y := i + 2;
	END_FOR
	
END_PROGRAM

PROGRAM _CYCLIC
	
	MyAppCyclic_0.message := ADR(message);
	MyAppCyclic_0.done := ADR(done);
    MyAppCyclic_0(Handle := MyAppInit_0.Handle);

END_PROGRAM

PROGRAM _EXIT

    MyAppExit_0(Handle := MyAppInit_0.Handle);

END_PROGRAM
