#include <string.h>
#define EXOS_ASSERT_LOG &logger
#include "exos_log.h"
#include "libnodejs.h"

#define SUCCESS(_format_, ...) exos_log_success(&logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define INFO(_format_, ...) exos_log_info(&logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define VERBOSE(_format_, ...) exos_log_debug(&logger, EXOS_LOG_TYPE_USER + EXOS_LOG_TYPE_VERBOSE, _format_, ##__VA_ARGS__);
#define ERROR(_format_, ...) exos_log_error(&logger, _format_, ##__VA_ARGS__);

static exos_log_handle_t logger;

typedef struct libNodeJSHandle
{
    libNodeJS_t ext_nodejs;
    exos_datamodel_handle_t nodejs;

    exos_dataset_handle_t start_dataset;
    exos_dataset_handle_t reset_dataset;
    exos_dataset_handle_t countup;
    exos_dataset_handle_t countdown;
} libNodeJSHandle_t;

static libNodeJSHandle_t h_NodeJS;

static void libNodeJS_datasetEvent(exos_dataset_handle_t *dataset, EXOS_DATASET_EVENT_TYPE event_type, void *info)
{
    switch (event_type)
    {
    case EXOS_DATASET_EVENT_UPDATED:
        VERBOSE("dataset %s updated! latency (us):%i", dataset->name, (exos_datamodel_get_nettime(dataset->datamodel,NULL) - dataset->nettime));
        //handle each subscription dataset separately
        if (0 == strcmp(dataset->name,"start"))
        {
            //trigger the callback if assigned
            if (NULL != h_NodeJS.ext_nodejs.start.on_change)
            {
                h_NodeJS.ext_nodejs.start.on_change();
            }
        }
        else if (0 == strcmp(dataset->name,"reset"))
        {
            //trigger the callback if assigned
            if (NULL != h_NodeJS.ext_nodejs.reset.on_change)
            {
                h_NodeJS.ext_nodejs.reset.on_change();
            }
        }
        break;

    default:
        break;

    }
}

static void libNodeJS_datamodelEvent(exos_datamodel_handle_t *datamodel, const EXOS_DATAMODEL_EVENT_TYPE event_type, void *info)
{
    switch (event_type)
    {
    case EXOS_DATAMODEL_EVENT_CONNECTION_CHANGED:
        INFO("application changed state to %s", exos_get_state_string(datamodel->connection_state));

        h_NodeJS.ext_nodejs.is_connected = false;
        h_NodeJS.ext_nodejs.is_operational = false;
        switch (datamodel->connection_state)
        {
        case EXOS_STATE_DISCONNECTED:
            if (NULL != h_NodeJS.ext_nodejs.on_disconnected)
            {
                h_NodeJS.ext_nodejs.on_disconnected();
            }
            break;
        case EXOS_STATE_CONNECTED:
            h_NodeJS.ext_nodejs.is_connected = true;
            if (NULL != h_NodeJS.ext_nodejs.on_connected)
            {
                h_NodeJS.ext_nodejs.on_connected();
            }
            break;
        case EXOS_STATE_OPERATIONAL:
            h_NodeJS.ext_nodejs.is_connected = true;
            h_NodeJS.ext_nodejs.is_operational = true;
            if (NULL != h_NodeJS.ext_nodejs.on_operational)
            {
                h_NodeJS.ext_nodejs.on_operational();
            }
            SUCCESS("NodeJS operational!");
            break;
        case EXOS_STATE_ABORTED:
            if (NULL != h_NodeJS.ext_nodejs.on_disconnected)
            {
                h_NodeJS.ext_nodejs.on_disconnected();
            }
            ERROR("application error %d (%s) occured", datamodel->error, exos_get_error_string(datamodel->error));
            break;
        }
        break;
    }
}

static void libNodeJS_publish_countup(void)
{
    exos_dataset_publish(&h_NodeJS.countup);
}
static void libNodeJS_publish_countdown(void)
{
    exos_dataset_publish(&h_NodeJS.countdown);
}

static void libNodeJS_connect(void)
{
    //connect the datamodel
    EXOS_ASSERT_OK(exos_datamodel_connect_nodejs(&(h_NodeJS.nodejs), libNodeJS_datamodelEvent));
    
    //connect datasets
    EXOS_ASSERT_OK(exos_dataset_connect(&(h_NodeJS.start_dataset), EXOS_DATASET_SUBSCRIBE, libNodeJS_datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&(h_NodeJS.reset_dataset), EXOS_DATASET_SUBSCRIBE, libNodeJS_datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&(h_NodeJS.countup), EXOS_DATASET_PUBLISH, libNodeJS_datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&(h_NodeJS.countdown), EXOS_DATASET_PUBLISH, libNodeJS_datasetEvent));
}
static void libNodeJS_disconnect(void)
{
    EXOS_ASSERT_OK(exos_datamodel_disconnect(&(h_NodeJS.nodejs)));
}

static void libNodeJS_set_operational(void)
{
    EXOS_ASSERT_OK(exos_datamodel_set_operational(&(h_NodeJS.nodejs)));
}

static void libNodeJS_process(void)
{
    EXOS_ASSERT_OK(exos_datamodel_process(&(h_NodeJS.nodejs)));
    exos_log_process(&logger);

}

static void libNodeJS_dispose(void)
{
    EXOS_ASSERT_OK(exos_datamodel_delete(&(h_NodeJS.nodejs)));
    exos_log_delete(&logger);
}

libNodeJS_t *libNodeJS_init(void)
{
    memset(&h_NodeJS,0,sizeof(h_NodeJS));

    h_NodeJS.ext_nodejs.countUp.publish = libNodeJS_publish_countup;
    h_NodeJS.ext_nodejs.countDown.publish = libNodeJS_publish_countdown;
    
    h_NodeJS.ext_nodejs.connect = libNodeJS_connect;
    h_NodeJS.ext_nodejs.disconnect = libNodeJS_disconnect;
    h_NodeJS.ext_nodejs.process = libNodeJS_process;
    h_NodeJS.ext_nodejs.set_operational = libNodeJS_set_operational;
    h_NodeJS.ext_nodejs.dispose = libNodeJS_dispose;
    
    exos_log_init(&logger, "NodeJS_Linux");

    SUCCESS("starting NodeJS_Linux application..");

    EXOS_ASSERT_OK(exos_datamodel_init(&h_NodeJS.nodejs, "NodeJS", "NodeJS_Linux"));

    //set the user_context to access custom data in the callbacks
    h_NodeJS.nodejs.user_context = NULL; //not used
    h_NodeJS.nodejs.user_tag = 0; //not used

    EXOS_ASSERT_OK(exos_dataset_init(&h_NodeJS.start_dataset, &h_NodeJS.nodejs, "start", &h_NodeJS.ext_nodejs.start.value, sizeof(h_NodeJS.ext_nodejs.start.value)));
    h_NodeJS.start_dataset.user_context = NULL; //not used
    h_NodeJS.start_dataset.user_tag = 0; //not used

    EXOS_ASSERT_OK(exos_dataset_init(&h_NodeJS.reset_dataset, &h_NodeJS.nodejs, "reset", &h_NodeJS.ext_nodejs.reset.value, sizeof(h_NodeJS.ext_nodejs.reset.value)));
    h_NodeJS.reset_dataset.user_context = NULL; //not used
    h_NodeJS.reset_dataset.user_tag = 0; //not used

    EXOS_ASSERT_OK(exos_dataset_init(&h_NodeJS.countup, &h_NodeJS.nodejs, "countUp", &h_NodeJS.ext_nodejs.countUp.value, sizeof(h_NodeJS.ext_nodejs.countUp.value)));
    h_NodeJS.countup.user_context = NULL; //not used
    h_NodeJS.countup.user_tag = 0; //not used

    EXOS_ASSERT_OK(exos_dataset_init(&h_NodeJS.countdown, &h_NodeJS.nodejs, "countDown", &h_NodeJS.ext_nodejs.countDown.value, sizeof(h_NodeJS.ext_nodejs.countDown.value)));
    h_NodeJS.countdown.user_context = NULL; //not used
    h_NodeJS.countdown.user_tag = 0; //not used

    return &(h_NodeJS.ext_nodejs);
}

