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
    napi_threadsafe_function connectionchange_cb;
    napi_value object_value; //volatile placeholder.
    napi_value value;        //volatile placeholder.
} obj_handles;

obj_handles simpanel = {};
obj_handles display = {};
obj_handles encoder = {};

napi_deferred deferred = NULL;
uv_idle_t cyclic_h;

SimPanel exos_data = {};
exos_dataset_handle_t exos_display;
exos_dataset_handle_t exos_encoder;
exos_datamodel_handle_t exos_simpanel;

void cyclic(uv_idle_t *handle)
{
    exos_datamodel_process(&exos_simpanel);
    usleep(2000);
}

static void datasetEvent(exos_dataset_handle_t *dataset, EXOS_DATASET_EVENT_TYPE event_type, void *info)
{
    switch (event_type)
    {
    case EXOS_DATASET_EVENT_UPDATED:
        printf("exosValueChanged()\n");
        //handle each subscription dataset separately
        if (0 == strcmp(dataset->name, "Encoder"))
        {
            printf("exosValueChanged() - %s\n", dataset->name);
            exos_data.Encoder = *(uint16_t *)dataset->data;

            if (encoder.onchange_cb != NULL)
            {
                printf("exosValueChanged() - %s - trig callback with dataset %i\n", dataset->name, exos_data.Encoder);
                napi_acquire_threadsafe_function(encoder.onchange_cb);
                napi_call_threadsafe_function(encoder.onchange_cb, &exos_data.Encoder, napi_tsfn_blocking);
                napi_release_threadsafe_function(encoder.onchange_cb, napi_tsfn_release);
            }
        }
        break;

    case EXOS_DATASET_EVENT_PUBLISHED:
    {
        //handle each published dataset separately
        if (0 == strcmp(dataset->name, "Display"))
        {
            int16_t *display = (int16_t *)dataset->data;
            printf("dataset %s published value: %d\n", dataset->name, *display);
        }
        break;
    }

    case EXOS_DATASET_EVENT_DELIVERED:
    {
        //handle each published dataset separately
        if (0 == strcmp(dataset->name, "Display"))
        {
            int16_t *display = (int16_t *)dataset->data;
            printf("dataset %s delivered value: %d\n", dataset->name, *display);
        }
        break;
    }

    case EXOS_DATASET_EVENT_CONNECTION_CHANGED:
        printf("dataset %s changed state to %s\n", dataset->name, exos_get_state_string(dataset->connection_state));

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
            printf("dataset %s error %d (%s) occured\n", dataset->name, dataset->error, exos_get_error_string(dataset->error));
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

        printf("Application changed state to %s\n", exos_get_state_string(datamodel->connection_state));

        /*        if (napi_ok != napi_get_reference_value(env, display.ref, &display.object_value))
        {
            napi_throw_error(env, "EINVAL", "Can't get reference");
            return NULL;
        }

        if (napi_ok != napi_get_named_property(env, display.object_value, "value", &display.value))
        {
            napi_throw_error(env, "EINVAL", "Can't get property");
            return NULL;
        }
*/
        switch (datamodel->connection_state)
        {
        case EXOS_STATE_DISCONNECTED:
            break;
        case EXOS_STATE_CONNECTED:
            break;
        case EXOS_STATE_OPERATIONAL:
            printf("SimPanel operational!");
            break;
        case EXOS_STATE_ABORTED:
            printf("application error %d (%s) occured", datamodel->error, exos_get_error_string(datamodel->error));

            /*            if (napi_ok != napi_set_value_utf8(env, display.value, &simple_value))
            {
                napi_throw_error(env, "EINVAL", "Expected number");
                return NULL;
            }
*/
            break;
        }
        break;
    }
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

napi_value encoder_onchange_init(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    encoder.onchange_cb = NULL;

    napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

    if (argc < 1)
    {
        napi_throw_error(env, "EINVAL", "Too few arguments");
        return NULL;
    }

    napi_value work_name;
    napi_create_string_utf8(env,
                            "ValueChanged Callback",
                            NAPI_AUTO_LENGTH,
                            &work_name);

    napi_valuetype cb_typ;
    napi_typeof(env, argv[0], &cb_typ);
    if (cb_typ == napi_function)
    {
        if (napi_ok != napi_create_threadsafe_function(env, argv[0], NULL, work_name, 0, 1, NULL, NULL, NULL, encoder_onchange_js_cb, &encoder.onchange_cb))
        {
            const napi_extended_error_info *info;
            napi_get_last_error_info(env, &info);

            napi_throw_error(env, NULL, info->error_message);
            return NULL;
        }
    }
    return NULL;
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
    printf("Published Display: %i\n", exos_data.Display);

    return NULL;
}

napi_value init_simpanel(napi_env env, napi_value exports)
{
    napi_value display_publish;
    //napi_value display_value;
    napi_value encoder_onchange;
    //napi_value encoder_value;
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

    napi_create_function(env, NULL, 0, display_publish_method, NULL, &display_publish);
    napi_set_named_property(env, display.value, "publish", display_publish);
    //napi_create_int32(env, (int32_t)exos_data.Display, &display_value);
    napi_set_named_property(env, display.value, "value", undefined);
    napi_set_named_property(env, display.value, "connectionState", undefined);

    napi_create_function(env, NULL, 0, encoder_onchange_init, NULL, &encoder_onchange);
    napi_set_named_property(env, encoder.value, "onChange", encoder_onchange);
    //napi_create_uint32(env, (uint32_t)exos_data.Encoder, &encoder_value);
    napi_set_named_property(env, encoder.value, "value", undefined);
    napi_set_named_property(env, encoder.value, "connectionState", undefined);

    //bind topics to artefact
    napi_set_named_property(env, dataModel, "Display", display.value);
    napi_set_named_property(env, dataModel, "Encoder", encoder.value);
    napi_set_named_property(env, simpanel.value, "DataModel", dataModel);
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

    uv_idle_init(uv_default_loop(), &cyclic_h);
    uv_idle_start(&cyclic_h, cyclic);

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_simpanel);