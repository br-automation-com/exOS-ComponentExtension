#include <DataX.h>

#define EXOS_ASSERT_LOG &handle->logger
#define EXOS_ASSERT_CALLBACK inst->_state = 255;
#include "exos_log.h"
#include "exos_datax.h"
#include <string.h>

#define SUCCESS(_format_, ...) exos_log_success(&handle->logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define INFO(_format_, ...) exos_log_info(&handle->logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define VERBOSE(_format_, ...) exos_log_debug(&handle->logger, EXOS_LOG_TYPE_USER + EXOS_LOG_TYPE_VERBOSE, _format_, ##__VA_ARGS__);
#define ERROR(_format_, ...) exos_log_error(&handle->logger, _format_, ##__VA_ARGS__);

typedef struct
{
    void *self;
    exos_log_handle_t logger;
    DataX data;

    exos_datamodel_handle_t datax;

    exos_dataset_handle_t enable_dataset;
    exos_dataset_handle_t active_dataset;
    exos_dataset_handle_t countup;
    exos_dataset_handle_t countdown;
} DataXHandle_t;

static void datasetEvent(exos_dataset_handle_t *dataset, EXOS_DATASET_EVENT_TYPE event_type, void *info)
{
    struct DataXCyclic *inst = (struct DataXCyclic *)dataset->datamodel->user_context;
    DataXHandle_t *handle = (DataXHandle_t *)inst->Handle;

    switch (event_type)
    {
    case EXOS_DATASET_EVENT_UPDATED:
        VERBOSE("dataset %s updated! latency (us):%i", dataset->name, (exos_datamodel_get_nettime(dataset->datamodel,NULL) - dataset->nettime));
        //handle each subscription dataset separately
		if(0 == strcmp(dataset->name, "active"))
		{
			inst->active = *(BOOL *)dataset->data;
		}
		else if(0 == strcmp(dataset->name, "countUp"))
		{
			inst->countUp = *(DINT *)dataset->data;
		}
		else if(0 == strcmp(dataset->name, "countDown"))
		{
			inst->countDown = *(DINT *)dataset->data;
		}
        break;

    case EXOS_DATASET_EVENT_PUBLISHED:
        VERBOSE("dataset %s published to local server for distribution! send buffer free:%i", dataset->name, dataset->send_buffer.free);
        //handle each published dataset separately
		if(0 == strcmp(dataset->name, "enable"))
		{
			//BOOL *enable = *(BOOL *)dataset->data;
		}
        break;

    case EXOS_DATASET_EVENT_DELIVERED:
        VERBOSE("dataset %s delivered to remote server for distribution! send buffer free:%i", dataset->name, dataset->send_buffer.free);
        //handle each published dataset separately
		if(0 == strcmp(dataset->name, "enable"))
		{
			//BOOL *enable = *(BOOL *)dataset->data;
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
    struct DataXCyclic *inst = (struct DataXCyclic *)datamodel->user_context;
    DataXHandle_t *handle = (DataXHandle_t *)inst->Handle;

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
            SUCCESS("DataX operational!");
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

_BUR_PUBLIC void DataXInit(struct DataXInit *inst)
{
    DataXHandle_t *handle;
    TMP_alloc(sizeof(DataXHandle_t), (void **)&handle);
    if (NULL == handle)
    {
        inst->Handle = 0;
        return;
    }

    memset(&handle->data, 0, sizeof(handle->data));
    handle->self = handle;

    exos_log_init(&handle->logger, "DataX_AR");

    exos_datamodel_handle_t *datax = &handle->datax;
    exos_dataset_handle_t *enable_dataset = &handle->enable_dataset;
    exos_dataset_handle_t *active_dataset = &handle->active_dataset;
    exos_dataset_handle_t *countup = &handle->countup;
    exos_dataset_handle_t *countdown = &handle->countdown;
    EXOS_ASSERT_OK(exos_datamodel_init(datax, "DataX", "DataX_AR"));

    EXOS_ASSERT_OK(exos_dataset_init(enable_dataset, datax, "enable", &handle->data.enable, sizeof(handle->data.enable)));
    EXOS_ASSERT_OK(exos_dataset_init(active_dataset, datax, "active", &handle->data.active, sizeof(handle->data.active)));
    EXOS_ASSERT_OK(exos_dataset_init(countup, datax, "countUp", &handle->data.countUp, sizeof(handle->data.countUp)));
    EXOS_ASSERT_OK(exos_dataset_init(countdown, datax, "countDown", &handle->data.countDown, sizeof(handle->data.countDown)));
    
    inst->Handle = (UDINT)handle;
}

_BUR_PUBLIC void DataXCyclic(struct DataXCyclic *inst)
{
    DataXHandle_t *handle = (DataXHandle_t *)inst->Handle;

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

    DataX *data = &handle->data;
    exos_datamodel_handle_t *datax = &handle->datax;
    //the user context of the datamodel points to the DataXCyclic instance
    datax->user_context = inst; //set it cyclically in case the program using the FUB is retransferred
    datax->user_tag = 0; //user defined

    exos_dataset_handle_t *enable_dataset = &handle->enable_dataset;
    enable_dataset->user_context = NULL; //user defined
    enable_dataset->user_tag = 0; //user defined

    exos_dataset_handle_t *active_dataset = &handle->active_dataset;
    active_dataset->user_context = NULL; //user defined
    active_dataset->user_tag = 0; //user defined

    exos_dataset_handle_t *countup = &handle->countup;
    countup->user_context = NULL; //user defined
    countup->user_tag = 0; //user defined

    exos_dataset_handle_t *countdown = &handle->countdown;
    countdown->user_context = NULL; //user defined
    countdown->user_tag = 0; //user defined

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

        SUCCESS("starting DataX application..");

        //connect the datamodel, then the datasets
        EXOS_ASSERT_OK(exos_datamodel_connect_datax(datax, datamodelEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(enable_dataset, EXOS_DATASET_PUBLISH, datasetEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(active_dataset, EXOS_DATASET_SUBSCRIBE, datasetEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(countup, EXOS_DATASET_SUBSCRIBE, datasetEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(countdown, EXOS_DATASET_SUBSCRIBE, datasetEvent));

        inst->Active = true;
        break;

    case 100:
    case 101:
        if (inst->Start)
        {
            if (inst->_state == 100)
            {
                EXOS_ASSERT_OK(exos_datamodel_set_operational(datax));
                inst->_state = 101;
            }
        }
        else
        {
            inst->_state = 100;
        }

        EXOS_ASSERT_OK(exos_datamodel_process(datax));
        //put your cyclic code here!

        //publish the enable_dataset dataset as soon as there are changes
        if (inst->enable != data->enable)
        {
            data->enable = inst->enable;
            exos_dataset_publish(enable_dataset);
        }
        break;

    case 255:
        //disconnect the datamodel
        EXOS_ASSERT_OK(exos_datamodel_disconnect(datax));

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

_BUR_PUBLIC void DataXExit(struct DataXExit *inst)
{
    DataXHandle_t *handle = (DataXHandle_t *)inst->Handle;

    if (NULL == handle)
    {
        ERROR("DataXExit: NULL handle, cannot delete resources");
        return;
    }
    if ((void *)handle != handle->self)
    {
        ERROR("DataXExit: invalid handle, cannot delete resources");
        return;
    }

    exos_datamodel_handle_t *datax = &handle->datax;

    EXOS_ASSERT_OK(exos_datamodel_delete(datax));

    //finish with deleting the log
    exos_log_delete(&handle->logger);
    //free the allocated handle
    TMP_free(sizeof(DataXHandle_t), (void *)handle);
}

