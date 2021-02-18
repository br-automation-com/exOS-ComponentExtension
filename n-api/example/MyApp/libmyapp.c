#define NAPI_VERSION 6
#include <node_api.h>
#include <stdint.h>
#include <exos_api.h>
#include "exos_myapp.h"
#include <uv.h>
#include <unistd.h>
#include <string.h>

#define BUR_NAPI_DEFAULT_BOOL_INIT false
#define BUR_NAPI_DEFAULT_NUM_INIT 0
#define BUR_NAPI_DEFAULT_STRING_INIT ""

typedef struct
{
    napi_ref ref;
    uint32_t ref_count;
    napi_threadsafe_function onchange_cb;
    napi_threadsafe_function connectiononchange_cb;
    napi_value object_value; //volatile placeholder.
    napi_value value;        //volatile placeholder.
} obj_handles;

obj_handles myapp = {};
obj_handles execute = {};
obj_handles done = {};
obj_handles message = {};
obj_handles parameters = {};
obj_handles results = {};

napi_deferred deferred = NULL;
uv_idle_t cyclic_h;

MyApp exos_data = {};
exos_dataset_handle_t exos_execute;
exos_dataset_handle_t exos_done;
exos_dataset_handle_t exos_message;
exos_dataset_handle_t exos_parameters;
exos_dataset_handle_t exos_results;
exos_datamodel_handle_t exos_myapp;

// exOS call backs
static void datasetEvent(exos_dataset_handle_t *dataset, EXOS_DATASET_EVENT_TYPE event_type, void *info)
{
    switch (event_type)
    {
    case EXOS_DATASET_EVENT_UPDATED:
        //handle each subscription dataset separately
        if (0 == strcmp(dataset->name, "execute"))
        {
            exos_data.execute = *(bool *)dataset->data;

            if (execute.onchange_cb != NULL)
            {
                napi_acquire_threadsafe_function(execute.onchange_cb);
                napi_call_threadsafe_function(execute.onchange_cb, &exos_data.execute, napi_tsfn_blocking);
                napi_release_threadsafe_function(execute.onchange_cb, napi_tsfn_release);
            }
        }
        else if (0 == strcmp(dataset->name, "done"))
        {
            exos_data.done = *(bool *)dataset->data;

            if (done.onchange_cb != NULL)
            {
                napi_acquire_threadsafe_function(done.onchange_cb);
                napi_call_threadsafe_function(done.onchange_cb, &exos_data.done, napi_tsfn_blocking);
                napi_release_threadsafe_function(done.onchange_cb, napi_tsfn_release);
            }
        }
        else if (0 == strcmp(dataset->name, "message"))
        {
            memcpy(&exos_data.message, dataset->data, sizeof(exos_data.message));
            //truncate string to 20 chars since memcpy do not check for null char.
            char *p = (char *)&exos_data.message;
            p = p + sizeof(exos_data.message) - 1;
            *p = '\0';

            if (message.onchange_cb != NULL)
            {
                napi_acquire_threadsafe_function(message.onchange_cb);
                napi_call_threadsafe_function(message.onchange_cb, &exos_data.message, napi_tsfn_blocking);
                napi_release_threadsafe_function(message.onchange_cb, napi_tsfn_release);
            }
        }
        else if (0 == strcmp(dataset->name, "parameters"))
        {
            memcpy(&exos_data.parameters, dataset->data, sizeof(exos_data.parameters));

            if (parameters.onchange_cb != NULL)
            {
                napi_acquire_threadsafe_function(parameters.onchange_cb);
                napi_call_threadsafe_function(parameters.onchange_cb, &exos_data.parameters, napi_tsfn_blocking);
                napi_release_threadsafe_function(parameters.onchange_cb, napi_tsfn_release);
            }
        }
        break;

    case EXOS_DATASET_EVENT_PUBLISHED:
    {
        //handle each published dataset separately
        if (0 == strcmp(dataset->name, "done"))
        {
            //bool *done = (bool *)dataset->data;
        }
        else if (0 == strcmp(dataset->name, "message"))
        {
            //char *parameters = (char *)dataset->data;
        }
        else if (0 == strcmp(dataset->name, "parameters"))
        {
            //(MyAppPar_t[10]) *parameters = (MyAppPar_t[10] *)dataset->data;
        }
        break;
    }

    case EXOS_DATASET_EVENT_DELIVERED:
    {
        //handle each published dataset separately
        if (0 == strcmp(dataset->name, "done"))
        {
            //bool *done = (bool *)dataset->data;
        }
        else if (0 == strcmp(dataset->name, "message"))
        {
            //char *parameters = (char *)dataset->data;
        }
        else if (0 == strcmp(dataset->name, "parameters"))
        {
            //(MyAppPar_t[10]) *parameters = (MyAppPar_t[10] *)dataset->data;
        }
        break;
    }

    case EXOS_DATASET_EVENT_CONNECTION_CHANGED:
        if (0 == strcmp(dataset->name, "execute"))
        {
            if (execute.connectiononchange_cb != NULL)
            {
                napi_acquire_threadsafe_function(execute.connectiononchange_cb);
                napi_call_threadsafe_function(execute.connectiononchange_cb, exos_get_state_string(dataset->connection_state), napi_tsfn_blocking);
                napi_release_threadsafe_function(execute.connectiononchange_cb, napi_tsfn_release);
            }
        }
        else if (0 == strcmp(dataset->name, "done"))
        {
            if (done.connectiononchange_cb != NULL)
            {
                napi_acquire_threadsafe_function(done.connectiononchange_cb);
                napi_call_threadsafe_function(done.connectiononchange_cb, exos_get_state_string(dataset->connection_state), napi_tsfn_blocking);
                napi_release_threadsafe_function(done.connectiononchange_cb, napi_tsfn_release);
            }
        }
        else if (0 == strcmp(dataset->name, "message"))
        {
            if (done.connectiononchange_cb != NULL)
            {
                napi_acquire_threadsafe_function(message.connectiononchange_cb);
                napi_call_threadsafe_function(message.connectiononchange_cb, exos_get_state_string(dataset->connection_state), napi_tsfn_blocking);
                napi_release_threadsafe_function(message.connectiononchange_cb, napi_tsfn_release);
            }
        }
        else if (0 == strcmp(dataset->name, "parameters"))
        {
            if (parameters.connectiononchange_cb != NULL)
            {
                napi_acquire_threadsafe_function(parameters.connectiononchange_cb);
                napi_call_threadsafe_function(parameters.connectiononchange_cb, exos_get_state_string(dataset->connection_state), napi_tsfn_blocking);
                napi_release_threadsafe_function(parameters.connectiononchange_cb, napi_tsfn_release);
            }
        }
        else if (0 == strcmp(dataset->name, "results"))
        {
            if (results.connectiononchange_cb != NULL)
            {
                napi_acquire_threadsafe_function(results.connectiononchange_cb);
                napi_call_threadsafe_function(results.connectiononchange_cb, exos_get_state_string(dataset->connection_state), napi_tsfn_blocking);
                napi_release_threadsafe_function(results.connectiononchange_cb, napi_tsfn_release);
            }
        }

        switch (dataset->connection_state)
        {
        case EXOS_STATE_DISCONNECTED:
            break;
        case EXOS_STATE_CONNECTED:
            //call the dataset changed event to update the dataset when connected
            //datasetEvent(dataset,EXOS_DATASET_EVENT_UPDATED,info);
            break;
        case EXOS_STATE_OPERATIONAL:
            break;
        case EXOS_STATE_ABORTED:
            break;
        }
        break;
    }
}

static void datamodelEvent(exos_datamodel_handle_t *datamodel, const EXOS_DATAMODEL_EVENT_TYPE event_type, void *info)
{
    switch (event_type)
    {
    case EXOS_DATASET_EVENT_CONNECTION_CHANGED:
        if (myapp.connectiononchange_cb != NULL)
        {
            napi_acquire_threadsafe_function(myapp.connectiononchange_cb);
            napi_call_threadsafe_function(myapp.connectiononchange_cb, exos_get_state_string(datamodel->connection_state), napi_tsfn_blocking);
            napi_release_threadsafe_function(myapp.connectiononchange_cb, napi_tsfn_release);
        }

        switch (datamodel->connection_state)
        {
        case EXOS_STATE_DISCONNECTED:
            break;
        case EXOS_STATE_CONNECTED:
            break;
        case EXOS_STATE_OPERATIONAL:
            break;
        case EXOS_STATE_ABORTED:
            break;
        }
        break;
    }
}

// napi callback setup main function
napi_value init_napi_onchange(napi_env env, napi_callback_info info, const char *identifier, napi_threadsafe_function_call_js call_js_cb, napi_threadsafe_function *result)
{
    size_t argc = 1;
    napi_value argv[1];

    if (napi_ok != napi_get_cb_info(env, info, &argc, argv, NULL, NULL))
    {
        char msg[100] = {};
        strcpy(msg, "init_napi_onchange() napi_get_cb_info failed - ");
        strcat(msg, identifier);
        napi_throw_error(env, "EINVAL", msg);
        return NULL;
    }

    if (argc < 1)
    {
        napi_throw_error(env, "EINVAL", "Too few arguments");
        return NULL;
    }

    napi_value work_name;
    if (napi_ok != napi_create_string_utf8(env, identifier, NAPI_AUTO_LENGTH, &work_name))
    {
        char msg[100] = {};
        strcpy(msg, "init_napi_onchange() napi_create_string_utf8 failed - ");
        strcat(msg, identifier);
        napi_throw_error(env, "EINVAL", msg);
        return NULL;
    }

    napi_valuetype cb_typ;
    if (napi_ok != napi_typeof(env, argv[0], &cb_typ))
    {
        char msg[100] = {};
        strcpy(msg, "init_napi_onchange() napi_typeof failed - ");
        strcat(msg, identifier);
        napi_throw_error(env, "EINVAL", msg);
        return NULL;
    }

    if (cb_typ == napi_function)
    {
        if (napi_ok != napi_create_threadsafe_function(env, argv[0], NULL, work_name, 0, 1, NULL, NULL, NULL, call_js_cb, result))
        {
            const napi_extended_error_info *info;
            napi_get_last_error_info(env, &info);

            napi_throw_error(env, NULL, info->error_message);
            return NULL;
        }
    }
    return NULL;
}

// js object callbacks
static void myapp_connonchange_js_cb(napi_env env, napi_value js_cb, void *context, void *data)
{
    const char *string = data;
    napi_value undefined;

    napi_get_undefined(env, &undefined);

    if (napi_ok != napi_create_string_utf8(env, string, strlen(string), &myapp.value))
        napi_throw_error(env, "EINVAL", "Can't create utf8 string from char*");

    if (napi_ok != napi_get_reference_value(env, myapp.ref, &myapp.object_value))
        napi_throw_error(env, "EINVAL", "Can't get reference");

    if (napi_ok != napi_set_named_property(env, myapp.object_value, "connectionState", myapp.value))
        napi_throw_error(env, "EINVAL", "Can't set \"connectionState\" property");

    if (napi_ok != napi_call_function(env, undefined, js_cb, 0, NULL, NULL))
        napi_throw_error(env, "EINVAL", "Can't call \"connectionOnChange\" callback");
}

static void execute_connonchange_js_cb(napi_env env, napi_value js_cb, void *context, void *data)
{
    const char *string = data;
    napi_value undefined;

    napi_get_undefined(env, &undefined);

    if (napi_ok != napi_create_string_utf8(env, string, strlen(string), &execute.value))
        napi_throw_error(env, "EINVAL", "Can't create utf8 string from char*");

    if (napi_ok != napi_get_reference_value(env, execute.ref, &execute.object_value))
        napi_throw_error(env, "EINVAL", "Can't get reference");

    if (napi_ok != napi_set_named_property(env, execute.object_value, "connectionState", execute.value))
        napi_throw_error(env, "EINVAL", "Can't set \"connectionState\" property");

    if (napi_ok != napi_call_function(env, undefined, js_cb, 0, NULL, NULL))
        napi_throw_error(env, "EINVAL", "Can't call \"connectionOnChange\" callback");
}

static void done_connonchange_js_cb(napi_env env, napi_value js_cb, void *context, void *data)
{
    const char *string = data;
    napi_value undefined;

    napi_get_undefined(env, &undefined);

    if (napi_ok != napi_create_string_utf8(env, string, strlen(string), &done.value))
        napi_throw_error(env, "EINVAL", "Can't create utf8 string from char*");

    if (napi_ok != napi_get_reference_value(env, done.ref, &done.object_value))
        napi_throw_error(env, "EINVAL", "Can't get reference");

    if (napi_ok != napi_set_named_property(env, done.object_value, "connectionState", done.value))
        napi_throw_error(env, "EINVAL", "Can't set \"connectionState\" property");

    if (napi_ok != napi_call_function(env, undefined, js_cb, 0, NULL, NULL))
        napi_throw_error(env, "EINVAL", "Can't call \"connectionOnChange\" callback");
}

static void message_connonchange_js_cb(napi_env env, napi_value js_cb, void *context, void *data)
{
    const char *string = data;
    napi_value undefined;

    napi_get_undefined(env, &undefined);

    if (napi_ok != napi_create_string_utf8(env, string, strlen(string), &message.value))
        napi_throw_error(env, "EINVAL", "Can't create utf8 string from char*");

    if (napi_ok != napi_get_reference_value(env, message.ref, &message.object_value))
        napi_throw_error(env, "EINVAL", "Can't get reference");

    if (napi_ok != napi_set_named_property(env, message.object_value, "connectionState", message.value))
        napi_throw_error(env, "EINVAL", "Can't set \"connectionState\" property");

    if (napi_ok != napi_call_function(env, undefined, js_cb, 0, NULL, NULL))
        napi_throw_error(env, "EINVAL", "Can't call \"connectionOnChange\" callback");
}

static void parameters_connonchange_js_cb(napi_env env, napi_value js_cb, void *context, void *data)
{
    const char *string = data;
    napi_value undefined;

    napi_get_undefined(env, &undefined);

    if (napi_ok != napi_create_string_utf8(env, string, strlen(string), &parameters.value))
        napi_throw_error(env, "EINVAL", "Can't create utf8 string from char*");

    if (napi_ok != napi_get_reference_value(env, parameters.ref, &parameters.object_value))
        napi_throw_error(env, "EINVAL", "Can't get reference");

    if (napi_ok != napi_set_named_property(env, parameters.object_value, "connectionState", parameters.value))
        napi_throw_error(env, "EINVAL", "Can't set \"connectionState\" property");

    if (napi_ok != napi_call_function(env, undefined, js_cb, 0, NULL, NULL))
        napi_throw_error(env, "EINVAL", "Can't call \"connectionOnChange\" callback");
}

static void results_connonchange_js_cb(napi_env env, napi_value js_cb, void *context, void *data)
{
    const char *string = data;
    napi_value undefined;

    napi_get_undefined(env, &undefined);

    if (napi_ok != napi_create_string_utf8(env, string, strlen(string), &results.value))
        napi_throw_error(env, "EINVAL", "Can't create utf8 string from char*");

    if (napi_ok != napi_get_reference_value(env, results.ref, &results.object_value))
        napi_throw_error(env, "EINVAL", "Can't get reference");

    if (napi_ok != napi_set_named_property(env, results.object_value, "connectionState", results.value))
        napi_throw_error(env, "EINVAL", "Can't set \"connectionState\" property");

    if (napi_ok != napi_call_function(env, undefined, js_cb, 0, NULL, NULL))
        napi_throw_error(env, "EINVAL", "Can't call \"connectionOnChange\" callback");
}

static void execute_onchange_js_cb(napi_env env, napi_value js_cb, void *context, void *data)
{
    bool bool_value = *(bool *)data;

    napi_value undefined;

    napi_get_undefined(env, &undefined);

    if (napi_ok != napi_get_boolean(env, bool_value, &execute.value))
        napi_throw_error(env, "EINVAL", "Can't create number from bool");

    if (napi_ok != napi_get_reference_value(env, execute.ref, &execute.object_value))
        napi_throw_error(env, "EINVAL", "Can't get reference");

    if (napi_ok != napi_set_named_property(env, execute.object_value, "value", execute.value))
        napi_throw_error(env, "EINVAL", "Can't set \"value\" property");

    if (napi_ok != napi_call_function(env, undefined, js_cb, 0, NULL, NULL))
        napi_throw_error(env, "EINVAL", "Can't call \"onChange\" callback");
}

static void done_onchange_js_cb(napi_env env, napi_value js_cb, void *context, void *data)
{
    bool bool_value = *(bool *)data;

    napi_value undefined;

    napi_get_undefined(env, &undefined);

    if (napi_ok != napi_get_boolean(env, bool_value, &done.value))
        napi_throw_error(env, "EINVAL", "Can't create number from bool");

    if (napi_ok != napi_get_reference_value(env, done.ref, &done.object_value))
        napi_throw_error(env, "EINVAL", "Can't get reference");

    if (napi_ok != napi_set_named_property(env, done.object_value, "value", done.value))
        napi_throw_error(env, "EINVAL", "Can't set \"value\" property");

    if (napi_ok != napi_call_function(env, undefined, js_cb, 0, NULL, NULL))
        napi_throw_error(env, "EINVAL", "Can't call \"onChange\" callback");
}

static void message_onchange_js_cb(napi_env env, napi_value js_cb, void *context, void *data)
{
    char *str = (char *)data;

    napi_value undefined;

    napi_get_undefined(env, &undefined);

    if (napi_create_string_utf8(env, str, strlen(str), &message.value))
        napi_throw_error(env, "EINVAL", "Can't create string  from message");

    if (napi_ok != napi_get_reference_value(env, message.ref, &message.object_value))
        napi_throw_error(env, "EINVAL", "Can't get reference");

    if (napi_ok != napi_set_named_property(env, message.object_value, "value", message.value))
        napi_throw_error(env, "EINVAL", "Can't set \"value\" property");

    if (napi_ok != napi_call_function(env, undefined, js_cb, 0, NULL, NULL))
        napi_throw_error(env, "EINVAL", "Can't call \"onChange\" callback");
}

static void parameters_onchange_js_cb(napi_env env, napi_value js_cb, void *context, void *data)
{

    MyAppPar_t values[10];
    memcpy(values, data, sizeof(values));

    napi_value undefined;

    napi_get_undefined(env, &undefined);

    if (napi_ok != napi_get_reference_value(env, parameters.ref, &parameters.object_value))
        napi_throw_error(env, "EINVAL", "Can't get reference");

    if (napi_ok != napi_get_named_property(env, parameters.object_value, "value", &parameters.value))
        napi_throw_error(env, "EINVAL", "Can't get property");

    for (uint32_t i = 0; i < 10; i++)
    {
        napi_value object, x, y;

        napi_create_object(env, &object);
        napi_create_int32(env, (int32_t)values[i].x, &x);
        napi_create_int32(env, (int32_t)values[i].y, &y);
        napi_set_named_property(env, object, "x", x);
        napi_set_named_property(env, object, "y", y);
        napi_set_element(env, parameters.value, i, object);
    }

    if (napi_ok != napi_call_function(env, undefined, js_cb, 0, NULL, NULL))
        napi_throw_error(env, "EINVAL", "Can't call \"onChange\" callback");
}

// js callback instances
napi_value myapp_connonchange_init(napi_env env, napi_callback_info info)
{
    return init_napi_onchange(env, info, "MyApp connection change", myapp_connonchange_js_cb, &myapp.connectiononchange_cb);
}

napi_value execute_connonchange_init(napi_env env, napi_callback_info info)
{
    return init_napi_onchange(env, info, "execute connection change", execute_connonchange_js_cb, &execute.connectiononchange_cb);
}

napi_value done_connonchange_init(napi_env env, napi_callback_info info)
{
    return init_napi_onchange(env, info, "done connection change", done_connonchange_js_cb, &done.connectiononchange_cb);
}

napi_value message_connonchange_init(napi_env env, napi_callback_info info)
{
    return init_napi_onchange(env, info, "message connection change", message_connonchange_js_cb, &message.connectiononchange_cb);
}

napi_value parameters_connonchange_init(napi_env env, napi_callback_info info)
{
    return init_napi_onchange(env, info, "parameters connection change", parameters_connonchange_js_cb, &parameters.connectiononchange_cb);
}

napi_value results_connonchange_init(napi_env env, napi_callback_info info)
{
    return init_napi_onchange(env, info, "results connection change", results_connonchange_js_cb, &results.connectiononchange_cb);
}

napi_value execute_onchange_init(napi_env env, napi_callback_info info)
{
    return init_napi_onchange(env, info, "execute dataset change", execute_onchange_js_cb, &execute.onchange_cb);
}

napi_value done_onchange_init(napi_env env, napi_callback_info info)
{
    return init_napi_onchange(env, info, "done dataset change", done_onchange_js_cb, &done.onchange_cb);
}

napi_value message_onchange_init(napi_env env, napi_callback_info info)
{
    return init_napi_onchange(env, info, "message dataset change", message_onchange_js_cb, &message.onchange_cb);
}

napi_value parameters_onchange_init(napi_env env, napi_callback_info info)
{
    return init_napi_onchange(env, info, "parameters dataset change", parameters_onchange_js_cb, &parameters.onchange_cb);
}

napi_value done_publish_method(napi_env env, napi_callback_info info)
{
    bool simple_value = 0;

    if (napi_ok != napi_get_reference_value(env, done.ref, &done.object_value))
    {
        napi_throw_error(env, "EINVAL", "Can't get reference");
        return NULL;
    }

    if (napi_ok != napi_get_named_property(env, done.object_value, "value", &done.value))
    {
        napi_throw_error(env, "EINVAL", "Can't get property");
        return NULL;
    }

    if (napi_ok != napi_get_value_bool(env, done.value, &simple_value))
    {
        napi_throw_error(env, "EINVAL", "Expected boolean");
        return NULL;
    }

    exos_data.done = simple_value;
    exos_dataset_publish(&exos_done);

    return NULL;
}

napi_value message_publish_method(napi_env env, napi_callback_info info)
{
    if (napi_ok != napi_get_reference_value(env, message.ref, &message.object_value))
    {
        napi_throw_error(env, "EINVAL", "Can't get reference");
        return NULL;
    }

    if (napi_ok != napi_get_named_property(env, message.object_value, "value", &message.value))
    {
        napi_throw_error(env, "EINVAL", "Can't get property");
        return NULL;
    }

    size_t result;
    if (napi_ok != napi_get_value_string_utf8(env, message.value, (char *)&exos_data.message, sizeof(exos_data.message), &result))
    {
        napi_throw_error(env, "EINVAL", "Expected string");
        return NULL;
    }

    exos_dataset_publish(&exos_message);

    return NULL;
}

napi_value results_publish_method(napi_env env, napi_callback_info info)
{
    if (napi_ok != napi_get_reference_value(env, results.ref, &results.object_value))
    {
        napi_throw_error(env, "EINVAL", "Can't get reference");
        return NULL;
    }

    if (napi_ok != napi_get_named_property(env, results.object_value, "value", &results.value))
    {
        napi_throw_error(env, "EINVAL", "Can't get property");
        return NULL;
    }

    for (uint32_t i = 0; i < 10; i++)
    {
        napi_value product;

        napi_get_element(env, results.value, i, &product);
        napi_get_named_property(env, product, "product", &product);
        napi_get_value_int32(env, product, &exos_data.results[i].product);
    }

    exos_dataset_publish(&exos_results);

    return NULL;
}

static void cleanup_myapp(void *env)
{
    EXOS_ERROR_CODE ec = EXOS_ERROR_OK;

    uv_idle_stop(&cyclic_h);

    ec = exos_datamodel_delete(&exos_myapp);
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't delete datamodel");
    }
}

void cyclic(uv_idle_t *handle)
{
    exos_datamodel_process(&exos_myapp);
    usleep(2000);
}

napi_value init_myapp(napi_env env, napi_value exports)
{
    napi_value myapp_conn_change;
    napi_value execute_conn_change;
    napi_value done_conn_change;
    napi_value message_conn_change;
    napi_value parameters_conn_change;
    napi_value results_conn_change;

    napi_value execute_onchange;
    napi_value done_publish;
    napi_value done_onchange;
    napi_value message_publish;
    napi_value message_onchange;
    napi_value parameters_onchange;
    napi_value parameters_value_object;
    napi_value results_publish;
    napi_value results_value_object;

    napi_value dataModel;
    napi_value undefined;

    napi_value def_bool, def_number, def_string;
    napi_get_boolean(env, BUR_NAPI_DEFAULT_BOOL_INIT, &def_bool);
    napi_create_int32(env, BUR_NAPI_DEFAULT_NUM_INIT, &def_number);
    napi_create_string_utf8(env, BUR_NAPI_DEFAULT_STRING_INIT, sizeof(BUR_NAPI_DEFAULT_STRING_INIT), &def_string);

    napi_get_undefined(env, &undefined);

    //create artefact
    if (napi_ok != napi_create_object(env, &myapp.value))
        return NULL;

    if (napi_ok != napi_create_object(env, &dataModel))
        return NULL;

    //create topics
    if (napi_ok != napi_create_object(env, &execute.value))
        return NULL;

    if (napi_ok != napi_create_object(env, &done.value))
        return NULL;

    if (napi_ok != napi_create_object(env, &message.value))
        return NULL;

    if (napi_ok != napi_create_object(env, &parameters.value))
        return NULL;

    if (napi_ok != napi_create_array(env, &parameters_value_object))
        return NULL;

    if (napi_ok != napi_create_object(env, &results.value))
        return NULL;

    if (napi_ok != napi_create_array(env, &results_value_object))
        return NULL;

    napi_create_function(env, NULL, 0, execute_onchange_init, NULL, &execute_onchange);
    napi_set_named_property(env, execute.value, "onChange", execute_onchange);
    napi_set_named_property(env, execute.value, "value", def_bool);
    napi_create_function(env, NULL, 0, execute_connonchange_init, NULL, &execute_conn_change);
    napi_set_named_property(env, execute.value, "connectionOnChange", execute_conn_change);
    napi_set_named_property(env, execute.value, "connectionState", undefined);

    napi_create_function(env, NULL, 0, done_onchange_init, NULL, &done_onchange);
    napi_set_named_property(env, done.value, "onChange", done_onchange);
    napi_create_function(env, NULL, 0, done_publish_method, NULL, &done_publish);
    napi_set_named_property(env, done.value, "publish", done_publish);
    napi_set_named_property(env, done.value, "value", def_bool);
    napi_create_function(env, NULL, 0, done_connonchange_init, NULL, &done_conn_change);
    napi_set_named_property(env, done.value, "connectionOnChange", done_conn_change);
    napi_set_named_property(env, done.value, "connectionState", undefined);

    napi_create_function(env, NULL, 0, message_onchange_init, NULL, &message_onchange);
    napi_set_named_property(env, message.value, "onChange", message_onchange);
    napi_create_function(env, NULL, 0, message_publish_method, NULL, &message_publish);
    napi_set_named_property(env, message.value, "publish", message_publish);
    napi_set_named_property(env, message.value, "value", def_string);
    napi_create_function(env, NULL, 0, message_connonchange_init, NULL, &message_conn_change);
    napi_set_named_property(env, message.value, "connectionOnChange", message_conn_change);
    napi_set_named_property(env, message.value, "connectionState", undefined);

    napi_create_function(env, NULL, 0, parameters_onchange_init, NULL, &parameters_onchange);
    napi_set_named_property(env, parameters.value, "onChange", parameters_onchange);
    for (uint32_t i = 0; i < 10; i++)
    {
        napi_value item;
        napi_create_object(env, &item);
        napi_set_named_property(env, item, "x", def_number);
        napi_set_named_property(env, item, "y", def_number);
        napi_set_element(env, parameters_value_object, i, item);
    }
    napi_set_named_property(env, parameters.value, "value", parameters_value_object);
    napi_create_function(env, NULL, 0, parameters_connonchange_init, NULL, &parameters_conn_change);
    napi_set_named_property(env, parameters.value, "connectionOnChange", parameters_conn_change);
    napi_set_named_property(env, parameters.value, "connectionState", undefined);

    napi_create_function(env, NULL, 0, results_publish_method, NULL, &results_publish);
    napi_set_named_property(env, results.value, "publish", results_publish);
    for (uint32_t i = 0; i < 10; i++)
    {
        napi_value item;
        napi_create_object(env, &item);
        napi_set_named_property(env, item, "product", def_number);
        napi_set_element(env, results_value_object, i, item);
    }
    napi_set_named_property(env, results.value, "value", results_value_object);
    napi_create_function(env, NULL, 0, results_connonchange_init, NULL, &results_conn_change);
    napi_set_named_property(env, results.value, "connectionOnChange", results_conn_change);
    napi_set_named_property(env, results.value, "connectionState", undefined);

    //bind topics to artefact
    napi_set_named_property(env, dataModel, "execute", execute.value);
    napi_set_named_property(env, dataModel, "done", done.value);
    napi_set_named_property(env, dataModel, "message", message.value);
    napi_set_named_property(env, dataModel, "parameters", parameters.value);
    napi_set_named_property(env, dataModel, "results", results.value);
    napi_set_named_property(env, myapp.value, "dataModel", dataModel);
    napi_create_function(env, NULL, 0, myapp_connonchange_init, NULL, &myapp_conn_change);
    napi_set_named_property(env, myapp.value, "connectionOnChange", myapp_conn_change);
    napi_set_named_property(env, myapp.value, "connectionState", undefined);

    //export the myapp
    napi_set_named_property(env, exports, "MyApp", myapp.value);

    //save references to objects
    if (napi_ok != napi_create_reference(env, myapp.value, myapp.ref_count, &myapp.ref))
    {
        napi_throw_error(env, "EINVAL", "Can't create myapp reference");
        return NULL;
    }
    if (napi_ok != napi_create_reference(env, execute.value, execute.ref_count, &execute.ref))
    {
        napi_throw_error(env, "EINVAL", "Can't create execute reference");
        return NULL;
    }
    if (napi_ok != napi_create_reference(env, done.value, done.ref_count, &done.ref))
    {
        napi_throw_error(env, "EINVAL", "Can't create done reference");
        return NULL;
    }
    if (napi_ok != napi_create_reference(env, message.value, message.ref_count, &message.ref))
    {
        napi_throw_error(env, "EINVAL", "Can't create message reference");
        return NULL;
    }
    if (napi_ok != napi_create_reference(env, parameters.value, parameters.ref_count, &parameters.ref))
    {
        napi_throw_error(env, "EINVAL", "Can't create parameters reference");
        return NULL;
    }
    if (napi_ok != napi_create_reference(env, results.value, results.ref_count, &results.ref))
    {
        napi_throw_error(env, "EINVAL", "Can't create results reference");
        return NULL;
    }

    //register cleanup hook
    if (napi_ok != napi_add_env_cleanup_hook(env, cleanup_myapp, env))
    {
        napi_throw_error(env, "EINVAL", "Can't register cleanup hook");
        return NULL;
    }

    EXOS_ERROR_CODE ec = EXOS_ERROR_OK;

    //exos inits
    ec = exos_datamodel_init(&exos_myapp, "MyApp", "MyApp_NodeJS");
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't initialize MyApp");
    }
    //set the user_context to access custom data in the callbacks
    exos_myapp.user_context = NULL; //user defined
    exos_myapp.user_tag = 0;        //user defined

    ec = exos_dataset_init(&exos_execute, &exos_myapp, "execute", &exos_data.execute, sizeof(exos_data.execute));
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't initialize execute");
    }
    exos_execute.user_context = NULL; //user defined
    exos_execute.user_tag = 0;        //user defined

    ec = exos_dataset_init(&exos_done, &exos_myapp, "done", &exos_data.done, sizeof(exos_data.done));
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't initialize done");
    }
    exos_done.user_context = NULL; //user defined
    exos_done.user_tag = 0;        //user defined

    ec = exos_dataset_init(&exos_message, &exos_myapp, "message", &exos_data.message, sizeof(exos_data.message));
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't initialize message");
    }
    exos_message.user_context = NULL; //user defined
    exos_message.user_tag = 0;        //user defined

    ec = exos_dataset_init(&exos_parameters, &exos_myapp, "parameters", &exos_data.parameters, sizeof(exos_data.parameters));
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't initialize parameters");
    }
    exos_parameters.user_context = NULL; //user defined
    exos_parameters.user_tag = 0;        //user defined

    ec = exos_dataset_init(&exos_results, &exos_myapp, "results", &exos_data.results, sizeof(exos_data.results));
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't initialize results");
    }
    exos_results.user_context = NULL; //user defined
    exos_results.user_tag = 0;        //user defined

    //register the artefact
    ec = exos_datamodel_connect_myapp(&exos_myapp, datamodelEvent);
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't connect MyApp");
    }

    //register datasets
    ec = exos_dataset_connect(&exos_execute, EXOS_DATASET_SUBSCRIBE, datasetEvent);
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't connect execute");
    }

    ec = exos_dataset_connect(&exos_done, EXOS_DATASET_PUBLISH + EXOS_DATASET_SUBSCRIBE, datasetEvent);
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't connect done");
    }

    ec = exos_dataset_connect(&exos_message, EXOS_DATASET_PUBLISH + EXOS_DATASET_SUBSCRIBE, datasetEvent);
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't connect message");
    }

    ec = exos_dataset_connect(&exos_parameters, EXOS_DATASET_SUBSCRIBE, datasetEvent);
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't connect parameters");
    }

    ec = exos_dataset_connect(&exos_results, EXOS_DATASET_PUBLISH, datasetEvent);
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't connect parameters");
    }

    uv_idle_init(uv_default_loop(), &cyclic_h);
    uv_idle_start(&cyclic_h, cyclic);

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_myapp);