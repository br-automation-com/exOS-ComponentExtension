
TYPE
	IntStruct_typ : 	STRUCT 
		MyInt13 : UDINT;
		MyInt14 : ARRAY[0..2]OF USINT;
		MyInt133 : UDINT;
		MyInt124 : ARRAY[0..2]OF USINT;
	END_STRUCT;
	IntStruct1_typ : 	STRUCT 
		MyInt13 : UDINT;
	END_STRUCT;
	IntStruct2_typ : 	STRUCT 
		MyInt23 : UDINT;
		MyInt24 : ARRAY[0..3]OF USINT;
		MyInt25 : UDINT;
	END_STRUCT;
	StringAndArray : 	STRUCT 
		MyInt1 : UDINT; (*PUB*)
		MyString : ARRAY[0..2]OF STRING[80]; (*PUB*)
		MyInt2 : ARRAY[0..4]OF USINT; (*PUB SUB*)
		MyIntStruct : ARRAY[0..5]OF IntStruct_typ; (*PUB SUB*)
		MyIntStruct1 : IntStruct1_typ; (*PUB SUB*)
		MyIntStruct2 : IntStruct2_typ; (*PUB SUB*)
		MyEnum1 : Enum_enum; (*PUB SUB*)
	END_STRUCT;
	Enum_enum : 
		(
		enum1,
		enum2
		);
END_TYPE
