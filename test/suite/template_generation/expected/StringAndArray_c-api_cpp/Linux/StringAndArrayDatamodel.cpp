#define EXOS_STATIC_INCLUDE
#include "StringAndArrayDatamodel.hpp"

StringAndArrayDatamodel::StringAndArrayDatamodel()
    : log("gStringAndArray_0")
{
    log.success << "starting gStringAndArray_0 application.." << std::endl;

    exos_assert_ok((&log), exos_datamodel_init(&datamodel, "StringAndArray_0", "gStringAndArray_0"));
    datamodel.user_context = this;

    MyInt1.init(&datamodel, "MyInt1", &log);
    MyString.init(&datamodel, "MyString", &log);
    MyInt2.init(&datamodel, "MyInt2", &log);
    MyIntStruct.init(&datamodel, "MyIntStruct", &log);
    MyIntStruct1.init(&datamodel, "MyIntStruct1", &log);
    MyIntStruct2.init(&datamodel, "MyIntStruct2", &log);
    MyEnum1.init(&datamodel, "MyEnum1", &log);
}

void StringAndArrayDatamodel::connect() {
    exos_assert_ok((&log), exos_datamodel_connect_stringandarray(&datamodel, &StringAndArrayDatamodel::_datamodelEvent));

    MyInt1.connect((EXOS_DATASET_TYPE)EXOS_DATASET_SUBSCRIBE);
    MyString.connect((EXOS_DATASET_TYPE)EXOS_DATASET_SUBSCRIBE);
    MyInt2.connect((EXOS_DATASET_TYPE)(EXOS_DATASET_PUBLISH+EXOS_DATASET_SUBSCRIBE));
    MyIntStruct.connect((EXOS_DATASET_TYPE)(EXOS_DATASET_PUBLISH+EXOS_DATASET_SUBSCRIBE));
    MyIntStruct1.connect((EXOS_DATASET_TYPE)(EXOS_DATASET_PUBLISH+EXOS_DATASET_SUBSCRIBE));
    MyIntStruct2.connect((EXOS_DATASET_TYPE)(EXOS_DATASET_PUBLISH+EXOS_DATASET_SUBSCRIBE));
    MyEnum1.connect((EXOS_DATASET_TYPE)(EXOS_DATASET_PUBLISH+EXOS_DATASET_SUBSCRIBE));
}

void StringAndArrayDatamodel::disconnect() {
    exos_assert_ok((&log), exos_datamodel_disconnect(&datamodel));
}

void StringAndArrayDatamodel::setOperational() {
    exos_assert_ok((&log), exos_datamodel_set_operational(&datamodel));
}

void StringAndArrayDatamodel::process() {
    exos_assert_ok((&log), exos_datamodel_process(&datamodel));
    log.process();
}

int StringAndArrayDatamodel::getNettime() {
    return exos_datamodel_get_nettime(&datamodel);
}

void StringAndArrayDatamodel::datamodelEvent(exos_datamodel_handle_t *datamodel, const EXOS_DATAMODEL_EVENT_TYPE event_type, void *info) {
    switch (event_type)
    {
    case EXOS_DATAMODEL_EVENT_CONNECTION_CHANGED:
        log.info << "application changed state to " << exos_get_state_string(datamodel->connection_state) << std::endl;
        connectionState = datamodel->connection_state;
        _onConnectionChange();
        switch (datamodel->connection_state)
        {
        case EXOS_STATE_DISCONNECTED:
            isOperational = false;
            isConnected = false;
            break;
        case EXOS_STATE_CONNECTED:
            isConnected = true;
            break;
        case EXOS_STATE_OPERATIONAL:
            log.success << "gStringAndArray_0 operational!" << std::endl;
            isOperational = true;
            break;
        case EXOS_STATE_ABORTED:
            log.error << "application error " << datamodel->error << " (" << exos_get_error_string(datamodel->error) << ") occured" << std::endl;
            isOperational = false;
            isConnected = false;
            break;
        }
        break;
    case EXOS_DATAMODEL_EVENT_SYNC_STATE_CHANGED:
        break;

    default:
        break;

    }
}

StringAndArrayDatamodel::~StringAndArrayDatamodel()
{
    exos_assert_ok((&log), exos_datamodel_delete(&datamodel));
}
