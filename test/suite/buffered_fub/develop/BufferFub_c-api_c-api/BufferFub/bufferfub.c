#include <BufferFub.h>

#define EXOS_ASSERT_LOG &handle->logger
#define EXOS_ASSERT_CALLBACK inst->_state = 255;
#include "exos_log.h"
#include "exos_bufferfub.h"
#include <string.h>

#define SUCCESS(_format_, ...) exos_log_success(&handle->logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define INFO(_format_, ...) exos_log_info(&handle->logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define VERBOSE(_format_, ...) exos_log_debug(&handle->logger, EXOS_LOG_TYPE_USER + EXOS_LOG_TYPE_VERBOSE, _format_, ##__VA_ARGS__);
#define ERROR(_format_, ...) exos_log_error(&handle->logger, _format_, ##__VA_ARGS__);

typedef struct
{
    void *self;
    exos_log_handle_t logger;
    BufferFub data;

    exos_datamodel_handle_t bufferfub;

    exos_dataset_handle_t bufferedsample;
    exos_dataset_handle_t setup_dataset;
    exos_dataset_handle_t cmdsendburst;
} BufferFubHandle_t;

static void datasetEvent(exos_dataset_handle_t *dataset, EXOS_DATASET_EVENT_TYPE event_type, void *info)
{
    struct BufferFubCyclic *inst = (struct BufferFubCyclic *)dataset->datamodel->user_context;
    BufferFubHandle_t *handle = (BufferFubHandle_t *)inst->Handle;

    switch (event_type)
    {
    case EXOS_DATASET_EVENT_UPDATED:
        VERBOSE("dataset %s updated! latency (us):%i", dataset->name, (exos_datamodel_get_nettime(dataset->datamodel) - dataset->nettime));
        //handle each subscription dataset separately
        if(0 == strcmp(dataset->name, "bufferedSample"))
        {
            inst->pBufferFub->bufferedSample = *(UDINT *)dataset->data;
        }
        break;

    case EXOS_DATASET_EVENT_PUBLISHED:
        VERBOSE("dataset %s published to local server for distribution! send buffer free:%i", dataset->name, dataset->send_buffer.free);
        //handle each published dataset separately
        if(0 == strcmp(dataset->name, "setup"))
        {
            // BufferFubSetup_typ *setup_dataset = (BufferFubSetup_typ *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "cmdSendBurst"))
        {
            // BOOL *cmdsendburst = (BOOL *)dataset->data;
        }
        break;

    case EXOS_DATASET_EVENT_DELIVERED:
        VERBOSE("dataset %s delivered to remote server for distribution! send buffer free:%i", dataset->name, dataset->send_buffer.free);
        //handle each published dataset separately
        if(0 == strcmp(dataset->name, "setup"))
        {
            // BufferFubSetup_typ *setup_dataset = (BufferFubSetup_typ *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "cmdSendBurst"))
        {
            // BOOL *cmdsendburst = (BOOL *)dataset->data;
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
    struct BufferFubCyclic *inst = (struct BufferFubCyclic *)datamodel->user_context;
    BufferFubHandle_t *handle = (BufferFubHandle_t *)inst->Handle;

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
            SUCCESS("BufferFub operational!");
            inst->Operational = 1;
            break;
        case EXOS_STATE_ABORTED:
            ERROR("application error %d (%s) occured", datamodel->error, exos_get_error_string(datamodel->error));
            inst->_state = 255;
            inst->Aborted = 1;
            break;
        }
        break;
    case EXOS_DATAMODEL_EVENT_SYNC_STATE_CHANGED:
        break;

    default:
        break;

    }

}

_BUR_PUBLIC void BufferFubInit(struct BufferFubInit *inst)
{
    BufferFubHandle_t *handle;
    TMP_alloc(sizeof(BufferFubHandle_t), (void **)&handle);
    if (NULL == handle)
    {
        inst->Handle = 0;
        return;
    }

    memset(&handle->data, 0, sizeof(handle->data));
    handle->self = handle;

    exos_log_init(&handle->logger, "gBufferFub_0");

    
    
    exos_datamodel_handle_t *bufferfub = &handle->bufferfub;
    exos_dataset_handle_t *bufferedsample = &handle->bufferedsample;
    exos_dataset_handle_t *setup_dataset = &handle->setup_dataset;
    exos_dataset_handle_t *cmdsendburst = &handle->cmdsendburst;
    EXOS_ASSERT_OK(exos_datamodel_init(bufferfub, "BufferFub_0", "gBufferFub_0"));

    EXOS_ASSERT_OK(exos_dataset_init(bufferedsample, bufferfub, "bufferedSample", &handle->data.bufferedSample, sizeof(handle->data.bufferedSample)));
    EXOS_ASSERT_OK(exos_dataset_init(setup_dataset, bufferfub, "setup", &handle->data.setup, sizeof(handle->data.setup)));
    EXOS_ASSERT_OK(exos_dataset_init(cmdsendburst, bufferfub, "cmdSendBurst", &handle->data.cmdSendBurst, sizeof(handle->data.cmdSendBurst)));
    
    inst->Handle = (UDINT)handle;
}

_BUR_PUBLIC void BufferFubCyclic(struct BufferFubCyclic *inst)
{
    BufferFubHandle_t *handle = (BufferFubHandle_t *)inst->Handle;

    inst->Error = false;
    if (NULL == handle || NULL == inst->pBufferFub)
    {
        inst->Error = true;
        return;
    }
    if ((void *)handle != handle->self)
    {
        inst->Error = true;
        return;
    }

    BufferFub *data = &handle->data;
    exos_datamodel_handle_t *bufferfub = &handle->bufferfub;
    //the user context of the datamodel points to the BufferFubCyclic instance
    bufferfub->user_context = inst; //set it cyclically in case the program using the FUB is retransferred
    bufferfub->user_tag = 0; //user defined
    //handle online download of the library
    if(NULL != bufferfub->datamodel_event_callback && bufferfub->datamodel_event_callback != datamodelEvent)
    {
        bufferfub->datamodel_event_callback = datamodelEvent;
        exos_log_delete(&handle->logger);
        exos_log_init(&handle->logger, "gBufferFub_0");
    }

    exos_dataset_handle_t *bufferedsample = &handle->bufferedsample;
    bufferedsample->user_context = NULL; //user defined
    bufferedsample->user_tag = 0; //user defined
    //handle online download of the library
    if(NULL != bufferedsample->dataset_event_callback && bufferedsample->dataset_event_callback != datasetEvent)
    {
        bufferedsample->dataset_event_callback = datasetEvent;
    }

    exos_dataset_handle_t *setup_dataset = &handle->setup_dataset;
    setup_dataset->user_context = NULL; //user defined
    setup_dataset->user_tag = 0; //user defined
    //handle online download of the library
    if(NULL != setup_dataset->dataset_event_callback && setup_dataset->dataset_event_callback != datasetEvent)
    {
        setup_dataset->dataset_event_callback = datasetEvent;
    }

    exos_dataset_handle_t *cmdsendburst = &handle->cmdsendburst;
    cmdsendburst->user_context = NULL; //user defined
    cmdsendburst->user_tag = 0; //user defined
    //handle online download of the library
    if(NULL != cmdsendburst->dataset_event_callback && cmdsendburst->dataset_event_callback != datasetEvent)
    {
        cmdsendburst->dataset_event_callback = datasetEvent;
    }

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

        SUCCESS("starting BufferFub application..");

        //connect the datamodel, then the datasets
        EXOS_ASSERT_OK(exos_datamodel_connect_bufferfub(bufferfub, datamodelEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(bufferedsample, EXOS_DATASET_SUBSCRIBE, datasetEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(setup_dataset, EXOS_DATASET_PUBLISH, datasetEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(cmdsendburst, EXOS_DATASET_PUBLISH, datasetEvent));

        inst->Active = true;
        break;

    case 100:
    case 101:
        if (inst->Start)
        {
            if (inst->_state == 100)
            {
                EXOS_ASSERT_OK(exos_datamodel_set_operational(bufferfub));
                inst->_state = 101;
            }
        }
        else
        {
            inst->_state = 100;
        }

        EXOS_ASSERT_OK(exos_datamodel_process(bufferfub));
        //put your cyclic code here!

        //publish the setup_dataset dataset as soon as there are changes
        if (0 != memcmp(&inst->pBufferFub->setup, &data->setup, sizeof(data->setup)))
        {
            memcpy(&data->setup, &inst->pBufferFub->setup, sizeof(data->setup));
            exos_dataset_publish(setup_dataset);
        }
        //publish the cmdsendburst dataset as soon as there are changes
        if (inst->pBufferFub->cmdSendBurst != data->cmdSendBurst)
        {
            data->cmdSendBurst = inst->pBufferFub->cmdSendBurst;
            exos_dataset_publish(cmdsendburst);
        }

        break;

    case 255:
        //disconnect the datamodel
        EXOS_ASSERT_OK(exos_datamodel_disconnect(bufferfub));

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

_BUR_PUBLIC void BufferFubExit(struct BufferFubExit *inst)
{
    BufferFubHandle_t *handle = (BufferFubHandle_t *)inst->Handle;

    if (NULL == handle)
    {
        ERROR("BufferFubExit: NULL handle, cannot delete resources");
        return;
    }
    if ((void *)handle != handle->self)
    {
        ERROR("BufferFubExit: invalid handle, cannot delete resources");
        return;
    }

    exos_datamodel_handle_t *bufferfub = &handle->bufferfub;

    EXOS_ASSERT_OK(exos_datamodel_delete(bufferfub));

    //finish with deleting the log
    exos_log_delete(&handle->logger);
    //free the allocated handle
    TMP_free(sizeof(BufferFubHandle_t), (void *)handle);
}

