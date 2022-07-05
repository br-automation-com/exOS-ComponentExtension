#define EXOS_STATIC_INCLUDE
#include "BufferFubDatamodel.hpp"

BufferFubDatamodel::BufferFubDatamodel()
    : log("gBufferFub_0")
{
    log.success << "starting gBufferFub_0 application.." << std::endl;

    exos_assert_ok((&log), exos_datamodel_init(&datamodel, "BufferFub_0", "gBufferFub_0"));
    datamodel.user_context = this;

    bufferedSample.init(&datamodel, "bufferedSample", &log);
    setup.init(&datamodel, "setup", &log);
    cmdSendBurst.init(&datamodel, "cmdSendBurst", &log);
}

void BufferFubDatamodel::connect() {
    exos_assert_ok((&log), exos_datamodel_connect_bufferfub(&datamodel, &BufferFubDatamodel::_datamodelEvent));

    bufferedSample.connect((EXOS_DATASET_TYPE)EXOS_DATASET_SUBSCRIBE);
    setup.connect((EXOS_DATASET_TYPE)EXOS_DATASET_PUBLISH);
    cmdSendBurst.connect((EXOS_DATASET_TYPE)EXOS_DATASET_PUBLISH);
}

void BufferFubDatamodel::disconnect() {
    exos_assert_ok((&log), exos_datamodel_disconnect(&datamodel));
}

void BufferFubDatamodel::setOperational() {
    exos_assert_ok((&log), exos_datamodel_set_operational(&datamodel));
}

void BufferFubDatamodel::process() {
    exos_assert_ok((&log), exos_datamodel_process(&datamodel));
    log.process();
}

int BufferFubDatamodel::getNettime() {
    return exos_datamodel_get_nettime(&datamodel);
}

void BufferFubDatamodel::datamodelEvent(exos_datamodel_handle_t *datamodel, const EXOS_DATAMODEL_EVENT_TYPE event_type, void *info) {
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
            log.success << "gBufferFub_0 operational!" << std::endl;
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

BufferFubDatamodel::~BufferFubDatamodel()
{
    exos_assert_ok((&log), exos_datamodel_delete(&datamodel));
}
