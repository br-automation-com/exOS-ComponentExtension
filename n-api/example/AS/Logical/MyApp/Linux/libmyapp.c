#include <string.h>
#define EXOS_ASSERT_LOG &logger
#include "exos_log.h"
#include "libmyapp.h"

#define SUCCESS(_format_, ...) exos_log_success(&logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define INFO(_format_, ...) exos_log_info(&logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define VERBOSE(_format_, ...) exos_log_debug(&logger, EXOS_LOG_TYPE_USER + EXOS_LOG_TYPE_VERBOSE, _format_, ##__VA_ARGS__);
#define ERROR(_format_, ...) exos_log_error(&logger, _format_, ##__VA_ARGS__);

static exos_log_handle_t logger;

typedef struct libMyAppHandle
{
    libMyApp_t ext_myapp;
    exos_datamodel_handle_t myapp;

    exos_dataset_handle_t execute_dataset;
    exos_dataset_handle_t done_dataset;
    exos_dataset_handle_t parameters_dataset;
    exos_dataset_handle_t results_dataset;
} libMyAppHandle_t;

static libMyAppHandle_t h_MyApp;

static void libMyApp_datasetEvent(exos_dataset_handle_t *dataset, EXOS_DATASET_EVENT_TYPE event_type, void *info)
{
    switch (event_type)
    {
    case EXOS_DATASET_EVENT_UPDATED:
        VERBOSE("dataset %s updated! latency (us):%i", dataset->name, (exos_datamodel_get_nettime(dataset->datamodel,NULL) - dataset->nettime));
        //handle each subscription dataset separately
        if (0 == strcmp(dataset->name,"execute"))
        {
            //trigger the callback if assigned
            if (NULL != h_MyApp.ext_myapp.execute.on_change)
            {
                h_MyApp.ext_myapp.execute.on_change();
            }
        }
        else if (0 == strcmp(dataset->name,"done"))
        {
            //trigger the callback if assigned
            if (NULL != h_MyApp.ext_myapp.done.on_change)
            {
                h_MyApp.ext_myapp.done.on_change();
            }
        }
        else if (0 == strcmp(dataset->name,"parameters"))
        {
            //trigger the callback if assigned
            if (NULL != h_MyApp.ext_myapp.parameters.on_change)
            {
                h_MyApp.ext_myapp.parameters.on_change();
            }
        }
        break;

    default:
        break;

    }
}

static void libMyApp_datamodelEvent(exos_datamodel_handle_t *datamodel, const EXOS_DATAMODEL_EVENT_TYPE event_type, void *info)
{
    switch (event_type)
    {
    case EXOS_DATAMODEL_EVENT_CONNECTION_CHANGED:
        INFO("application changed state to %s", exos_get_state_string(datamodel->connection_state));

        h_MyApp.ext_myapp.is_connected = false;
        h_MyApp.ext_myapp.is_operational = false;
        switch (datamodel->connection_state)
        {
        case EXOS_STATE_DISCONNECTED:
            if (NULL != h_MyApp.ext_myapp.on_disconnected)
            {
                h_MyApp.ext_myapp.on_disconnected();
            }
            break;
        case EXOS_STATE_CONNECTED:
            h_MyApp.ext_myapp.is_connected = true;
            if (NULL != h_MyApp.ext_myapp.on_connected)
            {
                h_MyApp.ext_myapp.on_connected();
            }
            break;
        case EXOS_STATE_OPERATIONAL:
            h_MyApp.ext_myapp.is_connected = true;
            h_MyApp.ext_myapp.is_operational = true;
            if (NULL != h_MyApp.ext_myapp.on_operational)
            {
                h_MyApp.ext_myapp.on_operational();
            }
            SUCCESS("MyApp operational!");
            break;
        case EXOS_STATE_ABORTED:
            if (NULL != h_MyApp.ext_myapp.on_disconnected)
            {
                h_MyApp.ext_myapp.on_disconnected();
            }
            ERROR("application error %d (%s) occured", datamodel->error, exos_get_error_string(datamodel->error));
            break;
        }
        break;
    }
}

static void libMyApp_publish_done_dataset(void)
{
    exos_dataset_publish(&h_MyApp.done_dataset);
}
static void libMyApp_publish_results_dataset(void)
{
    exos_dataset_publish(&h_MyApp.results_dataset);
}

static void libMyApp_connect(void)
{
    //connect the datamodel
    EXOS_ASSERT_OK(exos_datamodel_connect_myapp(&(h_MyApp.myapp), libMyApp_datamodelEvent));
    
    //connect datasets
    EXOS_ASSERT_OK(exos_dataset_connect(&(h_MyApp.execute_dataset), EXOS_DATASET_SUBSCRIBE, libMyApp_datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&(h_MyApp.done_dataset), EXOS_DATASET_PUBLISH + EXOS_DATASET_SUBSCRIBE, libMyApp_datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&(h_MyApp.parameters_dataset), EXOS_DATASET_SUBSCRIBE, libMyApp_datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&(h_MyApp.results_dataset), EXOS_DATASET_PUBLISH, libMyApp_datasetEvent));
}
static void libMyApp_disconnect(void)
{
    EXOS_ASSERT_OK(exos_datamodel_disconnect(&(h_MyApp.myapp)));
}

static void libMyApp_set_operational(void)
{
    EXOS_ASSERT_OK(exos_datamodel_set_operational(&(h_MyApp.myapp)));
}

static void libMyApp_process(void)
{
    EXOS_ASSERT_OK(exos_datamodel_process(&(h_MyApp.myapp)));
    exos_log_process(&logger);

}

static void libMyApp_dispose(void)
{
    EXOS_ASSERT_OK(exos_datamodel_delete(&(h_MyApp.myapp)));
    exos_log_delete(&logger);
}

libMyApp_t *libMyApp_init(void)
{
    memset(&h_MyApp,0,sizeof(h_MyApp));

    h_MyApp.ext_myapp.done.publish = libMyApp_publish_done_dataset;
    h_MyApp.ext_myapp.results.publish = libMyApp_publish_results_dataset;
    
    h_MyApp.ext_myapp.connect = libMyApp_connect;
    h_MyApp.ext_myapp.disconnect = libMyApp_disconnect;
    h_MyApp.ext_myapp.process = libMyApp_process;
    h_MyApp.ext_myapp.set_operational = libMyApp_set_operational;
    h_MyApp.ext_myapp.dispose = libMyApp_dispose;
    
    exos_log_init(&logger, "MyApp_Linux");

    SUCCESS("starting MyApp_Linux application..");

    EXOS_ASSERT_OK(exos_datamodel_init(&h_MyApp.myapp, "MyApp", "MyApp_Linux"));

    //set the user_context to access custom data in the callbacks
    h_MyApp.myapp.user_context = NULL; //not used
    h_MyApp.myapp.user_tag = 0; //not used

    EXOS_ASSERT_OK(exos_dataset_init(&h_MyApp.execute_dataset, &h_MyApp.myapp, "execute", &h_MyApp.ext_myapp.execute.value, sizeof(h_MyApp.ext_myapp.execute.value)));
    h_MyApp.execute_dataset.user_context = NULL; //not used
    h_MyApp.execute_dataset.user_tag = 0; //not used

    EXOS_ASSERT_OK(exos_dataset_init(&h_MyApp.done_dataset, &h_MyApp.myapp, "done", &h_MyApp.ext_myapp.done.value, sizeof(h_MyApp.ext_myapp.done.value)));
    h_MyApp.done_dataset.user_context = NULL; //not used
    h_MyApp.done_dataset.user_tag = 0; //not used

    EXOS_ASSERT_OK(exos_dataset_init(&h_MyApp.parameters_dataset, &h_MyApp.myapp, "parameters", &h_MyApp.ext_myapp.parameters.value, sizeof(h_MyApp.ext_myapp.parameters.value)));
    h_MyApp.parameters_dataset.user_context = NULL; //not used
    h_MyApp.parameters_dataset.user_tag = 0; //not used

    EXOS_ASSERT_OK(exos_dataset_init(&h_MyApp.results_dataset, &h_MyApp.myapp, "results", &h_MyApp.ext_myapp.results.value, sizeof(h_MyApp.ext_myapp.results.value)));
    h_MyApp.results_dataset.user_context = NULL; //not used
    h_MyApp.results_dataset.user_tag = 0; //not used

    return &(h_MyApp.ext_myapp);
}

