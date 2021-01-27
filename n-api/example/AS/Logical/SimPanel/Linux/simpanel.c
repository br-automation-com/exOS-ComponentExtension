#include <unistd.h>
#include <string.h>
#include "termination.h"

#define EXOS_ASSERT_LOG &logger
#include "exos_log.h"
#include "exos_simpanel.h"

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
        if(0 == strcmp(dataset->name,"Knobs"))
        {
            SimPanelKnobs *knobs = (SimPanelKnobs *)dataset->data;
        }
        else if(0 == strcmp(dataset->name,"Encoder"))
        {
            uint16_t *encoder = (uint16_t *)dataset->data;
        }
        break;

    case EXOS_DATASET_EVENT_PUBLISHED:
        VERBOSE("dataset %s published to local server for distribution! send buffer free:%i", dataset->name, dataset->send_buffer.free);
        //handle each published dataset separately
        if(0 == strcmp(dataset->name, "Knobs"))
        {
            SimPanelKnobs *knobs = (SimPanelKnobs *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "Display"))
        {
            int16_t *display = (int16_t *)dataset->data;
        }
        break;

    case EXOS_DATASET_EVENT_DELIVERED:
        VERBOSE("dataset %s delivered to remote server for distribution! send buffer free:%i", dataset->name, dataset->send_buffer.free);
        //handle each published dataset separately
        if(0 == strcmp(dataset->name, "Knobs"))
        {
            SimPanelKnobs *knobs = (SimPanelKnobs *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "Display"))
        {
            int16_t *display = (int16_t *)dataset->data;
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
            SUCCESS("SimPanel operational!");
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
	SimPanel data;

	exos_datamodel_handle_t simpanel;
	uint32_t increase;
	
	exos_dataset_handle_t knobs;
	exos_dataset_handle_t display;
	exos_dataset_handle_t encoder;
    
	exos_log_init(&logger, "SimPanel_Linux");

	SUCCESS("starting SimPanel application..");

	EXOS_ASSERT_OK(exos_datamodel_init(&simpanel, "SimPanel", "SimPanel_Linux"));

	//set the user_context to access custom data in the callbacks
	simpanel.user_context = NULL; //user defined
	simpanel.user_tag = 0; //user defined

	EXOS_ASSERT_OK(exos_dataset_init(&knobs, &simpanel, "Knobs", &data.Knobs, sizeof(data.Knobs)));
	knobs.user_context = NULL; //user defined
	knobs.user_tag = 0; //user defined

	EXOS_ASSERT_OK(exos_dataset_init(&display, &simpanel, "Display", &data.Display, sizeof(data.Display)));
	display.user_context = NULL; //user defined
	display.user_tag = 0; //user defined

	EXOS_ASSERT_OK(exos_dataset_init(&encoder, &simpanel, "Encoder", &data.Encoder, sizeof(data.Encoder)));
	encoder.user_context = NULL; //user defined
	encoder.user_tag = 0; //user defined

	//connect the datamodel
	EXOS_ASSERT_OK(exos_datamodel_connect_simpanel(&simpanel, datamodelEvent));
    
	//connect datasets
	EXOS_ASSERT_OK(exos_dataset_connect(&knobs, EXOS_DATASET_PUBLISH + EXOS_DATASET_SUBSCRIBE, datasetEvent));
	EXOS_ASSERT_OK(exos_dataset_connect(&display, EXOS_DATASET_PUBLISH, datasetEvent));
	EXOS_ASSERT_OK(exos_dataset_connect(&encoder, EXOS_DATASET_SUBSCRIBE, datasetEvent));
    
	catch_termination();
	while (true)
	{
		EXOS_ASSERT_OK(exos_datamodel_process(&simpanel));
		exos_log_process(&logger);

		//put your cyclic code here!
		increase++;
		if (200 < increase) 
		{
			increase = 0;
			data.Display++;
			exos_dataset_publish(&display);
		}
		
		if (is_terminated())
		{
			SUCCESS("SimPanel application terminated, closing..");
			break;
		}
	}


	EXOS_ASSERT_OK(exos_datamodel_delete(&simpanel));

	//finish with deleting the log
	exos_log_delete(&logger);
	return 0;
}
