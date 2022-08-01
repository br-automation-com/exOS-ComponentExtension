/*Automatically generated header file from StringAndArray.typ*/

#ifndef _EXOS_COMP_STRINGANDARRAY_H_
#define _EXOS_COMP_STRINGANDARRAY_H_

#include "exos_api.h"

#if defined(_SG4)
#include <StringAndA.h>
#else
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum Enum_enum
{
    enum1,
    enum2

} Enum_enum;

typedef struct IntStruct2_typ
{
    uint32_t MyInt23;
    uint8_t MyInt24[4];
    uint32_t MyInt25;

} IntStruct2_typ;

typedef struct IntStruct1_typ
{
    uint32_t MyInt13;

} IntStruct1_typ;

typedef struct IntStruct_typ
{
    uint32_t MyInt13;
    uint8_t MyInt14[3];
    uint32_t MyInt133;
    uint8_t MyInt124[3];

} IntStruct_typ;

typedef struct StringAndArray
{
    uint32_t MyInt1; //PUB
    char MyString[3][81]; //PUB
    uint8_t MyInt2[5]; //PUB SUB
    struct IntStruct_typ MyIntStruct[6]; //PUB SUB
    struct IntStruct1_typ MyIntStruct1; //PUB SUB
    struct IntStruct2_typ MyIntStruct2; //PUB SUB
    Enum_enum MyEnum1; //PUB SUB

} StringAndArray;

#endif // _SG4

EXOS_ERROR_CODE exos_datamodel_connect_stringandarray(exos_datamodel_handle_t *datamodel, exos_datamodel_event_cb datamodel_event_callback);

#endif // _EXOS_COMP_STRINGANDARRAY_H_
