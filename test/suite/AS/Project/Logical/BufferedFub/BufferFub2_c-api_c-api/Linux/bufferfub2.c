#include <unistd.h>
#include <string.h>
#include "termination.h"

#define EXOS_ASSERT_LOG &logger
#include "exos_log.h"
#include "exos_bufferfub2.h"

#define SUCCESS(_format_, ...) exos_log_success(&logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define INFO(_format_, ...) exos_log_info(&logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define VERBOSE(_format_, ...) exos_log_debug(&logger, EXOS_LOG_TYPE_USER + EXOS_LOG_TYPE_VERBOSE, _format_, ##__VA_ARGS__);
#define ERROR(_format_, ...) exos_log_error(&logger, _format_, ##__VA_ARGS__);

exos_log_handle_t logger;

static void datasetEvent(exos_dataset_handle_t *dataset, EXOS_DATASET_EVENT_TYPE event_type, void *info)
{
    switch (event_type)
    {
    case EXOS_DATASET_EVENT_UPDATED:
        VERBOSE("dataset %s updated! latency (us):%i", dataset->name, (exos_datamodel_get_nettime(dataset->datamodel) - dataset->nettime));
        //handle each subscription dataset separately
        if(0 == strcmp(dataset->name,"setup"))
        {
            BufferFub2Setup_typ *setup_dataset = (BufferFub2Setup_typ *)dataset->data;
        }
        else if(0 == strcmp(dataset->name,"cmdSendBurst"))
        {
            bool *cmdsendburst = (bool *)dataset->data;
        }
        break;

    case EXOS_DATASET_EVENT_PUBLISHED:
        VERBOSE("dataset %s published to local server for distribution! send buffer free:%i", dataset->name, dataset->send_buffer.free);
        //handle each published dataset separately
        if(0 == strcmp(dataset->name, "bufferedSample"))
        {
            BufferFub2Sample_typ *bufferedsample = (BufferFub2Sample_typ *)dataset->data;
        }
        break;

    case EXOS_DATASET_EVENT_DELIVERED:
        VERBOSE("dataset %s delivered to remote server for distribution! send buffer free:%i", dataset->name, dataset->send_buffer.free);
        //handle each published dataset separately
        if(0 == strcmp(dataset->name, "bufferedSample"))
        {
            BufferFub2Sample_typ *bufferedsample = (BufferFub2Sample_typ *)dataset->data;
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
    switch (event_type)
    {
    case EXOS_DATAMODEL_EVENT_CONNECTION_CHANGED:
        INFO("application changed state to %s", exos_get_state_string(datamodel->connection_state));

        switch (datamodel->connection_state)
        {
        case EXOS_STATE_DISCONNECTED:
            break;
        case EXOS_STATE_CONNECTED:
            break;
        case EXOS_STATE_OPERATIONAL:
            SUCCESS("BufferFub2 operational!");
            break;
        case EXOS_STATE_ABORTED:
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

int main()
{
    BufferFub2 data;

    exos_datamodel_handle_t bufferfub2;

    exos_dataset_handle_t bufferedsample;
    exos_dataset_handle_t setup_dataset;
    exos_dataset_handle_t cmdsendburst;
    
    exos_log_init(&logger, "gBufferFub2_0");

    SUCCESS("starting BufferFub2 application..");

    EXOS_ASSERT_OK(exos_datamodel_init(&bufferfub2, "BufferFub2_0", "gBufferFub2_0"));

    //set the user_context to access custom data in the callbacks
    bufferfub2.user_context = NULL; //user defined
    bufferfub2.user_tag = 0; //user defined

    EXOS_ASSERT_OK(exos_dataset_init(&bufferedsample, &bufferfub2, "bufferedSample", &data.bufferedSample, sizeof(data.bufferedSample)));
    bufferedsample.user_context = NULL; //user defined
    bufferedsample.user_tag = 0; //user defined

    EXOS_ASSERT_OK(exos_dataset_init(&setup_dataset, &bufferfub2, "setup", &data.setup, sizeof(data.setup)));
    setup_dataset.user_context = NULL; //user defined
    setup_dataset.user_tag = 0; //user defined

    EXOS_ASSERT_OK(exos_dataset_init(&cmdsendburst, &bufferfub2, "cmdSendBurst", &data.cmdSendBurst, sizeof(data.cmdSendBurst)));
    cmdsendburst.user_context = NULL; //user defined
    cmdsendburst.user_tag = 0; //user defined

    //connect the datamodel
    EXOS_ASSERT_OK(exos_datamodel_connect_bufferfub2(&bufferfub2, datamodelEvent));
    
    //connect datasets
    EXOS_ASSERT_OK(exos_dataset_connect(&bufferedsample, EXOS_DATASET_PUBLISH, datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&setup_dataset, EXOS_DATASET_SUBSCRIBE, datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&cmdsendburst, EXOS_DATASET_SUBSCRIBE, datasetEvent));
    
    catch_termination();
    while (true)
    {
        EXOS_ASSERT_OK(exos_datamodel_process(&bufferfub2));
        exos_log_process(&logger);

        //put your cyclic code here!

        if (is_terminated())
        {
            SUCCESS("BufferFub2 application terminated, closing..");
            break;
        }
    }


    EXOS_ASSERT_OK(exos_datamodel_delete(&bufferfub2));

    //finish with deleting the log
    exos_log_delete(&logger);
    return 0;
}
