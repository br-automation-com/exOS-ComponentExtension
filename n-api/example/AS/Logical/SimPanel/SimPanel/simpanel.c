#include <SimPanel.h>

#define EXOS_ASSERT_LOG &handle->logger
#define EXOS_ASSERT_CALLBACK inst->_state = 255;
#include "exos_log.h"
#include "exos_simpanel.h"
#include <string.h>

#define SUCCESS(_format_, ...) exos_log_success(&handle->logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define INFO(_format_, ...) exos_log_info(&handle->logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define VERBOSE(_format_, ...) exos_log_debug(&handle->logger, EXOS_LOG_TYPE_USER + EXOS_LOG_TYPE_VERBOSE, _format_, ##__VA_ARGS__);
#define ERROR(_format_, ...) exos_log_error(&handle->logger, _format_, ##__VA_ARGS__);

typedef struct
{
    void *self;
    exos_log_handle_t logger;
    SimPanel data;

    exos_datamodel_handle_t simpanel;

    exos_dataset_handle_t knobs;
    exos_dataset_handle_t display;
    exos_dataset_handle_t encoder;
} SimPanelHandle_t;

static void datasetEvent(exos_dataset_handle_t *dataset, EXOS_DATASET_EVENT_TYPE event_type, void *info)
{
    struct SimPanelCyclic *inst = (struct SimPanelCyclic *)dataset->datamodel->user_context;
    SimPanelHandle_t *handle = (SimPanelHandle_t *)inst->Handle;

    switch (event_type)
    {
    case EXOS_DATASET_EVENT_UPDATED:
        VERBOSE("dataset %s updated! latency (us):%i", dataset->name, (exos_datamodel_get_nettime(dataset->datamodel,NULL) - dataset->nettime));
        //handle each subscription dataset separately
        if(0 == strcmp(dataset->name, "Knobs"))
        {
            if(NULL != inst->Knobs)
            {
                memcpy(inst->Knobs, dataset->data, dataset->size);
            }
        }
        else if(0 == strcmp(dataset->name, "Display"))
        {
            inst->Display = *(INT *)dataset->data;
        }
        break;

    case EXOS_DATASET_EVENT_PUBLISHED:
        VERBOSE("dataset %s published to local server for distribution! send buffer free:%i", dataset->name, dataset->send_buffer.free);
        //handle each published dataset separately
        if(0 == strcmp(dataset->name, "Knobs"))
        {
            // SimPanelKnobs *knobs = (SimPanelKnobs *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "Encoder"))
        {
            // UINT *encoder = (UINT *)dataset->data;
        }
        break;

    case EXOS_DATASET_EVENT_DELIVERED:
        VERBOSE("dataset %s delivered to remote server for distribution! send buffer free:%i", dataset->name, dataset->send_buffer.free);
        //handle each published dataset separately
        if(0 == strcmp(dataset->name, "Knobs"))
        {
            // SimPanelKnobs *knobs = (SimPanelKnobs *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "Encoder"))
        {
            // UINT *encoder = (UINT *)dataset->data;
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
    struct SimPanelCyclic *inst = (struct SimPanelCyclic *)datamodel->user_context;
    SimPanelHandle_t *handle = (SimPanelHandle_t *)inst->Handle;

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
            SUCCESS("SimPanel operational!");
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

_BUR_PUBLIC void SimPanelInit(struct SimPanelInit *inst)
{
    SimPanelHandle_t *handle;
    TMP_alloc(sizeof(SimPanelHandle_t), (void **)&handle);
    if (NULL == handle)
    {
        inst->Handle = 0;
        return;
    }

    memset(&handle->data, 0, sizeof(handle->data));
    handle->self = handle;

    exos_log_init(&handle->logger, "SimPanel_AR");

    
    
    exos_datamodel_handle_t *simpanel = &handle->simpanel;
    exos_dataset_handle_t *knobs = &handle->knobs;
    exos_dataset_handle_t *display = &handle->display;
    exos_dataset_handle_t *encoder = &handle->encoder;
    EXOS_ASSERT_OK(exos_datamodel_init(simpanel, "SimPanel", "SimPanel_AR"));

    EXOS_ASSERT_OK(exos_dataset_init(knobs, simpanel, "Knobs", &handle->data.Knobs, sizeof(handle->data.Knobs)));
    EXOS_ASSERT_OK(exos_dataset_init(display, simpanel, "Display", &handle->data.Display, sizeof(handle->data.Display)));
    EXOS_ASSERT_OK(exos_dataset_init(encoder, simpanel, "Encoder", &handle->data.Encoder, sizeof(handle->data.Encoder)));
    
    inst->Handle = (UDINT)handle;
}

_BUR_PUBLIC void SimPanelCyclic(struct SimPanelCyclic *inst)
{
    SimPanelHandle_t *handle = (SimPanelHandle_t *)inst->Handle;

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

    SimPanel *data = &handle->data;
    exos_datamodel_handle_t *simpanel = &handle->simpanel;
    //the user context of the datamodel points to the SimPanelCyclic instance
    simpanel->user_context = inst; //set it cyclically in case the program using the FUB is retransferred
    simpanel->user_tag = 0; //user defined

    exos_dataset_handle_t *knobs = &handle->knobs;
    knobs->user_context = NULL; //user defined
    knobs->user_tag = 0; //user defined

    exos_dataset_handle_t *display = &handle->display;
    display->user_context = NULL; //user defined
    display->user_tag = 0; //user defined

    exos_dataset_handle_t *encoder = &handle->encoder;
    encoder->user_context = NULL; //user defined
    encoder->user_tag = 0; //user defined

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

        SUCCESS("starting SimPanel application..");

        //connect the datamodel, then the datasets
        EXOS_ASSERT_OK(exos_datamodel_connect_simpanel(simpanel, datamodelEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(knobs, EXOS_DATASET_PUBLISH + EXOS_DATASET_SUBSCRIBE, datasetEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(display, EXOS_DATASET_SUBSCRIBE, datasetEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(encoder, EXOS_DATASET_PUBLISH, datasetEvent));

        inst->Active = true;
        break;

    case 100:
    case 101:
        if (inst->Start)
        {
            if (inst->_state == 100)
            {
                EXOS_ASSERT_OK(exos_datamodel_set_operational(simpanel));
                inst->_state = 101;
            }
        }
        else
        {
            inst->_state = 100;
        }

        EXOS_ASSERT_OK(exos_datamodel_process(simpanel));
        //put your cyclic code here!

        if (NULL != inst->Knobs)
        {
            //publish the knobs dataset as soon as there are changes
            if (0 != memcmp(inst->Knobs, &data->Knobs, sizeof(data->Knobs)))
            {
                memcpy(&data->Knobs, inst->Knobs, sizeof(data->Knobs));
                exos_dataset_publish(knobs);
            }
        }
        //publish the encoder dataset as soon as there are changes
        if (inst->Encoder != data->Encoder)
        {
            data->Encoder = inst->Encoder;
            exos_dataset_publish(encoder);
        }

        break;

    case 255:
        //disconnect the datamodel
        EXOS_ASSERT_OK(exos_datamodel_disconnect(simpanel));

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

_BUR_PUBLIC void SimPanelExit(struct SimPanelExit *inst)
{
    SimPanelHandle_t *handle = (SimPanelHandle_t *)inst->Handle;

    if (NULL == handle)
    {
        ERROR("SimPanelExit: NULL handle, cannot delete resources");
        return;
    }
    if ((void *)handle != handle->self)
    {
        ERROR("SimPanelExit: invalid handle, cannot delete resources");
        return;
    }

    exos_datamodel_handle_t *simpanel = &handle->simpanel;

    EXOS_ASSERT_OK(exos_datamodel_delete(simpanel));

    //finish with deleting the log
    exos_log_delete(&handle->logger);
    //free the allocated handle
    TMP_free(sizeof(SimPanelHandle_t), (void *)handle);
}

