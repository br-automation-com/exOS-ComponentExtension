#include <libMyApp.h>

#define EXOS_ASSERT_LOG &handle->logger
#define EXOS_ASSERT_CALLBACK inst->_state = 255;
#include "exos_log.h"
#include "exos_myapp.h"
#include <string.h>

#define SUCCESS(_format_, ...) exos_log_success(&handle->logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define INFO(_format_, ...) exos_log_info(&handle->logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define VERBOSE(_format_, ...) exos_log_debug(&handle->logger, EXOS_LOG_TYPE_USER + EXOS_LOG_TYPE_VERBOSE, _format_, ##__VA_ARGS__);
#define ERROR(_format_, ...) exos_log_error(&handle->logger, _format_, ##__VA_ARGS__);

typedef struct
{
    void *self;
    exos_log_handle_t logger;
    MyApp data;

    exos_datamodel_handle_t myapp;

    exos_dataset_handle_t execute_dataset;
    exos_dataset_handle_t done_dataset;
    exos_dataset_handle_t parameters_dataset;
    exos_dataset_handle_t results_dataset;
} MyAppHandle_t;

static void datasetEvent(exos_dataset_handle_t *dataset, EXOS_DATASET_EVENT_TYPE event_type, void *info)
{
    struct MyAppCyclic *inst = (struct MyAppCyclic *)dataset->datamodel->user_context;
    MyAppHandle_t *handle = (MyAppHandle_t *)inst->Handle;

    switch (event_type)
    {
    case EXOS_DATASET_EVENT_UPDATED:
        VERBOSE("dataset %s updated! latency (us):%i", dataset->name, (exos_datamodel_get_nettime(dataset->datamodel,NULL) - dataset->nettime));
        //handle each subscription dataset separately
        if(0 == strcmp(dataset->name, "done"))
        {
            if(NULL != inst->done)
            {
                *inst->done = *(BOOL *)dataset->data;
            }
        }
        else if(0 == strcmp(dataset->name, "results"))
        {
            memcpy(&inst->results, dataset->data, dataset->size);
        }
        break;

    case EXOS_DATASET_EVENT_PUBLISHED:
        VERBOSE("dataset %s published to local server for distribution! send buffer free:%i", dataset->name, dataset->send_buffer.free);
        //handle each published dataset separately
        if(0 == strcmp(dataset->name, "execute"))
        {
            // BOOL *execute_dataset = (BOOL *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "done"))
        {
            // BOOL *done_dataset = (BOOL *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "parameters"))
        {
            // MyAppPar_t *parameters_dataset = (MyAppPar_t *)dataset->data;
        }
        break;

    case EXOS_DATASET_EVENT_DELIVERED:
        VERBOSE("dataset %s delivered to remote server for distribution! send buffer free:%i", dataset->name, dataset->send_buffer.free);
        //handle each published dataset separately
        if(0 == strcmp(dataset->name, "execute"))
        {
            // BOOL *execute_dataset = (BOOL *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "done"))
        {
            // BOOL *done_dataset = (BOOL *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "parameters"))
        {
            // MyAppPar_t *parameters_dataset = (MyAppPar_t *)dataset->data;
        }
        break;

    case EXOS_DATASET_EVENT_CONNECTION_CHANGED:
        INFO("dataset %s changed state to %s", dataset->name, exos_get_state_string(dataset->connection_state));

        switch (dataset->connection_state)
        {
        case EXOS_STATE_DISCONNECTED:
            break;
        case EXOS_STATE_CONNECTED:
            //call the dataset changed event to update the dataset when connected
            //datasetEvent(dataset,EXOS_DATASET_UPDATED,info);
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

static void datamodelEvent(exos_datamodel_handle_t *datamodel, const EXOS_DATAMODEL_EVENT_TYPE event_type, void *info)
{
    struct MyAppCyclic *inst = (struct MyAppCyclic *)datamodel->user_context;
    MyAppHandle_t *handle = (MyAppHandle_t *)inst->Handle;

    switch (event_type)
    {
    case EXOS_DATAMODEL_EVENT_CONNECTION_CHANGED:
        INFO("application changed state to %s", exos_get_state_string(datamodel->connection_state));

        inst->Disconnected = 0;
        inst->Connected = 0;
        inst->Operational = 0;
        inst->Aborted = 0;

        switch (datamodel->connection_state)
        {
        case EXOS_STATE_DISCONNECTED:
            inst->Disconnected = 1;
            inst->_state = 255;
            break;
        case EXOS_STATE_CONNECTED:
            inst->Connected = 1;
            break;
        case EXOS_STATE_OPERATIONAL:
            SUCCESS("MyApp operational!");
            inst->Operational = 1;
            break;
        case EXOS_STATE_ABORTED:
            ERROR("application error %d (%s) occured", datamodel->error, exos_get_error_string(datamodel->error));
            inst->_state = 255;
            inst->Aborted = 1;
            break;
        }
        break;
    }
}

_BUR_PUBLIC void MyAppInit(struct MyAppInit *inst)
{
    MyAppHandle_t *handle;
    TMP_alloc(sizeof(MyAppHandle_t), (void **)&handle);
    if (NULL == handle)
    {
        inst->Handle = 0;
        return;
    }

    memset(&handle->data, 0, sizeof(handle->data));
    handle->self = handle;

    exos_log_init(&handle->logger, "MyApp_AR");

    
    
    exos_datamodel_handle_t *myapp = &handle->myapp;
    exos_dataset_handle_t *execute_dataset = &handle->execute_dataset;
    exos_dataset_handle_t *done_dataset = &handle->done_dataset;
    exos_dataset_handle_t *parameters_dataset = &handle->parameters_dataset;
    exos_dataset_handle_t *results_dataset = &handle->results_dataset;
    EXOS_ASSERT_OK(exos_datamodel_init(myapp, "MyApp", "MyApp_AR"));

    EXOS_ASSERT_OK(exos_dataset_init(execute_dataset, myapp, "execute", &handle->data.execute, sizeof(handle->data.execute)));
    EXOS_ASSERT_OK(exos_dataset_init(done_dataset, myapp, "done", &handle->data.done, sizeof(handle->data.done)));
    EXOS_ASSERT_OK(exos_dataset_init(parameters_dataset, myapp, "parameters", &handle->data.parameters, sizeof(handle->data.parameters)));
    EXOS_ASSERT_OK(exos_dataset_init(results_dataset, myapp, "results", &handle->data.results, sizeof(handle->data.results)));
    
    inst->Handle = (UDINT)handle;
}

_BUR_PUBLIC void MyAppCyclic(struct MyAppCyclic *inst)
{
    MyAppHandle_t *handle = (MyAppHandle_t *)inst->Handle;

    inst->Error = false;
    if (NULL == handle)
    {
        inst->Error = true;
        return;
    }
    if ((void *)handle != handle->self)
    {
        inst->Error = true;
        return;
    }

    MyApp *data = &handle->data;
    exos_datamodel_handle_t *myapp = &handle->myapp;
    //the user context of the datamodel points to the MyAppCyclic instance
    myapp->user_context = inst; //set it cyclically in case the program using the FUB is retransferred
    myapp->user_tag = 0; //user defined

    exos_dataset_handle_t *execute_dataset = &handle->execute_dataset;
    execute_dataset->user_context = NULL; //user defined
    execute_dataset->user_tag = 0; //user defined

    exos_dataset_handle_t *done_dataset = &handle->done_dataset;
    done_dataset->user_context = NULL; //user defined
    done_dataset->user_tag = 0; //user defined

    exos_dataset_handle_t *parameters_dataset = &handle->parameters_dataset;
    parameters_dataset->user_context = NULL; //user defined
    parameters_dataset->user_tag = 0; //user defined

    exos_dataset_handle_t *results_dataset = &handle->results_dataset;
    results_dataset->user_context = NULL; //user defined
    results_dataset->user_tag = 0; //user defined

    //unregister on disable
    if (inst->_state && !inst->Enable)
    {
        inst->_state = 255;
    }

    switch (inst->_state)
    {
    case 0:
        inst->Disconnected = 1;
        inst->Connected = 0;
        inst->Operational = 0;
        inst->Aborted = 0;

        if (inst->Enable)
        {
            inst->_state = 10;
        }
        break;

    case 10:
        inst->_state = 100;

        SUCCESS("starting MyApp application..");

        //connect the datamodel, then the datasets
        EXOS_ASSERT_OK(exos_datamodel_connect_myapp(myapp, datamodelEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(execute_dataset, EXOS_DATASET_PUBLISH, datasetEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(done_dataset, EXOS_DATASET_PUBLISH + EXOS_DATASET_SUBSCRIBE, datasetEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(parameters_dataset, EXOS_DATASET_PUBLISH, datasetEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(results_dataset, EXOS_DATASET_SUBSCRIBE, datasetEvent));

        inst->Active = true;
        break;

    case 100:
    case 101:
        if (inst->Start)
        {
            if (inst->_state == 100)
            {
                EXOS_ASSERT_OK(exos_datamodel_set_operational(myapp));
                inst->_state = 101;
            }
        }
        else
        {
            inst->_state = 100;
        }

        EXOS_ASSERT_OK(exos_datamodel_process(myapp));
        //put your cyclic code here!

        //publish the execute_dataset dataset as soon as there are changes
        if (inst->execute != data->execute)
        {
            data->execute = inst->execute;
            exos_dataset_publish(execute_dataset);
        }
        if (NULL != inst->done)
        {
            //publish the done_dataset dataset as soon as there are changes
            if (*inst->done != data->done)
            {
                data->done = *inst->done;
                exos_dataset_publish(done_dataset);
            }
        }
        //publish the parameters_dataset dataset as soon as there are changes
        if (0 != memcmp(&inst->parameters, &data->parameters, sizeof(data->parameters)))
        {
            memcpy(&data->parameters, &inst->parameters, sizeof(data->parameters));
            exos_dataset_publish(parameters_dataset);
        }

        break;

    case 255:
        //disconnect the datamodel
        EXOS_ASSERT_OK(exos_datamodel_disconnect(myapp));

        inst->Active = false;
        inst->_state = 254;
        //no break

    case 254:
        if (!inst->Enable)
            inst->_state = 0;
        break;
    }

    exos_log_process(&handle->logger);

}

_BUR_PUBLIC void MyAppExit(struct MyAppExit *inst)
{
    MyAppHandle_t *handle = (MyAppHandle_t *)inst->Handle;

    if (NULL == handle)
    {
        ERROR("MyAppExit: NULL handle, cannot delete resources");
        return;
    }
    if ((void *)handle != handle->self)
    {
        ERROR("MyAppExit: invalid handle, cannot delete resources");
        return;
    }

    exos_datamodel_handle_t *myapp = &handle->myapp;

    EXOS_ASSERT_OK(exos_datamodel_delete(myapp));

    //finish with deleting the log
    exos_log_delete(&handle->logger);
    //free the allocated handle
    TMP_free(sizeof(MyAppHandle_t), (void *)handle);
}

