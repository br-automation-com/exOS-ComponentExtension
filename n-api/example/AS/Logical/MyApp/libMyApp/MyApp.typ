
TYPE
	MyApp : 	STRUCT 
		execute : BOOL; (*PUB*)
		done : BOOL; (*PUBSUB*)
		parameters : {REDUND_UNREPLICABLE} ARRAY[0..9]OF MyAppPar_t; (*PUB*)
		results : ARRAY[0..9]OF MyAppRes_t; (*SUB*)
	END_STRUCT;
	MyAppPar_t : 	STRUCT 
		x : INT;
		y : INT;
	END_STRUCT;
	MyAppRes_t : 	STRUCT 
		product : DINT;
	END_STRUCT;
END_TYPE
