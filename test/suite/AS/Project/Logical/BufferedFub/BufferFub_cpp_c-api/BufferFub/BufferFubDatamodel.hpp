#ifndef _BUFFERFUBDATAMODEL_H_
#define _BUFFERFUBDATAMODEL_H_

#include <string>
#include <iostream>
#include <string.h>
#include <functional>
#include "BufferFubDataset.hpp"

class BufferFubDatamodel
{
private:
    exos_datamodel_handle_t datamodel = {};
    std::function<void()> _onConnectionChange = [](){};

    void datamodelEvent(exos_datamodel_handle_t *datamodel, const EXOS_DATAMODEL_EVENT_TYPE event_type, void *info);
    static void _datamodelEvent(exos_datamodel_handle_t *datamodel, const EXOS_DATAMODEL_EVENT_TYPE event_type, void *info) {
        BufferFubDatamodel* inst = static_cast<BufferFubDatamodel*>(datamodel->user_context);
        inst->datamodelEvent(datamodel, event_type, info);
    }

public:
    BufferFubDatamodel();
    void process();
    void connect();
    void disconnect();
    void setOperational();
    int getNettime();
    void onConnectionChange(std::function<void()> f) {_onConnectionChange = std::move(f);};

    bool isOperational = false;
    bool isConnected = false;
    EXOS_CONNECTION_STATE connectionState = EXOS_STATE_DISCONNECTED;

    BufferFubLogger log;

    BufferFubDataset<uint32_t> bufferedSample;
    BufferFubDataset<BufferFubSetup_typ> setup;
    BufferFubDataset<bool> cmdSendBurst;

    ~BufferFubDatamodel();
};

#endif
