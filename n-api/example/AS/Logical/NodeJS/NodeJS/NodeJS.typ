
TYPE
	Counters_typ : 	STRUCT 
		c : ARRAY[0..10]OF INT;
	END_STRUCT;
	NodeJS : 	STRUCT 
		counters : Counters_typ; (*PUBSUB*)
		run_counter : BOOL; (*PUB*)
	END_STRUCT;
END_TYPE
