#include <string.h>
#define EXOS_ASSERT_LOG &logger
#include "exos_log.h"
#include "libbufferfub.h"

#define SUCCESS(_format_, ...) exos_log_success(&logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define INFO(_format_, ...) exos_log_info(&logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define VERBOSE(_format_, ...) exos_log_debug(&logger, EXOS_LOG_TYPE_USER + EXOS_LOG_TYPE_VERBOSE, _format_, ##__VA_ARGS__);
#define ERROR(_format_, ...) exos_log_error(&logger, _format_, ##__VA_ARGS__);

static exos_log_handle_t logger;

typedef struct libBufferFubHandle
{
    libBufferFub_t ext_bufferfub;
    exos_datamodel_handle_t bufferfub;

    exos_dataset_handle_t bufferedsample;
    exos_dataset_handle_t setup_dataset;
    exos_dataset_handle_t cmdsendburst;
} libBufferFubHandle_t;

static libBufferFubHandle_t h_BufferFub;

static void libBufferFub_datasetEvent(exos_dataset_handle_t *dataset, EXOS_DATASET_EVENT_TYPE event_type, void *info)
{
    switch (event_type)
    {
    case EXOS_DATASET_EVENT_UPDATED:
        VERBOSE("dataset %s updated! latency (us):%i", dataset->name, (exos_datamodel_get_nettime(dataset->datamodel) - dataset->nettime));
        //handle each subscription dataset separately
        if (0 == strcmp(dataset->name, "bufferedSample"))
        {
            //update the nettime
            h_BufferFub.ext_bufferfub.bufferedSample.nettime = dataset->nettime;

            //trigger the callback if assigned
            if (NULL != h_BufferFub.ext_bufferfub.bufferedSample.on_change)
            {
                h_BufferFub.ext_bufferfub.bufferedSample.on_change();
            }
        }
        break;

    case EXOS_DATASET_EVENT_PUBLISHED:
        break;
    case EXOS_DATASET_EVENT_DELIVERED:
        break;
    case EXOS_DATASET_EVENT_CONNECTION_CHANGED:
        INFO("dataset %s changed state to %s", dataset->name, exos_get_state_string(dataset->connection_state));

        switch (dataset->connection_state)
        {
        case EXOS_STATE_DISCONNECTED:
            break;
        case EXOS_STATE_CONNECTED:
            break;
        case EXOS_STATE_OPERATIONAL:
            break;
        case EXOS_STATE_ABORTED:
            ERROR("dataset %s error %d (%s) occured", dataset->name, dataset->error, exos_get_error_string(dataset->error));
            break;
        }
        break;
    }
}

static void libBufferFub_datamodelEvent(exos_datamodel_handle_t *datamodel, const EXOS_DATAMODEL_EVENT_TYPE event_type, void *info)
{
    switch (event_type)
    {
    case EXOS_DATAMODEL_EVENT_CONNECTION_CHANGED:
        INFO("application changed state to %s", exos_get_state_string(datamodel->connection_state));

        h_BufferFub.ext_bufferfub.is_connected = false;
        h_BufferFub.ext_bufferfub.is_operational = false;
        switch (datamodel->connection_state)
        {
        case EXOS_STATE_DISCONNECTED:
            if (NULL != h_BufferFub.ext_bufferfub.on_disconnected)
            {
                h_BufferFub.ext_bufferfub.on_disconnected();
            }
            break;
        case EXOS_STATE_CONNECTED:
            h_BufferFub.ext_bufferfub.is_connected = true;
            if (NULL != h_BufferFub.ext_bufferfub.on_connected)
            {
                h_BufferFub.ext_bufferfub.on_connected();
            }
            break;
        case EXOS_STATE_OPERATIONAL:
            h_BufferFub.ext_bufferfub.is_connected = true;
            h_BufferFub.ext_bufferfub.is_operational = true;
            if (NULL != h_BufferFub.ext_bufferfub.on_operational)
            {
                h_BufferFub.ext_bufferfub.on_operational();
            }
            SUCCESS("BufferFub operational!");
            break;
        case EXOS_STATE_ABORTED:
            if (NULL != h_BufferFub.ext_bufferfub.on_disconnected)
            {
                h_BufferFub.ext_bufferfub.on_disconnected();
            }
            ERROR("application error %d (%s) occured", datamodel->error, exos_get_error_string(datamodel->error));
            break;
        }
        break;
    case EXOS_DATAMODEL_EVENT_SYNC_STATE_CHANGED:
        break;

    default:
        break;

    }
}

static void libBufferFub_publish_setup_dataset(void)
{
    EXOS_ASSERT_OK(exos_dataset_publish(&h_BufferFub.setup_dataset));
}
static void libBufferFub_publish_cmdsendburst(void)
{
    EXOS_ASSERT_OK(exos_dataset_publish(&h_BufferFub.cmdsendburst));
}

static void libBufferFub_connect(void)
{
    //connect the datamodel
    EXOS_ASSERT_OK(exos_datamodel_connect_bufferfub(&(h_BufferFub.bufferfub), libBufferFub_datamodelEvent));
    
    //connect datasets
    EXOS_ASSERT_OK(exos_dataset_connect(&(h_BufferFub.bufferedsample), EXOS_DATASET_SUBSCRIBE, libBufferFub_datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&(h_BufferFub.setup_dataset), EXOS_DATASET_PUBLISH, libBufferFub_datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&(h_BufferFub.cmdsendburst), EXOS_DATASET_PUBLISH, libBufferFub_datasetEvent));
}
static void libBufferFub_disconnect(void)
{
    h_BufferFub.ext_bufferfub.is_connected = false;
    h_BufferFub.ext_bufferfub.is_operational = false;

    EXOS_ASSERT_OK(exos_datamodel_disconnect(&(h_BufferFub.bufferfub)));
}

static void libBufferFub_set_operational(void)
{
    EXOS_ASSERT_OK(exos_datamodel_set_operational(&(h_BufferFub.bufferfub)));
}

static void libBufferFub_process(void)
{
    EXOS_ASSERT_OK(exos_datamodel_process(&(h_BufferFub.bufferfub)));
    exos_log_process(&logger);
}

static void libBufferFub_dispose(void)
{
    h_BufferFub.ext_bufferfub.is_connected = false;
    h_BufferFub.ext_bufferfub.is_operational = false;

    EXOS_ASSERT_OK(exos_datamodel_delete(&(h_BufferFub.bufferfub)));
    exos_log_delete(&logger);
}

static int32_t libBufferFub_get_nettime(void)
{
    return exos_datamodel_get_nettime(&(h_BufferFub.bufferfub));
}

static void libBufferFub_log_error(char* log_entry)
{
    exos_log_error(&logger, log_entry);
}

static void libBufferFub_log_warning(char* log_entry)
{
    exos_log_warning(&logger, EXOS_LOG_TYPE_USER, log_entry);
}

static void libBufferFub_log_success(char* log_entry)
{
    exos_log_success(&logger, EXOS_LOG_TYPE_USER, log_entry);
}

static void libBufferFub_log_info(char* log_entry)
{
    exos_log_info(&logger, EXOS_LOG_TYPE_USER, log_entry);
}

static void libBufferFub_log_debug(char* log_entry)
{
    exos_log_debug(&logger, EXOS_LOG_TYPE_USER, log_entry);
}

static void libBufferFub_log_verbose(char* log_entry)
{
    exos_log_warning(&logger, EXOS_LOG_TYPE_USER + EXOS_LOG_TYPE_VERBOSE, log_entry);
}

libBufferFub_t *libBufferFub_init(void)
{
    memset(&h_BufferFub, 0, sizeof(h_BufferFub));

    h_BufferFub.ext_bufferfub.setup.publish = libBufferFub_publish_setup_dataset;
    h_BufferFub.ext_bufferfub.cmdSendBurst.publish = libBufferFub_publish_cmdsendburst;
    
    h_BufferFub.ext_bufferfub.connect = libBufferFub_connect;
    h_BufferFub.ext_bufferfub.disconnect = libBufferFub_disconnect;
    h_BufferFub.ext_bufferfub.process = libBufferFub_process;
    h_BufferFub.ext_bufferfub.set_operational = libBufferFub_set_operational;
    h_BufferFub.ext_bufferfub.dispose = libBufferFub_dispose;
    h_BufferFub.ext_bufferfub.get_nettime = libBufferFub_get_nettime;
    h_BufferFub.ext_bufferfub.log.error = libBufferFub_log_error;
    h_BufferFub.ext_bufferfub.log.warning = libBufferFub_log_warning;
    h_BufferFub.ext_bufferfub.log.success = libBufferFub_log_success;
    h_BufferFub.ext_bufferfub.log.info = libBufferFub_log_info;
    h_BufferFub.ext_bufferfub.log.debug = libBufferFub_log_debug;
    h_BufferFub.ext_bufferfub.log.verbose = libBufferFub_log_verbose;
    
    exos_log_init(&logger, "gBufferFub_0");

    SUCCESS("starting gBufferFub_0 application..");

    EXOS_ASSERT_OK(exos_datamodel_init(&h_BufferFub.bufferfub, "BufferFub_0", "gBufferFub_0"));

    //set the user_context to access custom data in the callbacks
    h_BufferFub.bufferfub.user_context = NULL; //not used
    h_BufferFub.bufferfub.user_tag = 0; //not used

    EXOS_ASSERT_OK(exos_dataset_init(&h_BufferFub.bufferedsample, &h_BufferFub.bufferfub, "bufferedSample", &h_BufferFub.ext_bufferfub.bufferedSample.value, sizeof(h_BufferFub.ext_bufferfub.bufferedSample.value)));
    h_BufferFub.bufferedsample.user_context = NULL; //not used
    h_BufferFub.bufferedsample.user_tag = 0; //not used

    EXOS_ASSERT_OK(exos_dataset_init(&h_BufferFub.setup_dataset, &h_BufferFub.bufferfub, "setup", &h_BufferFub.ext_bufferfub.setup.value, sizeof(h_BufferFub.ext_bufferfub.setup.value)));
    h_BufferFub.setup_dataset.user_context = NULL; //not used
    h_BufferFub.setup_dataset.user_tag = 0; //not used

    EXOS_ASSERT_OK(exos_dataset_init(&h_BufferFub.cmdsendburst, &h_BufferFub.bufferfub, "cmdSendBurst", &h_BufferFub.ext_bufferfub.cmdSendBurst.value, sizeof(h_BufferFub.ext_bufferfub.cmdSendBurst.value)));
    h_BufferFub.cmdsendburst.user_context = NULL; //not used
    h_BufferFub.cmdsendburst.user_tag = 0; //not used

    return &(h_BufferFub.ext_bufferfub);
}
