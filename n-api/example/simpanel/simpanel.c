#define NAPI_VERSION 6
#include <node_api.h>
#include <stdint.h>
#include <stdio.h>
#include <exos_api.h>
#include "exos_simpanel.h"
#include <uv.h>
#include <unistd.h>
#include <string.h>

typedef struct
{
    napi_ref ref;
    uint32_t ref_count;
    napi_threadsafe_function onchange_cb;
    napi_threadsafe_function connectiononchange_cb;
    napi_value object_value; //volatile placeholder.
    napi_value value;        //volatile placeholder.
} obj_handles;

obj_handles simpanel = {};
obj_handles display = {};
obj_handles encoder = {};
obj_handles knobs = {};

napi_deferred deferred = NULL;
uv_idle_t cyclic_h;

SimPanel exos_data = {};
exos_dataset_handle_t exos_display;
exos_dataset_handle_t exos_encoder;
exos_dataset_handle_t exos_knobs;
exos_datamodel_handle_t exos_simpanel;

void cyclic(uv_idle_t *handle)
{
    exos_datamodel_process(&exos_simpanel);
    usleep(2000);
}

// exOS call backs
static void datasetEvent(exos_dataset_handle_t *dataset, EXOS_DATASET_EVENT_TYPE event_type, void *info)
{
    switch (event_type)
    {
    case EXOS_DATASET_EVENT_UPDATED:
        //handle each subscription dataset separately
        if (0 == strcmp(dataset->name, "Encoder"))
        {
            exos_data.Encoder = *(uint16_t *)dataset->data;

            if (encoder.onchange_cb != NULL)
            {
                napi_acquire_threadsafe_function(encoder.onchange_cb);
                napi_call_threadsafe_function(encoder.onchange_cb, &exos_data.Encoder, napi_tsfn_blocking);
                napi_release_threadsafe_function(encoder.onchange_cb, napi_tsfn_release);
            }
        }
        else if (0 == strcmp(dataset->name, "Knobs"))
        {
            exos_data.Knobs = *(SimPanelKnobs *)dataset->data;

            if (knobs.onchange_cb != NULL)
            {
                napi_acquire_threadsafe_function(knobs.onchange_cb);
                napi_call_threadsafe_function(knobs.onchange_cb, &exos_data.Knobs, napi_tsfn_blocking);
                napi_release_threadsafe_function(knobs.onchange_cb, napi_tsfn_release);
            }
        }
        break;

    case EXOS_DATASET_EVENT_PUBLISHED:
    {
        //handle each published dataset separately
        if (0 == strcmp(dataset->name, "Display"))
        {
            //int16_t *display = (int16_t *)dataset->data;
        }
        break;
    }

    case EXOS_DATASET_EVENT_DELIVERED:
    {
        //handle each published dataset separately
        if (0 == strcmp(dataset->name, "Display"))
        {
            //int16_t *display = (int16_t *)dataset->data;
        }
        break;
    }

    case EXOS_DATASET_EVENT_CONNECTION_CHANGED:
        if (0 == strcmp(dataset->name, "Display"))
        {
            if (display.connectiononchange_cb != NULL)
            {
                napi_acquire_threadsafe_function(display.connectiononchange_cb);
                napi_call_threadsafe_function(display.connectiononchange_cb, exos_get_state_string(dataset->connection_state), napi_tsfn_blocking);
                napi_release_threadsafe_function(display.connectiononchange_cb, napi_tsfn_release);
            }
        }
        else if (0 == strcmp(dataset->name, "Encoder"))
        {
            if (encoder.connectiononchange_cb != NULL)
            {
                napi_acquire_threadsafe_function(encoder.connectiononchange_cb);
                napi_call_threadsafe_function(encoder.connectiononchange_cb, exos_get_state_string(dataset->connection_state), napi_tsfn_blocking);
                napi_release_threadsafe_function(encoder.connectiononchange_cb, napi_tsfn_release);
            }
        }
        else if (0 == strcmp(dataset->name, "Knobs"))
        {
            if (knobs.connectiononchange_cb != NULL)
            {
                napi_acquire_threadsafe_function(knobs.connectiononchange_cb);
                napi_call_threadsafe_function(knobs.connectiononchange_cb, exos_get_state_string(dataset->connection_state), napi_tsfn_blocking);
                napi_release_threadsafe_function(knobs.connectiononchange_cb, napi_tsfn_release);
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
        if (simpanel.connectiononchange_cb != NULL)
        {
            napi_acquire_threadsafe_function(simpanel.connectiononchange_cb);
            napi_call_threadsafe_function(simpanel.connectiononchange_cb, exos_get_state_string(datamodel->connection_state), napi_tsfn_blocking);
            napi_release_threadsafe_function(simpanel.connectiononchange_cb, napi_tsfn_release);
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
static void simpanel_connonchange_js_cb(napi_env env, napi_value js_cb, void *context, void *data)
{
    const char *string = data;
    napi_value undefined;

    napi_get_undefined(env, &undefined);

    if (napi_ok != napi_create_string_utf8(env, string, strlen(string), &simpanel.value))
        napi_throw_error(env, "EINVAL", "Can't create utf8 string from char*");

    if (napi_ok != napi_get_reference_value(env, simpanel.ref, &simpanel.object_value))
        napi_throw_error(env, "EINVAL", "Can't get reference");

    if (napi_ok != napi_set_named_property(env, simpanel.object_value, "connectionState", simpanel.value))
        napi_throw_error(env, "EINVAL", "Can't set \"connectionState\" property");

    if (napi_ok != napi_call_function(env, undefined, js_cb, 0, NULL, NULL))
        napi_throw_error(env, "EINVAL", "Can't call \"connectionOnChange\" callback");
}

static void display_connonchange_js_cb(napi_env env, napi_value js_cb, void *context, void *data)
{
    const char *string = data;
    napi_value undefined;

    napi_get_undefined(env, &undefined);

    if (napi_ok != napi_create_string_utf8(env, string, strlen(string), &display.value))
        napi_throw_error(env, "EINVAL", "Can't create utf8 string from char*");

    if (napi_ok != napi_get_reference_value(env, display.ref, &display.object_value))
        napi_throw_error(env, "EINVAL", "Can't get reference");

    if (napi_ok != napi_set_named_property(env, display.object_value, "connectionState", display.value))
        napi_throw_error(env, "EINVAL", "Can't set \"connectionState\" property");

    if (napi_ok != napi_call_function(env, undefined, js_cb, 0, NULL, NULL))
        napi_throw_error(env, "EINVAL", "Can't call \"connectionOnChange\" callback");
}

static void encoder_connonchange_js_cb(napi_env env, napi_value js_cb, void *context, void *data)
{
    const char *string = data;
    napi_value undefined;

    napi_get_undefined(env, &undefined);

    if (napi_ok != napi_create_string_utf8(env, string, strlen(string), &encoder.value))
        napi_throw_error(env, "EINVAL", "Can't create utf8 string from char*");

    if (napi_ok != napi_get_reference_value(env, encoder.ref, &encoder.object_value))
        napi_throw_error(env, "EINVAL", "Can't get reference");

    if (napi_ok != napi_set_named_property(env, encoder.object_value, "connectionState", encoder.value))
        napi_throw_error(env, "EINVAL", "Can't set \"connectionState\" property");

    if (napi_ok != napi_call_function(env, undefined, js_cb, 0, NULL, NULL))
        napi_throw_error(env, "EINVAL", "Can't call \"connectionOnChange\" callback");
}

static void knobs_connonchange_js_cb(napi_env env, napi_value js_cb, void *context, void *data)
{
    const char *string = data;
    napi_value undefined;

    napi_get_undefined(env, &undefined);

    if (napi_ok != napi_create_string_utf8(env, string, strlen(string), &knobs.value))
        napi_throw_error(env, "EINVAL", "Can't create utf8 string from char*");

    if (napi_ok != napi_get_reference_value(env, knobs.ref, &knobs.object_value))
        napi_throw_error(env, "EINVAL", "Can't get reference");

    if (napi_ok != napi_set_named_property(env, knobs.object_value, "connectionState", knobs.value))
        napi_throw_error(env, "EINVAL", "Can't set \"connectionState\" property");

    if (napi_ok != napi_call_function(env, undefined, js_cb, 0, NULL, NULL))
        napi_throw_error(env, "EINVAL", "Can't call \"connectionOnChange\" callback");
}

static void encoder_onchange_js_cb(napi_env env, napi_value js_cb, void *context, void *data)
{
    uint32_t *number = data;
    napi_value undefined;

    napi_get_undefined(env, &undefined);

    if (napi_ok != napi_create_uint32(env, *number, &encoder.value))
        napi_throw_error(env, "EINVAL", "Can't create number from uint32_t");

    if (napi_ok != napi_get_reference_value(env, encoder.ref, &encoder.object_value))
        napi_throw_error(env, "EINVAL", "Can't get reference");

    if (napi_ok != napi_set_named_property(env, encoder.object_value, "value", encoder.value))
        napi_throw_error(env, "EINVAL", "Can't set \"value\" property");

    if (napi_ok != napi_call_function(env, undefined, js_cb, 0, NULL, NULL))
        napi_throw_error(env, "EINVAL", "Can't call \"onChange\" callback");
}

static void knobs_onchange_js_cb(napi_env env, napi_value js_cb, void *context, void *data)
{
    SimPanelKnobs *values = data;
    int32_t castedP1;
    int32_t castedP2;
    napi_value undefined;
    napi_value P1;
    napi_value P2;

    napi_get_undefined(env, &undefined);

    castedP1 = (int32_t)values->P1;
    castedP2 = (int32_t)values->P2;

    if (napi_ok != napi_get_reference_value(env, knobs.ref, &knobs.object_value))
        napi_throw_error(env, "EINVAL", "Can't get reference");

    if (napi_ok != napi_get_named_property(env, knobs.object_value, "value", &knobs.value))
        napi_throw_error(env, "EINVAL", "Can't get property");

    if (napi_ok != napi_create_int32(env, castedP1, &P1))
        napi_throw_error(env, "EINVAL", "Can't create number from uint32_t");

    if (napi_ok != napi_set_named_property(env, knobs.value, "P1", P1))
        napi_throw_error(env, "EINVAL", "Can't set \"value\" property");

    if (napi_ok != napi_create_int32(env, castedP2, &P2))
        napi_throw_error(env, "EINVAL", "Can't create number from uint32_t");

    if (napi_ok != napi_set_named_property(env, knobs.value, "P2", P2))
        napi_throw_error(env, "EINVAL", "Can't set \"value\" property");

    if (napi_ok != napi_call_function(env, undefined, js_cb, 0, NULL, NULL))
        napi_throw_error(env, "EINVAL", "Can't call \"onChange\" callback");
}

// js callback instances
napi_value simpanel_connonchange_init(napi_env env, napi_callback_info info)
{
    return init_napi_onchange(env, info, "SimPanel connection change", simpanel_connonchange_js_cb, &simpanel.connectiononchange_cb);
}

napi_value display_connonchange_init(napi_env env, napi_callback_info info)
{
    return init_napi_onchange(env, info, "Display connection change", display_connonchange_js_cb, &display.connectiononchange_cb);
}

napi_value encoder_connonchange_init(napi_env env, napi_callback_info info)
{
    return init_napi_onchange(env, info, "encoder connection change", encoder_connonchange_js_cb, &encoder.connectiononchange_cb);
}

napi_value knobs_connonchange_init(napi_env env, napi_callback_info info)
{
    return init_napi_onchange(env, info, "knobs connection change", knobs_connonchange_js_cb, &knobs.connectiononchange_cb);
}

napi_value encoder_onchange_init(napi_env env, napi_callback_info info)
{
    return init_napi_onchange(env, info, "Encoder dataset change", encoder_onchange_js_cb, &encoder.onchange_cb);
}

napi_value knobs_onchange_init(napi_env env, napi_callback_info info)
{
    return init_napi_onchange(env, info, "Knobs dataset change", knobs_onchange_js_cb, &knobs.onchange_cb);
}

napi_value display_publish_method(napi_env env, napi_callback_info info)
{
    int32_t simple_value = 0;

    if (napi_ok != napi_get_reference_value(env, display.ref, &display.object_value))
    {
        napi_throw_error(env, "EINVAL", "Can't get reference");
        return NULL;
    }

    if (napi_ok != napi_get_named_property(env, display.object_value, "value", &display.value))
    {
        napi_throw_error(env, "EINVAL", "Can't get property");
        return NULL;
    }

    if (napi_ok != napi_get_value_int32(env, display.value, &simple_value))
    {
        napi_throw_error(env, "EINVAL", "Expected number");
        return NULL;
    }

    exos_data.Display = (int16_t)simple_value;
    exos_dataset_publish(&exos_display);

    return NULL;
}

napi_value knobs_publish_method(napi_env env, napi_callback_info info)
{
    int32_t simple_value_P1;
    int32_t simple_value_P2;
    napi_value P1;
    napi_value P2;

    if (napi_ok != napi_get_reference_value(env, knobs.ref, &knobs.object_value))
    {
        napi_throw_error(env, "EINVAL", "Can't get reference");
        return NULL;
    }

    if (napi_ok != napi_get_named_property(env, knobs.object_value, "value", &knobs.value))
    {
        napi_throw_error(env, "EINVAL", "Can't get property");
        return NULL;
    }

    if (napi_ok != napi_get_named_property(env, knobs.value, "P1", &P1))
    {
        napi_throw_error(env, "EINVAL", "Can't get property");
        return NULL;
    }

    if (napi_ok != napi_get_value_int32(env, P1, &simple_value_P1))
    {
        napi_throw_error(env, "EINVAL", "P1 - Expected number");
        return NULL;
    }

    if (napi_ok != napi_get_named_property(env, knobs.value, "P2", &P2))
    {
        napi_throw_error(env, "EINVAL", "Can't get property");
        return NULL;
    }

    if (napi_ok != napi_get_value_int32(env, P2, &simple_value_P2))
    {
        napi_throw_error(env, "EINVAL", "P2 - Expected number");
        return NULL;
    }

    exos_data.Knobs.P1 = (int16_t)simple_value_P1;
    exos_data.Knobs.P2 = (int16_t)simple_value_P2;
    exos_dataset_publish(&exos_knobs);

    return NULL;
}

napi_value init_simpanel(napi_env env, napi_value exports)
{
    napi_value simpanel_conn_change;
    napi_value display_conn_change;
    napi_value encoder_conn_change;
    napi_value knobs_conn_change;

    napi_value display_publish;
    napi_value encoder_onchange;
    napi_value knobs_publish;
    napi_value knobs_onchange;
    napi_value knobs_value_object;

    napi_value dataModel;
    napi_value undefined;

    napi_get_undefined(env, &undefined);

    //create artefact
    if (napi_ok != napi_create_object(env, &simpanel.value))
        return NULL;

    if (napi_ok != napi_create_object(env, &dataModel))
        return NULL;

    //create topics
    if (napi_ok != napi_create_object(env, &display.value))
        return NULL;

    if (napi_ok != napi_create_object(env, &encoder.value))
        return NULL;

    if (napi_ok != napi_create_object(env, &knobs.value))
        return NULL;

    if (napi_ok != napi_create_object(env, &knobs_value_object))
        return NULL;

    napi_create_function(env, NULL, 0, display_publish_method, NULL, &display_publish);
    napi_set_named_property(env, display.value, "publish", display_publish);
    napi_set_named_property(env, display.value, "value", undefined);
    napi_create_function(env, NULL, 0, display_connonchange_init, NULL, &display_conn_change);
    napi_set_named_property(env, display.value, "connectionOnChange", display_conn_change);
    napi_set_named_property(env, display.value, "connectionState", undefined);

    napi_create_function(env, NULL, 0, encoder_onchange_init, NULL, &encoder_onchange);
    napi_set_named_property(env, encoder.value, "onChange", encoder_onchange);
    napi_set_named_property(env, encoder.value, "value", undefined);
    napi_create_function(env, NULL, 0, encoder_connonchange_init, NULL, &encoder_conn_change);
    napi_set_named_property(env, encoder.value, "connectionOnChange", encoder_conn_change);
    napi_set_named_property(env, encoder.value, "connectionState", undefined);

    napi_create_function(env, NULL, 0, knobs_onchange_init, NULL, &knobs_onchange);
    napi_set_named_property(env, knobs.value, "onChange", knobs_onchange);
    napi_create_function(env, NULL, 0, knobs_publish_method, NULL, &knobs_publish);
    napi_set_named_property(env, knobs.value, "publish", knobs_publish);
    napi_set_named_property(env, knobs_value_object, "P1", undefined);
    napi_set_named_property(env, knobs_value_object, "P2", undefined);
    napi_set_named_property(env, knobs.value, "value", knobs_value_object);
    napi_create_function(env, NULL, 0, knobs_connonchange_init, NULL, &knobs_conn_change);
    napi_set_named_property(env, knobs.value, "connectionOnChange", knobs_conn_change);
    napi_set_named_property(env, knobs.value, "connectionState", undefined);

    //bind topics to artefact
    napi_set_named_property(env, dataModel, "Display", display.value);
    napi_set_named_property(env, dataModel, "Encoder", encoder.value);
    napi_set_named_property(env, dataModel, "Knobs", knobs.value);
    napi_set_named_property(env, simpanel.value, "DataModel", dataModel);
    napi_create_function(env, NULL, 0, simpanel_connonchange_init, NULL, &simpanel_conn_change);
    napi_set_named_property(env, simpanel.value, "connectionOnChange", simpanel_conn_change);
    napi_set_named_property(env, simpanel.value, "connectionState", undefined);

    //export the simpanel
    napi_set_named_property(env, exports, "SimPanel", simpanel.value);

    //save references to objects
    if (napi_ok != napi_create_reference(env, simpanel.value, simpanel.ref_count, &simpanel.ref))
    {
        napi_throw_error(env, "EINVAL", "Can't create simpanel reference");
        return NULL;
    }
    if (napi_ok != napi_create_reference(env, display.value, display.ref_count, &display.ref))
    {
        napi_throw_error(env, "EINVAL", "Can't create simpanel reference");
        return NULL;
    }
    if (napi_ok != napi_create_reference(env, encoder.value, encoder.ref_count, &encoder.ref))
    {
        napi_throw_error(env, "EINVAL", "Can't create simpanel reference");
        return NULL;
    }
    if (napi_ok != napi_create_reference(env, knobs.value, knobs.ref_count, &knobs.ref))
    {
        napi_throw_error(env, "EINVAL", "Can't create simpanel reference");
        return NULL;
    }

    EXOS_ERROR_CODE ec = EXOS_ERROR_OK;

    //exos inits
    ec = exos_datamodel_init(&exos_simpanel, "SimPanel", "SimPanel_NodeJS");
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't initialize SimPanel");
    }
    //set the user_context to access custom data in the callbacks
    exos_simpanel.user_context = NULL; //user defined
    exos_simpanel.user_tag = 0;        //user defined

    ec = exos_dataset_init(&exos_display, &exos_simpanel, "Display", &exos_data.Display, sizeof(exos_data.Display));
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't initialize Display");
    }
    exos_display.user_context = NULL; //user defined
    exos_display.user_tag = 0;        //user defined

    ec = exos_dataset_init(&exos_encoder, &exos_simpanel, "Encoder", &exos_data.Encoder, sizeof(exos_data.Encoder));
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't initialize Encoder");
    }
    exos_encoder.user_context = NULL; //user defined
    exos_encoder.user_tag = 0;        //user defined

    ec = exos_dataset_init(&exos_knobs, &exos_simpanel, "Knobs", &exos_data.Knobs, sizeof(exos_data.Knobs));
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't initialize Knobs");
    }
    exos_knobs.user_context = NULL; //user defined
    exos_knobs.user_tag = 0;        //user defined

    ec = exos_datamodel_connect_simpanel(&exos_simpanel, datamodelEvent);

    //register the artefact
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't connect SimPanel");
    }

    //register datasets
    ec = exos_dataset_connect(&exos_display, EXOS_DATASET_PUBLISH, datasetEvent);
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't connect Display");
    }

    ec = exos_dataset_connect(&exos_encoder, EXOS_DATASET_SUBSCRIBE, datasetEvent);
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't connect Encoder");
    }

    ec = exos_dataset_connect(&exos_knobs, EXOS_DATASET_SUBSCRIBE + EXOS_DATASET_PUBLISH, datasetEvent);
    if (EXOS_ERROR_OK != ec)
    {
        napi_throw_error(env, "EINVAL", "Can't connect Knobs");
    }

    uv_idle_init(uv_default_loop(), &cyclic_h);
    uv_idle_start(&cyclic_h, cyclic);

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_simpanel);

//IMPLEMENT API_EXTERN napi_status napi_remove_env_cleanup_hook(napi_env env, void (*fun)(void* arg), void* arg);