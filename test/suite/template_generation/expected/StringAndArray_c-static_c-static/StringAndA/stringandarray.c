#include <string.h>
#include <stdbool.h>
#include "libstringandarray.h"

/* libStringAndArray_t datamodel features:

main methods:
    stringandarray->connect()
    stringandarray->disconnect()
    stringandarray->process()
    stringandarray->set_operational()
    stringandarray->dispose()
    stringandarray->get_nettime() : (int32_t) get current nettime

void(void) user callbacks:
    stringandarray->on_connected
    stringandarray->on_disconnected
    stringandarray->on_operational

boolean values:
    stringandarray->is_connected
    stringandarray->is_operational

logging methods:
    stringandarray->log.error(char *)
    stringandarray->log.warning(char *)
    stringandarray->log.success(char *)
    stringandarray->log.info(char *)
    stringandarray->log.debug(char *)
    stringandarray->log.verbose(char *)

dataset MyInt1:
    stringandarray->MyInt1.publish()
    stringandarray->MyInt1.value : (uint32_t)  actual dataset value

dataset MyString:
    stringandarray->MyString.publish()
    stringandarray->MyString.value : (char[3][81])  actual dataset value

dataset MyInt2:
    stringandarray->MyInt2.publish()
    stringandarray->MyInt2.on_change : void(void) user callback function
    stringandarray->MyInt2.nettime : (int32_t) nettime @ time of publish
    stringandarray->MyInt2.value : (uint8_t[5])  actual dataset value

dataset MyIntStruct:
    stringandarray->MyIntStruct.publish()
    stringandarray->MyIntStruct.on_change : void(void) user callback function
    stringandarray->MyIntStruct.nettime : (int32_t) nettime @ time of publish
    stringandarray->MyIntStruct.value : (IntStruct_typ[6])  actual dataset values

dataset MyIntStruct1:
    stringandarray->MyIntStruct1.publish()
    stringandarray->MyIntStruct1.on_change : void(void) user callback function
    stringandarray->MyIntStruct1.nettime : (int32_t) nettime @ time of publish
    stringandarray->MyIntStruct1.value : (IntStruct1_typ)  actual dataset values

dataset MyIntStruct2:
    stringandarray->MyIntStruct2.publish()
    stringandarray->MyIntStruct2.on_change : void(void) user callback function
    stringandarray->MyIntStruct2.nettime : (int32_t) nettime @ time of publish
    stringandarray->MyIntStruct2.value : (IntStruct2_typ)  actual dataset values

dataset MyEnum1:
    stringandarray->MyEnum1.publish()
    stringandarray->MyEnum1.on_change : void(void) user callback function
    stringandarray->MyEnum1.nettime : (int32_t) nettime @ time of publish
    stringandarray->MyEnum1.value : (Enum_enum)  actual dataset value
*/

static libStringAndArray_t *stringandarray;
static struct StringAndArrayCyclic *cyclic_inst;

static void on_connected_stringandarray(void)
{
}

static void on_change_myint2(void)
{
    memcpy(&(cyclic_inst->pStringAndArray->MyInt2), &(stringandarray->MyInt2.value), sizeof(cyclic_inst->pStringAndArray->MyInt2));
    
    // Your code here...
}
static void on_change_myintstruct(void)
{
    memcpy(&(cyclic_inst->pStringAndArray->MyIntStruct), &(stringandarray->MyIntStruct.value), sizeof(cyclic_inst->pStringAndArray->MyIntStruct));
    
    // Your code here...
}
static void on_change_myintstruct1(void)
{
    memcpy(&(cyclic_inst->pStringAndArray->MyIntStruct1), &(stringandarray->MyIntStruct1.value), sizeof(cyclic_inst->pStringAndArray->MyIntStruct1));
    
    // Your code here...
}
static void on_change_myintstruct2(void)
{
    memcpy(&(cyclic_inst->pStringAndArray->MyIntStruct2), &(stringandarray->MyIntStruct2.value), sizeof(cyclic_inst->pStringAndArray->MyIntStruct2));
    
    // Your code here...
}
static void on_change_myenum1(void)
{
    cyclic_inst->pStringAndArray->MyEnum1 = stringandarray->MyEnum1.value;
    
    // Your code here...
}
_BUR_PUBLIC void StringAndArrayCyclic(struct StringAndArrayCyclic *inst)
{
    // check if function block has been created before
    if(cyclic_inst != NULL)
    {
        // return error if more than one function blocks have been created
        if(inst != cyclic_inst)
        {
            inst->Operational = false;
            inst->Connected = false;
            inst->Error = true;
            return;
        }
    }
    cyclic_inst = inst;
    // initialize library
    if((libStringAndArray_t *)inst->_Handle == NULL || (libStringAndArray_t *)inst->_Handle != stringandarray)
    {
        //retrieve the stringandarray structure
        stringandarray = libStringAndArray_init();

        //setup callbacks
        stringandarray->on_connected = on_connected_stringandarray;
        // stringandarray->on_disconnected = .. ;
        // stringandarray->on_operational = .. ;
        stringandarray->MyInt2.on_change = on_change_myint2;
        stringandarray->MyIntStruct.on_change = on_change_myintstruct;
        stringandarray->MyIntStruct1.on_change = on_change_myintstruct1;
        stringandarray->MyIntStruct2.on_change = on_change_myintstruct2;
        stringandarray->MyEnum1.on_change = on_change_myenum1;

        inst->_Handle = (UDINT)stringandarray;
    }
    // return error if reference to structure is not set on function block
    if(inst->pStringAndArray == NULL)
    {
        inst->Operational = false;
        inst->Connected = false;
        inst->Error = true;
        return;
    }
    if (inst->Enable && !inst->_Enable)
    {
        //connect to the server
        stringandarray->connect();
    }
    if (!inst->Enable && inst->_Enable)
    {
        //disconnect from server
        cyclic_inst = NULL;
        stringandarray->disconnect();
    }
    inst->_Enable = inst->Enable;

    if(inst->Start && !inst->_Start && stringandarray->is_connected)
    {
        stringandarray->set_operational();
        inst->_Start = inst->Start;
    }
    if(!inst->Start)
    {
        inst->_Start = false;
    }

    //trigger callbacks
    stringandarray->process();

    if (stringandarray->is_connected)
    {
        if (stringandarray->MyInt1.value != inst->pStringAndArray->MyInt1)
        {
            stringandarray->MyInt1.value = inst->pStringAndArray->MyInt1;
            stringandarray->MyInt1.publish();
        }
    
        if (memcmp(&(stringandarray->MyString.value), &(inst->pStringAndArray->MyString), sizeof(inst->pStringAndArray->MyString)))
        {
            memcpy(&(stringandarray->MyString.value), &(inst->pStringAndArray->MyString), sizeof(stringandarray->MyString.value));
            stringandarray->MyString.publish();
        }
    
        if (memcmp(&(stringandarray->MyInt2.value), &(inst->pStringAndArray->MyInt2), sizeof(inst->pStringAndArray->MyInt2)))
        {
            memcpy(&(stringandarray->MyInt2.value), &(inst->pStringAndArray->MyInt2), sizeof(stringandarray->MyInt2.value));
            stringandarray->MyInt2.publish();
        }
    
        if (memcmp(&(stringandarray->MyIntStruct.value), &(inst->pStringAndArray->MyIntStruct), sizeof(inst->pStringAndArray->MyIntStruct)))
        {
            memcpy(&(stringandarray->MyIntStruct.value), &(inst->pStringAndArray->MyIntStruct), sizeof(stringandarray->MyIntStruct.value));
            stringandarray->MyIntStruct.publish();
        }
    
        if (memcmp(&(stringandarray->MyIntStruct1.value), &(inst->pStringAndArray->MyIntStruct1), sizeof(inst->pStringAndArray->MyIntStruct1)))
        {
            memcpy(&(stringandarray->MyIntStruct1.value), &(inst->pStringAndArray->MyIntStruct1), sizeof(stringandarray->MyIntStruct1.value));
            stringandarray->MyIntStruct1.publish();
        }
    
        if (memcmp(&(stringandarray->MyIntStruct2.value), &(inst->pStringAndArray->MyIntStruct2), sizeof(inst->pStringAndArray->MyIntStruct2)))
        {
            memcpy(&(stringandarray->MyIntStruct2.value), &(inst->pStringAndArray->MyIntStruct2), sizeof(stringandarray->MyIntStruct2.value));
            stringandarray->MyIntStruct2.publish();
        }
    
        if (stringandarray->MyEnum1.value != inst->pStringAndArray->MyEnum1)
        {
            stringandarray->MyEnum1.value = inst->pStringAndArray->MyEnum1;
            stringandarray->MyEnum1.publish();
        }
    
        // Your code here...
    }
    inst->Connected = stringandarray->is_connected;
    inst->Operational = stringandarray->is_operational;
}

UINT _EXIT ProgramExit(unsigned long phase)
{
    //shutdown
    stringandarray->dispose();
    cyclic_inst = NULL;
    return 0;
}
