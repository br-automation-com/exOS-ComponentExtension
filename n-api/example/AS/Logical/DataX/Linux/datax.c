#include <unistd.h>
#include <string.h>
#include "termination.h"

#define EXOS_ASSERT_LOG &logger
#include "exos_log.h"
#include "exos_datax.h"

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
        VERBOSE("dataset %s updated! latency (us):%i", dataset->name, (exos_datamodel_get_nettime(dataset->datamodel,NULL) - dataset->nettime));
        //handle each subscription dataset separately
        if(0 == strcmp(dataset->name,"enable"))
        {
            bool *enable_dataset = (bool *)dataset->data;
        }
        break;

    case EXOS_DATASET_EVENT_PUBLISHED:
        VERBOSE("dataset %s published to local server for distribution! send buffer free:%i", dataset->name, dataset->send_buffer.free);
        //handle each published dataset separately
        if(0 == strcmp(dataset->name, "active"))
        {
            bool *active_dataset = (bool *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "countUp"))
        {
            int32_t *countup = (int32_t *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "countDown"))
        {
            int32_t *countdown = (int32_t *)dataset->data;
        }
        break;

    case EXOS_DATASET_EVENT_DELIVERED:
        VERBOSE("dataset %s delivered to remote server for distribution! send buffer free:%i", dataset->name, dataset->send_buffer.free);
        //handle each published dataset separately
        if(0 == strcmp(dataset->name, "active"))
        {
            bool *active_dataset = (bool *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "countUp"))
        {
            int32_t *countup = (int32_t *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "countDown"))
        {
            int32_t *countdown = (int32_t *)dataset->data;
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
            SUCCESS("DataX operational!");
            break;
        case EXOS_STATE_ABORTED:
            ERROR("application error %d (%s) occured", datamodel->error, exos_get_error_string(datamodel->error));
            break;
        }
        break;
    }
}

int main()
{
    DataX data;

    exos_datamodel_handle_t datax;

    exos_dataset_handle_t enable_dataset;
    exos_dataset_handle_t active_dataset;
    exos_dataset_handle_t countup;
    exos_dataset_handle_t countdown;
    
    exos_log_init(&logger, "DataX_Linux");

    SUCCESS("starting DataX application..");

    EXOS_ASSERT_OK(exos_datamodel_init(&datax, "DataX", "DataX_Linux"));

    //set the user_context to access custom data in the callbacks
    datax.user_context = NULL; //user defined
    datax.user_tag = 0; //user defined

    EXOS_ASSERT_OK(exos_dataset_init(&enable_dataset, &datax, "enable", &data.enable, sizeof(data.enable)));
    enable_dataset.user_context = NULL; //user defined
    enable_dataset.user_tag = 0; //user defined

    EXOS_ASSERT_OK(exos_dataset_init(&active_dataset, &datax, "active", &data.active, sizeof(data.active)));
    active_dataset.user_context = NULL; //user defined
    active_dataset.user_tag = 0; //user defined

    EXOS_ASSERT_OK(exos_dataset_init(&countup, &datax, "countUp", &data.countUp, sizeof(data.countUp)));
    countup.user_context = NULL; //user defined
    countup.user_tag = 0; //user defined

    EXOS_ASSERT_OK(exos_dataset_init(&countdown, &datax, "countDown", &data.countDown, sizeof(data.countDown)));
    countdown.user_context = NULL; //user defined
    countdown.user_tag = 0; //user defined

    //connect the datamodel
    EXOS_ASSERT_OK(exos_datamodel_connect_datax(&datax, datamodelEvent));
    
    //connect datasets
    EXOS_ASSERT_OK(exos_dataset_connect(&enable_dataset, EXOS_DATASET_SUBSCRIBE, datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&active_dataset, EXOS_DATASET_PUBLISH, datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&countup, EXOS_DATASET_PUBLISH, datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&countdown, EXOS_DATASET_PUBLISH, datasetEvent));
    
    catch_termination();
    while (true)
    {
        EXOS_ASSERT_OK(exos_datamodel_process(&datax));
        exos_log_process(&logger);

        //put your cyclic code here!
		if (data.enable != data.active)
		{
			data.active = data.enable;
			exos_dataset_publish(&active_dataset);
		}

		if (data.enable)
		{
			data.countDown--;
			exos_dataset_publish(&countup);
			data.countUp++;
			exos_dataset_publish(&countdown);
		}

        if (is_terminated())
        {
            SUCCESS("DataX application terminated, closing..");
            break;
        }
    }


    EXOS_ASSERT_OK(exos_datamodel_delete(&datax));

    //finish with deleting the log
    exos_log_delete(&logger);
    return 0;
}
