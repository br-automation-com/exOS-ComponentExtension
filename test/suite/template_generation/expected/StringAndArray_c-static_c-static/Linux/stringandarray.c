#include <unistd.h>
#include "libstringandarray.h"
#include "termination.h"
#include <stdio.h>

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
    stringandarray->MyInt1.on_change : void(void) user callback function
    stringandarray->MyInt1.nettime : (int32_t) nettime @ time of publish
    stringandarray->MyInt1.value : (uint32_t)  actual dataset value

dataset MyString:
    stringandarray->MyString.on_change : void(void) user callback function
    stringandarray->MyString.nettime : (int32_t) nettime @ time of publish
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

static void on_connected_stringandarray(void)
{
   stringandarray->log.success("stringandarray connected!");
}

static void on_change_myint1(void)
{
   stringandarray->log.verbose("stringandarray->MyInt1 changed!");
   // printf("on_change: stringandarray->MyInt1: %u\n", stringandarray->MyInt1.value);

   // Your code here...
}
static void on_change_mystring(void)
{
   stringandarray->log.verbose("stringandarray->MyString changed!");
   // uint32_t i;
   // printf("on_change: stringandarray->MyString: Array of char[]:\n");
   // for(i = 0; i < sizeof(stringandarray->MyString.value) / sizeof(stringandarray->MyString.value[0]); i++ )
   // {
   //     printf("  Index %i: %s\n", i, stringandarray->MyString.value[i]);
   // }

   // Your code here...
}
static void on_change_myint2(void)
{
   stringandarray->log.verbose("stringandarray->MyInt2 changed!");
   // uint32_t i;
   // printf("on_change: stringandarray->MyInt2: Array of uint8_t:\n");
   // for(i = 0; i < sizeof(stringandarray->MyInt2.value) / sizeof(stringandarray->MyInt2.value[0]); i++ )
   // {
   //     printf("  Index %i: %u\n", i, stringandarray->MyInt2.value[i]);
   // }

   // Your code here...
}
static void on_change_myintstruct(void)
{
   stringandarray->log.verbose("stringandarray->MyIntStruct changed!");
   // uint32_t i;
   // printf("on_change: stringandarray->MyIntStruct: Array of IntStruct_typ:\n");
   // for(i = 0; i < sizeof(stringandarray->MyIntStruct.value) / sizeof(stringandarray->MyIntStruct.value[0]); i++ )
   // {
   //     printf("  Index %i: 0x%.8x\n", i, &stringandarray->MyIntStruct.value[i]);
   // }

   // Your code here...
}
static void on_change_myintstruct1(void)
{
   stringandarray->log.verbose("stringandarray->MyIntStruct1 changed!");
   // printf("on_change: stringandarray->MyIntStruct1: 0x%.8x\n", stringandarray->MyIntStruct1.value);

   // Your code here...
}
static void on_change_myintstruct2(void)
{
   stringandarray->log.verbose("stringandarray->MyIntStruct2 changed!");
   // printf("on_change: stringandarray->MyIntStruct2: 0x%.8x\n", stringandarray->MyIntStruct2.value);

   // Your code here...
}
static void on_change_myenum1(void)
{
   stringandarray->log.verbose("stringandarray->MyEnum1 changed!");
   // printf("on_change: stringandarray->MyEnum1: %i\n", stringandarray->MyEnum1.value);

   // Your code here...
}

int main()
{
    //retrieve the stringandarray structure
    stringandarray = libStringAndArray_init();

    //setup callbacks
    stringandarray->on_connected = on_connected_stringandarray;
    // stringandarray->on_disconnected = .. ;
    // stringandarray->on_operational = .. ;
    stringandarray->MyInt1.on_change = on_change_myint1;
    stringandarray->MyString.on_change = on_change_mystring;
    stringandarray->MyInt2.on_change = on_change_myint2;
    stringandarray->MyIntStruct.on_change = on_change_myintstruct;
    stringandarray->MyIntStruct1.on_change = on_change_myintstruct1;
    stringandarray->MyIntStruct2.on_change = on_change_myintstruct2;
    stringandarray->MyEnum1.on_change = on_change_myenum1;

    //connect to the server
    stringandarray->connect();

    catch_termination();
    while (!is_terminated())
    {
        //trigger callbacks and synchronize with AR
        stringandarray->process();

        // if (stringandarray->is_connected)
        // {
        //     stringandarray->MyInt2.value[..] = .. ;
        //     stringandarray->MyInt2.publish();
        
        //     stringandarray->MyIntStruct.value[..]. .. = .. ;
        //     stringandarray->MyIntStruct.publish();
        
        //     stringandarray->MyIntStruct1.value. .. = .. ;
        //     stringandarray->MyIntStruct1.publish();
        
        //     stringandarray->MyIntStruct2.value. .. = .. ;
        //     stringandarray->MyIntStruct2.publish();
        
        //     stringandarray->MyEnum1.value = .. ;
        //     stringandarray->MyEnum1.publish();
        
        // }
    }

    //shutdown
    stringandarray->disconnect();
    stringandarray->dispose();

    return 0;
}
