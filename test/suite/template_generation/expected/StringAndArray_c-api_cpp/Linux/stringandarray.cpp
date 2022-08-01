#include <string>
#include <csignal>
#include "StringAndArrayDatamodel.hpp"
#include "termination.h"

/* datamodel features:

main methods:
    stringandarray.connect()
    stringandarray.disconnect()
    stringandarray.process()
    stringandarray.setOperational()
    stringandarray.dispose()
    stringandarray.getNettime() : (int32_t) get current nettime

void(void) user lambda callback:
    stringandarray.onConnectionChange([&] () {
        // stringandarray.connectionState ...
    })

boolean values:
    stringandarray.isConnected
    stringandarray.isOperational

logging methods:
    stringandarray.log.error << "some value:" << 1 << std::endl;
    stringandarray.log.warning << "some value:" << 1 << std::endl;
    stringandarray.log.success << "some value:" << 1 << std::endl;
    stringandarray.log.info << "some value:" << 1 << std::endl;
    stringandarray.log.debug << "some value:" << 1 << std::endl;
    stringandarray.log.verbose << "some value:" << 1 << std::endl;

dataset MyInt1:
    stringandarray.MyInt1.onChange([&] () {
        stringandarray.MyInt1.value ...
    })
    stringandarray.MyInt1.nettime : (int32_t) nettime @ time of publish
    stringandarray.MyInt1.value : (uint32_t)  actual dataset value

dataset MyString:
    stringandarray.MyString.onChange([&] () {
        stringandarray.MyString.value ...
    })
    stringandarray.MyString.nettime : (int32_t) nettime @ time of publish
    stringandarray.MyString.value : (char[3][81])  actual dataset value

dataset MyInt2:
    stringandarray.MyInt2.publish()
    stringandarray.MyInt2.onChange([&] () {
        stringandarray.MyInt2.value ...
    })
    stringandarray.MyInt2.nettime : (int32_t) nettime @ time of publish
    stringandarray.MyInt2.value : (uint8_t[5])  actual dataset value

dataset MyIntStruct:
    stringandarray.MyIntStruct.publish()
    stringandarray.MyIntStruct.onChange([&] () {
        stringandarray.MyIntStruct.value ...
    })
    stringandarray.MyIntStruct.nettime : (int32_t) nettime @ time of publish
    stringandarray.MyIntStruct.value : (IntStruct_typ[6])  actual dataset values

dataset MyIntStruct1:
    stringandarray.MyIntStruct1.publish()
    stringandarray.MyIntStruct1.onChange([&] () {
        stringandarray.MyIntStruct1.value ...
    })
    stringandarray.MyIntStruct1.nettime : (int32_t) nettime @ time of publish
    stringandarray.MyIntStruct1.value : (IntStruct1_typ)  actual dataset values

dataset MyIntStruct2:
    stringandarray.MyIntStruct2.publish()
    stringandarray.MyIntStruct2.onChange([&] () {
        stringandarray.MyIntStruct2.value ...
    })
    stringandarray.MyIntStruct2.nettime : (int32_t) nettime @ time of publish
    stringandarray.MyIntStruct2.value : (IntStruct2_typ)  actual dataset values

dataset MyEnum1:
    stringandarray.MyEnum1.publish()
    stringandarray.MyEnum1.onChange([&] () {
        stringandarray.MyEnum1.value ...
    })
    stringandarray.MyEnum1.nettime : (int32_t) nettime @ time of publish
    stringandarray.MyEnum1.value : (Enum_enum)  actual dataset value
*/



int main(int argc, char ** argv)
{
    catch_termination();
    
    StringAndArrayDatamodel stringandarray;
    stringandarray.connect();
    
    stringandarray.onConnectionChange([&] () {
        if (stringandarray.connectionState == EXOS_STATE_CONNECTED) {
            // Datamodel connected
        }
        else if (stringandarray.connectionState == EXOS_STATE_DISCONNECTED) {    
            // Datamodel disconnected
        }
    });

    stringandarray.MyInt1.onChange([&] () {
        // stringandarray.MyInt1.value ...
    });

    stringandarray.MyString.onChange([&] () {
        // stringandarray.MyString.value ...
    });

    stringandarray.MyInt2.onChange([&] () {
        // stringandarray.MyInt2.value ...
    });

    stringandarray.MyIntStruct.onChange([&] () {
        // stringandarray.MyIntStruct.value ...
    });

    stringandarray.MyIntStruct1.onChange([&] () {
        // stringandarray.MyIntStruct1.value ...
    });

    stringandarray.MyIntStruct2.onChange([&] () {
        // stringandarray.MyIntStruct2.value ...
    });

    stringandarray.MyEnum1.onChange([&] () {
        // stringandarray.MyEnum1.value ...
    });


    while(!is_terminated()) {
        // trigger callbacks
        stringandarray.process();
        
        // publish datasets
        
        if (stringandarray.isConnected) {
            // stringandarray.MyInt2.value = ...
            // stringandarray.MyInt2.publish();
            
            // stringandarray.MyIntStruct.value = ...
            // stringandarray.MyIntStruct.publish();
            
            // stringandarray.MyIntStruct1.value = ...
            // stringandarray.MyIntStruct1.publish();
            
            // stringandarray.MyIntStruct2.value = ...
            // stringandarray.MyIntStruct2.publish();
            
            // stringandarray.MyEnum1.value = ...
            // stringandarray.MyEnum1.publish();
            
        }
    }

    return 0;
}
