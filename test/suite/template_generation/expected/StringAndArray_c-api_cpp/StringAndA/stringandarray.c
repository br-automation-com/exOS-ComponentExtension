#include <StringAndA.h>

#define EXOS_ASSERT_LOG &handle->logger
#define EXOS_ASSERT_CALLBACK inst->_state = 255;
#include "exos_log.h"
#include "exos_stringandarray.h"
#include <string.h>

#define SUCCESS(_format_, ...) exos_log_success(&handle->logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define INFO(_format_, ...) exos_log_info(&handle->logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define VERBOSE(_format_, ...) exos_log_debug(&handle->logger, EXOS_LOG_TYPE_USER + EXOS_LOG_TYPE_VERBOSE, _format_, ##__VA_ARGS__);
#define ERROR(_format_, ...) exos_log_error(&handle->logger, _format_, ##__VA_ARGS__);

typedef struct
{
    void *self;
    exos_log_handle_t logger;
    StringAndArray data;

    exos_datamodel_handle_t stringandarray;

    exos_dataset_handle_t myint1;
    exos_dataset_handle_t mystring;
    exos_dataset_handle_t myint2;
    exos_dataset_handle_t myintstruct;
    exos_dataset_handle_t myintstruct1;
    exos_dataset_handle_t myintstruct2;
    exos_dataset_handle_t myenum1;
} StringAndArrayHandle_t;

static void datasetEvent(exos_dataset_handle_t *dataset, EXOS_DATASET_EVENT_TYPE event_type, void *info)
{
    struct StringAndArrayCyclic *inst = (struct StringAndArrayCyclic *)dataset->datamodel->user_context;
    StringAndArrayHandle_t *handle = (StringAndArrayHandle_t *)inst->Handle;

    switch (event_type)
    {
    case EXOS_DATASET_EVENT_UPDATED:
        VERBOSE("dataset %s updated! latency (us):%i", dataset->name, (exos_datamodel_get_nettime(dataset->datamodel) - dataset->nettime));
        //handle each subscription dataset separately
        if(0 == strcmp(dataset->name, "MyInt2"))
        {
            memcpy(&inst->pStringAndArray->MyInt2, dataset->data, dataset->size);
        }
        else if(0 == strcmp(dataset->name, "MyIntStruct"))
        {
            memcpy(&inst->pStringAndArray->MyIntStruct, dataset->data, dataset->size);
        }
        else if(0 == strcmp(dataset->name, "MyIntStruct1"))
        {
            memcpy(&inst->pStringAndArray->MyIntStruct1, dataset->data, dataset->size);
        }
        else if(0 == strcmp(dataset->name, "MyIntStruct2"))
        {
            memcpy(&inst->pStringAndArray->MyIntStruct2, dataset->data, dataset->size);
        }
        else if(0 == strcmp(dataset->name, "MyEnum1"))
        {
            inst->pStringAndArray->MyEnum1 = *(Enum_enum *)dataset->data;
        }
        break;

    case EXOS_DATASET_EVENT_PUBLISHED:
        VERBOSE("dataset %s published to local server for distribution! send buffer free:%i", dataset->name, dataset->send_buffer.free);
        //handle each published dataset separately
        if(0 == strcmp(dataset->name, "MyInt1"))
        {
            // UDINT *myint1 = (UDINT *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "MyString"))
        {
            // STRING *mystring = (STRING *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "MyInt2"))
        {
            // USINT *myint2 = (USINT *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "MyIntStruct"))
        {
            // IntStruct_typ *myintstruct = (IntStruct_typ *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "MyIntStruct1"))
        {
            // IntStruct1_typ *myintstruct1 = (IntStruct1_typ *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "MyIntStruct2"))
        {
            // IntStruct2_typ *myintstruct2 = (IntStruct2_typ *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "MyEnum1"))
        {
            // Enum_enum *myenum1 = (Enum_enum *)dataset->data;
        }
        break;

    case EXOS_DATASET_EVENT_DELIVERED:
        VERBOSE("dataset %s delivered to remote server for distribution! send buffer free:%i", dataset->name, dataset->send_buffer.free);
        //handle each published dataset separately
        if(0 == strcmp(dataset->name, "MyInt1"))
        {
            // UDINT *myint1 = (UDINT *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "MyString"))
        {
            // STRING *mystring = (STRING *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "MyInt2"))
        {
            // USINT *myint2 = (USINT *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "MyIntStruct"))
        {
            // IntStruct_typ *myintstruct = (IntStruct_typ *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "MyIntStruct1"))
        {
            // IntStruct1_typ *myintstruct1 = (IntStruct1_typ *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "MyIntStruct2"))
        {
            // IntStruct2_typ *myintstruct2 = (IntStruct2_typ *)dataset->data;
        }
        else if(0 == strcmp(dataset->name, "MyEnum1"))
        {
            // Enum_enum *myenum1 = (Enum_enum *)dataset->data;
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
    struct StringAndArrayCyclic *inst = (struct StringAndArrayCyclic *)datamodel->user_context;
    StringAndArrayHandle_t *handle = (StringAndArrayHandle_t *)inst->Handle;

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
            SUCCESS("StringAndArray operational!");
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

_BUR_PUBLIC void StringAndArrayInit(struct StringAndArrayInit *inst)
{
    StringAndArrayHandle_t *handle;
    TMP_alloc(sizeof(StringAndArrayHandle_t), (void **)&handle);
    if (NULL == handle)
    {
        inst->Handle = 0;
        return;
    }

    memset(&handle->data, 0, sizeof(handle->data));
    handle->self = handle;

    exos_log_init(&handle->logger, "gStringAndArray_0");

    
    
    exos_datamodel_handle_t *stringandarray = &handle->stringandarray;
    exos_dataset_handle_t *myint1 = &handle->myint1;
    exos_dataset_handle_t *mystring = &handle->mystring;
    exos_dataset_handle_t *myint2 = &handle->myint2;
    exos_dataset_handle_t *myintstruct = &handle->myintstruct;
    exos_dataset_handle_t *myintstruct1 = &handle->myintstruct1;
    exos_dataset_handle_t *myintstruct2 = &handle->myintstruct2;
    exos_dataset_handle_t *myenum1 = &handle->myenum1;
    EXOS_ASSERT_OK(exos_datamodel_init(stringandarray, "StringAndArray_0", "gStringAndArray_0"));

    EXOS_ASSERT_OK(exos_dataset_init(myint1, stringandarray, "MyInt1", &handle->data.MyInt1, sizeof(handle->data.MyInt1)));
    EXOS_ASSERT_OK(exos_dataset_init(mystring, stringandarray, "MyString", &handle->data.MyString, sizeof(handle->data.MyString)));
    EXOS_ASSERT_OK(exos_dataset_init(myint2, stringandarray, "MyInt2", &handle->data.MyInt2, sizeof(handle->data.MyInt2)));
    EXOS_ASSERT_OK(exos_dataset_init(myintstruct, stringandarray, "MyIntStruct", &handle->data.MyIntStruct, sizeof(handle->data.MyIntStruct)));
    EXOS_ASSERT_OK(exos_dataset_init(myintstruct1, stringandarray, "MyIntStruct1", &handle->data.MyIntStruct1, sizeof(handle->data.MyIntStruct1)));
    EXOS_ASSERT_OK(exos_dataset_init(myintstruct2, stringandarray, "MyIntStruct2", &handle->data.MyIntStruct2, sizeof(handle->data.MyIntStruct2)));
    EXOS_ASSERT_OK(exos_dataset_init(myenum1, stringandarray, "MyEnum1", &handle->data.MyEnum1, sizeof(handle->data.MyEnum1)));
    
    inst->Handle = (UDINT)handle;
}

_BUR_PUBLIC void StringAndArrayCyclic(struct StringAndArrayCyclic *inst)
{
    StringAndArrayHandle_t *handle = (StringAndArrayHandle_t *)inst->Handle;

    inst->Error = false;
    if (NULL == handle || NULL == inst->pStringAndArray)
    {
        inst->Error = true;
        return;
    }
    if ((void *)handle != handle->self)
    {
        inst->Error = true;
        return;
    }

    StringAndArray *data = &handle->data;
    exos_datamodel_handle_t *stringandarray = &handle->stringandarray;
    //the user context of the datamodel points to the StringAndArrayCyclic instance
    stringandarray->user_context = inst; //set it cyclically in case the program using the FUB is retransferred
    stringandarray->user_tag = 0; //user defined
    //handle online download of the library
    if(NULL != stringandarray->datamodel_event_callback && stringandarray->datamodel_event_callback != datamodelEvent)
    {
        stringandarray->datamodel_event_callback = datamodelEvent;
        exos_log_delete(&handle->logger);
        exos_log_init(&handle->logger, "gStringAndArray_0");
    }

    exos_dataset_handle_t *myint1 = &handle->myint1;
    myint1->user_context = NULL; //user defined
    myint1->user_tag = 0; //user defined
    //handle online download of the library
    if(NULL != myint1->dataset_event_callback && myint1->dataset_event_callback != datasetEvent)
    {
        myint1->dataset_event_callback = datasetEvent;
    }

    exos_dataset_handle_t *mystring = &handle->mystring;
    mystring->user_context = NULL; //user defined
    mystring->user_tag = 0; //user defined
    //handle online download of the library
    if(NULL != mystring->dataset_event_callback && mystring->dataset_event_callback != datasetEvent)
    {
        mystring->dataset_event_callback = datasetEvent;
    }

    exos_dataset_handle_t *myint2 = &handle->myint2;
    myint2->user_context = NULL; //user defined
    myint2->user_tag = 0; //user defined
    //handle online download of the library
    if(NULL != myint2->dataset_event_callback && myint2->dataset_event_callback != datasetEvent)
    {
        myint2->dataset_event_callback = datasetEvent;
    }

    exos_dataset_handle_t *myintstruct = &handle->myintstruct;
    myintstruct->user_context = NULL; //user defined
    myintstruct->user_tag = 0; //user defined
    //handle online download of the library
    if(NULL != myintstruct->dataset_event_callback && myintstruct->dataset_event_callback != datasetEvent)
    {
        myintstruct->dataset_event_callback = datasetEvent;
    }

    exos_dataset_handle_t *myintstruct1 = &handle->myintstruct1;
    myintstruct1->user_context = NULL; //user defined
    myintstruct1->user_tag = 0; //user defined
    //handle online download of the library
    if(NULL != myintstruct1->dataset_event_callback && myintstruct1->dataset_event_callback != datasetEvent)
    {
        myintstruct1->dataset_event_callback = datasetEvent;
    }

    exos_dataset_handle_t *myintstruct2 = &handle->myintstruct2;
    myintstruct2->user_context = NULL; //user defined
    myintstruct2->user_tag = 0; //user defined
    //handle online download of the library
    if(NULL != myintstruct2->dataset_event_callback && myintstruct2->dataset_event_callback != datasetEvent)
    {
        myintstruct2->dataset_event_callback = datasetEvent;
    }

    exos_dataset_handle_t *myenum1 = &handle->myenum1;
    myenum1->user_context = NULL; //user defined
    myenum1->user_tag = 0; //user defined
    //handle online download of the library
    if(NULL != myenum1->dataset_event_callback && myenum1->dataset_event_callback != datasetEvent)
    {
        myenum1->dataset_event_callback = datasetEvent;
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

        SUCCESS("starting StringAndArray application..");

        //connect the datamodel, then the datasets
        EXOS_ASSERT_OK(exos_datamodel_connect_stringandarray(stringandarray, datamodelEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(myint1, EXOS_DATASET_PUBLISH, datasetEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(mystring, EXOS_DATASET_PUBLISH, datasetEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(myint2, EXOS_DATASET_PUBLISH + EXOS_DATASET_SUBSCRIBE, datasetEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(myintstruct, EXOS_DATASET_PUBLISH + EXOS_DATASET_SUBSCRIBE, datasetEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(myintstruct1, EXOS_DATASET_PUBLISH + EXOS_DATASET_SUBSCRIBE, datasetEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(myintstruct2, EXOS_DATASET_PUBLISH + EXOS_DATASET_SUBSCRIBE, datasetEvent));
        EXOS_ASSERT_OK(exos_dataset_connect(myenum1, EXOS_DATASET_PUBLISH + EXOS_DATASET_SUBSCRIBE, datasetEvent));

        inst->Active = true;
        break;

    case 100:
    case 101:
        if (inst->Start)
        {
            if (inst->_state == 100)
            {
                EXOS_ASSERT_OK(exos_datamodel_set_operational(stringandarray));
                inst->_state = 101;
            }
        }
        else
        {
            inst->_state = 100;
        }

        EXOS_ASSERT_OK(exos_datamodel_process(stringandarray));
        //put your cyclic code here!

        //publish the myint1 dataset as soon as there are changes
        if (inst->pStringAndArray->MyInt1 != data->MyInt1)
        {
            data->MyInt1 = inst->pStringAndArray->MyInt1;
            exos_dataset_publish(myint1);
        }
        //publish the mystring dataset as soon as there are changes
        if (0 != memcmp(&inst->pStringAndArray->MyString, &data->MyString, sizeof(data->MyString)))
        {
            memcpy(&data->MyString, &inst->pStringAndArray->MyString, sizeof(data->MyString));
            exos_dataset_publish(mystring);
        }
        //publish the myint2 dataset as soon as there are changes
        if (0 != memcmp(&inst->pStringAndArray->MyInt2, &data->MyInt2, sizeof(data->MyInt2)))
        {
            memcpy(&data->MyInt2, &inst->pStringAndArray->MyInt2, sizeof(data->MyInt2));
            exos_dataset_publish(myint2);
        }
        //publish the myintstruct dataset as soon as there are changes
        if (0 != memcmp(&inst->pStringAndArray->MyIntStruct, &data->MyIntStruct, sizeof(data->MyIntStruct)))
        {
            memcpy(&data->MyIntStruct, &inst->pStringAndArray->MyIntStruct, sizeof(data->MyIntStruct));
            exos_dataset_publish(myintstruct);
        }
        //publish the myintstruct1 dataset as soon as there are changes
        if (0 != memcmp(&inst->pStringAndArray->MyIntStruct1, &data->MyIntStruct1, sizeof(data->MyIntStruct1)))
        {
            memcpy(&data->MyIntStruct1, &inst->pStringAndArray->MyIntStruct1, sizeof(data->MyIntStruct1));
            exos_dataset_publish(myintstruct1);
        }
        //publish the myintstruct2 dataset as soon as there are changes
        if (0 != memcmp(&inst->pStringAndArray->MyIntStruct2, &data->MyIntStruct2, sizeof(data->MyIntStruct2)))
        {
            memcpy(&data->MyIntStruct2, &inst->pStringAndArray->MyIntStruct2, sizeof(data->MyIntStruct2));
            exos_dataset_publish(myintstruct2);
        }
        //publish the myenum1 dataset as soon as there are changes
        if (inst->pStringAndArray->MyEnum1 != data->MyEnum1)
        {
            data->MyEnum1 = inst->pStringAndArray->MyEnum1;
            exos_dataset_publish(myenum1);
        }

        break;

    case 255:
        //disconnect the datamodel
        EXOS_ASSERT_OK(exos_datamodel_disconnect(stringandarray));

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

_BUR_PUBLIC void StringAndArrayExit(struct StringAndArrayExit *inst)
{
    StringAndArrayHandle_t *handle = (StringAndArrayHandle_t *)inst->Handle;

    if (NULL == handle)
    {
        ERROR("StringAndArrayExit: NULL handle, cannot delete resources");
        return;
    }
    if ((void *)handle != handle->self)
    {
        ERROR("StringAndArrayExit: invalid handle, cannot delete resources");
        return;
    }

    exos_datamodel_handle_t *stringandarray = &handle->stringandarray;

    EXOS_ASSERT_OK(exos_datamodel_delete(stringandarray));

    //finish with deleting the log
    exos_log_delete(&handle->logger);
    //free the allocated handle
    TMP_free(sizeof(StringAndArrayHandle_t), (void *)handle);
}

