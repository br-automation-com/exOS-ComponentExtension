#ifndef _STRINGANDARRAYDATAMODEL_H_
#define _STRINGANDARRAYDATAMODEL_H_

#include <string>
#include <iostream>
#include <string.h>
#include <functional>
#include "StringAndArrayDataset.hpp"

class StringAndArrayDatamodel
{
private:
    exos_datamodel_handle_t datamodel = {};
    std::function<void()> _onConnectionChange = [](){};

    void datamodelEvent(exos_datamodel_handle_t *datamodel, const EXOS_DATAMODEL_EVENT_TYPE event_type, void *info);
    static void _datamodelEvent(exos_datamodel_handle_t *datamodel, const EXOS_DATAMODEL_EVENT_TYPE event_type, void *info) {
        StringAndArrayDatamodel* inst = static_cast<StringAndArrayDatamodel*>(datamodel->user_context);
        inst->datamodelEvent(datamodel, event_type, info);
    }

public:
    StringAndArrayDatamodel();
    void process();
    void connect();
    void disconnect();
    void setOperational();
    int getNettime();
    void onConnectionChange(std::function<void()> f) {_onConnectionChange = std::move(f);};

    bool isOperational = false;
    bool isConnected = false;
    EXOS_CONNECTION_STATE connectionState = EXOS_STATE_DISCONNECTED;

    StringAndArrayLogger log;

    StringAndArrayDataset<uint32_t> MyInt1;
    StringAndArrayDataset<char[3][81]> MyString;
    StringAndArrayDataset<uint8_t[5]> MyInt2;
    StringAndArrayDataset<IntStruct_typ[6]> MyIntStruct;
    StringAndArrayDataset<IntStruct1_typ> MyIntStruct1;
    StringAndArrayDataset<IntStruct2_typ> MyIntStruct2;
    StringAndArrayDataset<Enum_enum> MyEnum1;

    ~StringAndArrayDatamodel();
};

#endif
