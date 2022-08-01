#define EXOS_STATIC_INCLUDE
#include "ros_topics_typDatamodel.hpp"

ros_topics_typDatamodel::ros_topics_typDatamodel()
    : log("gros_topics_typ_0")
{
    log.success << "starting gros_topics_typ_0 application.." << std::endl;

    exos_assert_ok((&log), exos_datamodel_init(&datamodel, "ros_topics_typ_0", "gros_topics_typ_0"));
    datamodel.user_context = this;

    odemetry.init(&datamodel, "odemetry", &log);
    twist.init(&datamodel, "twist", &log);
    config.init(&datamodel, "config", &log);
}

void ros_topics_typDatamodel::connect() {
    exos_assert_ok((&log), exos_datamodel_connect_ros_topics_typ(&datamodel, &ros_topics_typDatamodel::_datamodelEvent));

    odemetry.connect((EXOS_DATASET_TYPE)EXOS_DATASET_SUBSCRIBE);
    twist.connect((EXOS_DATASET_TYPE)EXOS_DATASET_PUBLISH);
    config.connect((EXOS_DATASET_TYPE)EXOS_DATASET_PUBLISH);
}

void ros_topics_typDatamodel::disconnect() {
    exos_assert_ok((&log), exos_datamodel_disconnect(&datamodel));
}

void ros_topics_typDatamodel::setOperational() {
    exos_assert_ok((&log), exos_datamodel_set_operational(&datamodel));
}

void ros_topics_typDatamodel::process() {
    exos_assert_ok((&log), exos_datamodel_process(&datamodel));
    log.process();
}

int ros_topics_typDatamodel::getNettime() {
    return exos_datamodel_get_nettime(&datamodel);
}

void ros_topics_typDatamodel::datamodelEvent(exos_datamodel_handle_t *datamodel, const EXOS_DATAMODEL_EVENT_TYPE event_type, void *info) {
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
            log.success << "gros_topics_typ_0 operational!" << std::endl;
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

ros_topics_typDatamodel::~ros_topics_typDatamodel()
{
    exos_assert_ok((&log), exos_datamodel_delete(&datamodel));
}
