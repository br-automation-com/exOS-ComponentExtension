/*Automatically generated c file from StringAndArray.typ*/

#include "exos_stringandarray.h"

const char config_stringandarray[] = "{\"name\":\"struct\",\"attributes\":{\"name\":\"<NAME>\",\"dataType\":\"StringAndArray\",\"info\":\"<infoId0>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"MyInt1\",\"dataType\":\"UDINT\",\"comment\":\"PUB\",\"info\":\"<infoId1>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"MyString\",\"dataType\":\"STRING\",\"stringLength\":81,\"comment\":\"PUB\",\"arraySize\":3,\"info\":\"<infoId2>\",\"info2\":\"<infoId3>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"MyInt2\",\"dataType\":\"USINT\",\"comment\":\"PUB SUB\",\"arraySize\":5,\"info\":\"<infoId4>\",\"info2\":\"<infoId5>\"}},{\"name\":\"struct\",\"attributes\":{\"name\":\"MyIntStruct\",\"dataType\":\"IntStruct_typ\",\"comment\":\"PUB SUB\",\"arraySize\":6,\"info\":\"<infoId6>\",\"info2\":\"<infoId7>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"MyInt13\",\"dataType\":\"UDINT\",\"info\":\"<infoId8>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"MyInt14\",\"dataType\":\"USINT\",\"arraySize\":3,\"info\":\"<infoId9>\",\"info2\":\"<infoId10>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"MyInt133\",\"dataType\":\"UDINT\",\"info\":\"<infoId11>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"MyInt124\",\"dataType\":\"USINT\",\"arraySize\":3,\"info\":\"<infoId12>\",\"info2\":\"<infoId13>\"}}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"MyIntStruct1\",\"dataType\":\"IntStruct1_typ\",\"comment\":\"PUB SUB\",\"info\":\"<infoId14>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"MyInt13\",\"dataType\":\"UDINT\",\"info\":\"<infoId15>\"}}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"MyIntStruct2\",\"dataType\":\"IntStruct2_typ\",\"comment\":\"PUB SUB\",\"info\":\"<infoId16>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"MyInt23\",\"dataType\":\"UDINT\",\"info\":\"<infoId17>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"MyInt24\",\"dataType\":\"USINT\",\"arraySize\":4,\"info\":\"<infoId18>\",\"info2\":\"<infoId19>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"MyInt25\",\"dataType\":\"UDINT\",\"info\":\"<infoId20>\"}}]},{\"name\":\"enum\",\"attributes\":{\"name\":\"MyEnum1\",\"dataType\":\"Enum_enum\",\"comment\":\"PUB SUB\",\"info\":\"<infoId21>\"},\"children\":[{\"name\":\"value\",\"attributes\":{\"name\":\"enum1\",\"value\":0}},{\"name\":\"value\",\"attributes\":{\"name\":\"enum2\",\"value\":1}}]}]}";

/*Connect the StringAndArray datamodel to the server*/
EXOS_ERROR_CODE exos_datamodel_connect_stringandarray(exos_datamodel_handle_t *datamodel, exos_datamodel_event_cb datamodel_event_callback)
{
    StringAndArray data;
    exos_dataset_info_t datasets[] = {
        {EXOS_DATASET_BROWSE_NAME_INIT,{}},
        {EXOS_DATASET_BROWSE_NAME(MyInt1),{}},
        {EXOS_DATASET_BROWSE_NAME(MyString),{}},
        {EXOS_DATASET_BROWSE_NAME(MyString[0]),{3}},
        {EXOS_DATASET_BROWSE_NAME(MyInt2),{}},
        {EXOS_DATASET_BROWSE_NAME(MyInt2[0]),{5}},
        {EXOS_DATASET_BROWSE_NAME(MyIntStruct),{}},
        {EXOS_DATASET_BROWSE_NAME(MyIntStruct[0]),{6}},
        {EXOS_DATASET_BROWSE_NAME(MyIntStruct[0].MyInt13),{6}},
        {EXOS_DATASET_BROWSE_NAME(MyIntStruct[0].MyInt14),{6}},
        {EXOS_DATASET_BROWSE_NAME(MyIntStruct[0].MyInt14[0]),{6,3}},
        {EXOS_DATASET_BROWSE_NAME(MyIntStruct[0].MyInt133),{6}},
        {EXOS_DATASET_BROWSE_NAME(MyIntStruct[0].MyInt124),{6}},
        {EXOS_DATASET_BROWSE_NAME(MyIntStruct[0].MyInt124[0]),{6,3}},
        {EXOS_DATASET_BROWSE_NAME(MyIntStruct1),{}},
        {EXOS_DATASET_BROWSE_NAME(MyIntStruct1.MyInt13),{}},
        {EXOS_DATASET_BROWSE_NAME(MyIntStruct2),{}},
        {EXOS_DATASET_BROWSE_NAME(MyIntStruct2.MyInt23),{}},
        {EXOS_DATASET_BROWSE_NAME(MyIntStruct2.MyInt24),{}},
        {EXOS_DATASET_BROWSE_NAME(MyIntStruct2.MyInt24[0]),{4}},
        {EXOS_DATASET_BROWSE_NAME(MyIntStruct2.MyInt25),{}},
        {EXOS_DATASET_BROWSE_NAME(MyEnum1),{}}
    };

    exos_datamodel_calc_dataset_info(datasets, sizeof(datasets));

    return exos_datamodel_connect(datamodel, config_stringandarray, datasets, sizeof(datasets), datamodel_event_callback);
}
