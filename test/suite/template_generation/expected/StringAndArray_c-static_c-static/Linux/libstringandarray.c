#include <string.h>
#define EXOS_ASSERT_LOG &logger
#include "exos_log.h"
#include "libstringandarray.h"

#define SUCCESS(_format_, ...) exos_log_success(&logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define INFO(_format_, ...) exos_log_info(&logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
#define VERBOSE(_format_, ...) exos_log_debug(&logger, EXOS_LOG_TYPE_USER + EXOS_LOG_TYPE_VERBOSE, _format_, ##__VA_ARGS__);
#define ERROR(_format_, ...) exos_log_error(&logger, _format_, ##__VA_ARGS__);

static exos_log_handle_t logger;

typedef struct libStringAndArrayHandle
{
    libStringAndArray_t ext_stringandarray;
    exos_datamodel_handle_t stringandarray;

    exos_dataset_handle_t myint1;
    exos_dataset_handle_t mystring;
    exos_dataset_handle_t myint2;
    exos_dataset_handle_t myintstruct;
    exos_dataset_handle_t myintstruct1;
    exos_dataset_handle_t myintstruct2;
    exos_dataset_handle_t myenum1;
} libStringAndArrayHandle_t;

static libStringAndArrayHandle_t h_StringAndArray;

static void libStringAndArray_datasetEvent(exos_dataset_handle_t *dataset, EXOS_DATASET_EVENT_TYPE event_type, void *info)
{
    switch (event_type)
    {
    case EXOS_DATASET_EVENT_UPDATED:
        VERBOSE("dataset %s updated! latency (us):%i", dataset->name, (exos_datamodel_get_nettime(dataset->datamodel) - dataset->nettime));
        //handle each subscription dataset separately
        if (0 == strcmp(dataset->name, "MyInt1"))
        {
            //update the nettime
            h_StringAndArray.ext_stringandarray.MyInt1.nettime = dataset->nettime;

            //trigger the callback if assigned
            if (NULL != h_StringAndArray.ext_stringandarray.MyInt1.on_change)
            {
                h_StringAndArray.ext_stringandarray.MyInt1.on_change();
            }
        }
        else if (0 == strcmp(dataset->name, "MyString"))
        {
            //update the nettime
            h_StringAndArray.ext_stringandarray.MyString.nettime = dataset->nettime;

            //trigger the callback if assigned
            if (NULL != h_StringAndArray.ext_stringandarray.MyString.on_change)
            {
                h_StringAndArray.ext_stringandarray.MyString.on_change();
            }
        }
        else if (0 == strcmp(dataset->name, "MyInt2"))
        {
            //update the nettime
            h_StringAndArray.ext_stringandarray.MyInt2.nettime = dataset->nettime;

            //trigger the callback if assigned
            if (NULL != h_StringAndArray.ext_stringandarray.MyInt2.on_change)
            {
                h_StringAndArray.ext_stringandarray.MyInt2.on_change();
            }
        }
        else if (0 == strcmp(dataset->name, "MyIntStruct"))
        {
            //update the nettime
            h_StringAndArray.ext_stringandarray.MyIntStruct.nettime = dataset->nettime;

            //trigger the callback if assigned
            if (NULL != h_StringAndArray.ext_stringandarray.MyIntStruct.on_change)
            {
                h_StringAndArray.ext_stringandarray.MyIntStruct.on_change();
            }
        }
        else if (0 == strcmp(dataset->name, "MyIntStruct1"))
        {
            //update the nettime
            h_StringAndArray.ext_stringandarray.MyIntStruct1.nettime = dataset->nettime;

            //trigger the callback if assigned
            if (NULL != h_StringAndArray.ext_stringandarray.MyIntStruct1.on_change)
            {
                h_StringAndArray.ext_stringandarray.MyIntStruct1.on_change();
            }
        }
        else if (0 == strcmp(dataset->name, "MyIntStruct2"))
        {
            //update the nettime
            h_StringAndArray.ext_stringandarray.MyIntStruct2.nettime = dataset->nettime;

            //trigger the callback if assigned
            if (NULL != h_StringAndArray.ext_stringandarray.MyIntStruct2.on_change)
            {
                h_StringAndArray.ext_stringandarray.MyIntStruct2.on_change();
            }
        }
        else if (0 == strcmp(dataset->name, "MyEnum1"))
        {
            //update the nettime
            h_StringAndArray.ext_stringandarray.MyEnum1.nettime = dataset->nettime;

            //trigger the callback if assigned
            if (NULL != h_StringAndArray.ext_stringandarray.MyEnum1.on_change)
            {
                h_StringAndArray.ext_stringandarray.MyEnum1.on_change();
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

static void libStringAndArray_datamodelEvent(exos_datamodel_handle_t *datamodel, const EXOS_DATAMODEL_EVENT_TYPE event_type, void *info)
{
    switch (event_type)
    {
    case EXOS_DATAMODEL_EVENT_CONNECTION_CHANGED:
        INFO("application changed state to %s", exos_get_state_string(datamodel->connection_state));

        h_StringAndArray.ext_stringandarray.is_connected = false;
        h_StringAndArray.ext_stringandarray.is_operational = false;
        switch (datamodel->connection_state)
        {
        case EXOS_STATE_DISCONNECTED:
            if (NULL != h_StringAndArray.ext_stringandarray.on_disconnected)
            {
                h_StringAndArray.ext_stringandarray.on_disconnected();
            }
            break;
        case EXOS_STATE_CONNECTED:
            h_StringAndArray.ext_stringandarray.is_connected = true;
            if (NULL != h_StringAndArray.ext_stringandarray.on_connected)
            {
                h_StringAndArray.ext_stringandarray.on_connected();
            }
            break;
        case EXOS_STATE_OPERATIONAL:
            h_StringAndArray.ext_stringandarray.is_connected = true;
            h_StringAndArray.ext_stringandarray.is_operational = true;
            if (NULL != h_StringAndArray.ext_stringandarray.on_operational)
            {
                h_StringAndArray.ext_stringandarray.on_operational();
            }
            SUCCESS("StringAndArray operational!");
            break;
        case EXOS_STATE_ABORTED:
            if (NULL != h_StringAndArray.ext_stringandarray.on_disconnected)
            {
                h_StringAndArray.ext_stringandarray.on_disconnected();
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

static void libStringAndArray_publish_myint2(void)
{
    EXOS_ASSERT_OK(exos_dataset_publish(&h_StringAndArray.myint2));
}
static void libStringAndArray_publish_myintstruct(void)
{
    EXOS_ASSERT_OK(exos_dataset_publish(&h_StringAndArray.myintstruct));
}
static void libStringAndArray_publish_myintstruct1(void)
{
    EXOS_ASSERT_OK(exos_dataset_publish(&h_StringAndArray.myintstruct1));
}
static void libStringAndArray_publish_myintstruct2(void)
{
    EXOS_ASSERT_OK(exos_dataset_publish(&h_StringAndArray.myintstruct2));
}
static void libStringAndArray_publish_myenum1(void)
{
    EXOS_ASSERT_OK(exos_dataset_publish(&h_StringAndArray.myenum1));
}

static void libStringAndArray_connect(void)
{
    //connect the datamodel
    EXOS_ASSERT_OK(exos_datamodel_connect_stringandarray(&(h_StringAndArray.stringandarray), libStringAndArray_datamodelEvent));
    
    //connect datasets
    EXOS_ASSERT_OK(exos_dataset_connect(&(h_StringAndArray.myint1), EXOS_DATASET_SUBSCRIBE, libStringAndArray_datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&(h_StringAndArray.mystring), EXOS_DATASET_SUBSCRIBE, libStringAndArray_datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&(h_StringAndArray.myint2), EXOS_DATASET_PUBLISH + EXOS_DATASET_SUBSCRIBE, libStringAndArray_datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&(h_StringAndArray.myintstruct), EXOS_DATASET_PUBLISH + EXOS_DATASET_SUBSCRIBE, libStringAndArray_datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&(h_StringAndArray.myintstruct1), EXOS_DATASET_PUBLISH + EXOS_DATASET_SUBSCRIBE, libStringAndArray_datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&(h_StringAndArray.myintstruct2), EXOS_DATASET_PUBLISH + EXOS_DATASET_SUBSCRIBE, libStringAndArray_datasetEvent));
    EXOS_ASSERT_OK(exos_dataset_connect(&(h_StringAndArray.myenum1), EXOS_DATASET_PUBLISH + EXOS_DATASET_SUBSCRIBE, libStringAndArray_datasetEvent));
}
static void libStringAndArray_disconnect(void)
{
    h_StringAndArray.ext_stringandarray.is_connected = false;
    h_StringAndArray.ext_stringandarray.is_operational = false;

    EXOS_ASSERT_OK(exos_datamodel_disconnect(&(h_StringAndArray.stringandarray)));
}

static void libStringAndArray_set_operational(void)
{
    EXOS_ASSERT_OK(exos_datamodel_set_operational(&(h_StringAndArray.stringandarray)));
}

static void libStringAndArray_process(void)
{
    EXOS_ASSERT_OK(exos_datamodel_process(&(h_StringAndArray.stringandarray)));
    exos_log_process(&logger);
}

static void libStringAndArray_dispose(void)
{
    h_StringAndArray.ext_stringandarray.is_connected = false;
    h_StringAndArray.ext_stringandarray.is_operational = false;

    EXOS_ASSERT_OK(exos_datamodel_delete(&(h_StringAndArray.stringandarray)));
    exos_log_delete(&logger);
}

static int32_t libStringAndArray_get_nettime(void)
{
    return exos_datamodel_get_nettime(&(h_StringAndArray.stringandarray));
}

static void libStringAndArray_log_error(char* log_entry)
{
    exos_log_error(&logger, log_entry);
}

static void libStringAndArray_log_warning(char* log_entry)
{
    exos_log_warning(&logger, EXOS_LOG_TYPE_USER, log_entry);
}

static void libStringAndArray_log_success(char* log_entry)
{
    exos_log_success(&logger, EXOS_LOG_TYPE_USER, log_entry);
}

static void libStringAndArray_log_info(char* log_entry)
{
    exos_log_info(&logger, EXOS_LOG_TYPE_USER, log_entry);
}

static void libStringAndArray_log_debug(char* log_entry)
{
    exos_log_debug(&logger, EXOS_LOG_TYPE_USER, log_entry);
}

static void libStringAndArray_log_verbose(char* log_entry)
{
    exos_log_warning(&logger, EXOS_LOG_TYPE_USER + EXOS_LOG_TYPE_VERBOSE, log_entry);
}

libStringAndArray_t *libStringAndArray_init(void)
{
    memset(&h_StringAndArray, 0, sizeof(h_StringAndArray));

    h_StringAndArray.ext_stringandarray.MyInt2.publish = libStringAndArray_publish_myint2;
    h_StringAndArray.ext_stringandarray.MyIntStruct.publish = libStringAndArray_publish_myintstruct;
    h_StringAndArray.ext_stringandarray.MyIntStruct1.publish = libStringAndArray_publish_myintstruct1;
    h_StringAndArray.ext_stringandarray.MyIntStruct2.publish = libStringAndArray_publish_myintstruct2;
    h_StringAndArray.ext_stringandarray.MyEnum1.publish = libStringAndArray_publish_myenum1;
    
    h_StringAndArray.ext_stringandarray.connect = libStringAndArray_connect;
    h_StringAndArray.ext_stringandarray.disconnect = libStringAndArray_disconnect;
    h_StringAndArray.ext_stringandarray.process = libStringAndArray_process;
    h_StringAndArray.ext_stringandarray.set_operational = libStringAndArray_set_operational;
    h_StringAndArray.ext_stringandarray.dispose = libStringAndArray_dispose;
    h_StringAndArray.ext_stringandarray.get_nettime = libStringAndArray_get_nettime;
    h_StringAndArray.ext_stringandarray.log.error = libStringAndArray_log_error;
    h_StringAndArray.ext_stringandarray.log.warning = libStringAndArray_log_warning;
    h_StringAndArray.ext_stringandarray.log.success = libStringAndArray_log_success;
    h_StringAndArray.ext_stringandarray.log.info = libStringAndArray_log_info;
    h_StringAndArray.ext_stringandarray.log.debug = libStringAndArray_log_debug;
    h_StringAndArray.ext_stringandarray.log.verbose = libStringAndArray_log_verbose;
    
    exos_log_init(&logger, "gStringAndArray_0");

    SUCCESS("starting gStringAndArray_0 application..");

    EXOS_ASSERT_OK(exos_datamodel_init(&h_StringAndArray.stringandarray, "StringAndArray_0", "gStringAndArray_0"));

    //set the user_context to access custom data in the callbacks
    h_StringAndArray.stringandarray.user_context = NULL; //not used
    h_StringAndArray.stringandarray.user_tag = 0; //not used

    EXOS_ASSERT_OK(exos_dataset_init(&h_StringAndArray.myint1, &h_StringAndArray.stringandarray, "MyInt1", &h_StringAndArray.ext_stringandarray.MyInt1.value, sizeof(h_StringAndArray.ext_stringandarray.MyInt1.value)));
    h_StringAndArray.myint1.user_context = NULL; //not used
    h_StringAndArray.myint1.user_tag = 0; //not used

    EXOS_ASSERT_OK(exos_dataset_init(&h_StringAndArray.mystring, &h_StringAndArray.stringandarray, "MyString", &h_StringAndArray.ext_stringandarray.MyString.value, sizeof(h_StringAndArray.ext_stringandarray.MyString.value)));
    h_StringAndArray.mystring.user_context = NULL; //not used
    h_StringAndArray.mystring.user_tag = 0; //not used

    EXOS_ASSERT_OK(exos_dataset_init(&h_StringAndArray.myint2, &h_StringAndArray.stringandarray, "MyInt2", &h_StringAndArray.ext_stringandarray.MyInt2.value, sizeof(h_StringAndArray.ext_stringandarray.MyInt2.value)));
    h_StringAndArray.myint2.user_context = NULL; //not used
    h_StringAndArray.myint2.user_tag = 0; //not used

    EXOS_ASSERT_OK(exos_dataset_init(&h_StringAndArray.myintstruct, &h_StringAndArray.stringandarray, "MyIntStruct", &h_StringAndArray.ext_stringandarray.MyIntStruct.value, sizeof(h_StringAndArray.ext_stringandarray.MyIntStruct.value)));
    h_StringAndArray.myintstruct.user_context = NULL; //not used
    h_StringAndArray.myintstruct.user_tag = 0; //not used

    EXOS_ASSERT_OK(exos_dataset_init(&h_StringAndArray.myintstruct1, &h_StringAndArray.stringandarray, "MyIntStruct1", &h_StringAndArray.ext_stringandarray.MyIntStruct1.value, sizeof(h_StringAndArray.ext_stringandarray.MyIntStruct1.value)));
    h_StringAndArray.myintstruct1.user_context = NULL; //not used
    h_StringAndArray.myintstruct1.user_tag = 0; //not used

    EXOS_ASSERT_OK(exos_dataset_init(&h_StringAndArray.myintstruct2, &h_StringAndArray.stringandarray, "MyIntStruct2", &h_StringAndArray.ext_stringandarray.MyIntStruct2.value, sizeof(h_StringAndArray.ext_stringandarray.MyIntStruct2.value)));
    h_StringAndArray.myintstruct2.user_context = NULL; //not used
    h_StringAndArray.myintstruct2.user_tag = 0; //not used

    EXOS_ASSERT_OK(exos_dataset_init(&h_StringAndArray.myenum1, &h_StringAndArray.stringandarray, "MyEnum1", &h_StringAndArray.ext_stringandarray.MyEnum1.value, sizeof(h_StringAndArray.ext_stringandarray.MyEnum1.value)));
    h_StringAndArray.myenum1.user_context = NULL; //not used
    h_StringAndArray.myenum1.user_tag = 0; //not used

    return &(h_StringAndArray.ext_stringandarray);
}
