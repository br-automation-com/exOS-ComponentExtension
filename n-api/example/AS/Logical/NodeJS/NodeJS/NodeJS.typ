TYPE
	NodeJS : 	STRUCT 
		start : BOOL; (*PUB*)
		reset : BOOL; (*PUB*)
		countUp : DINT; (*SUB*)
		countDown : DINT; (*SUB*)
	END_STRUCT;
END_TYPE