#include <node_api.h>
#include <stdint.h>
#include <stdio.h>
#include <exos_api.h>
#include "exos_simpanel.h"
#include <uv.h>
#include <unistd.h>
#include <string.h>

napi_value simpanel = NULL;

napi_value display = NULL;
napi_value display_publish = NULL;

napi_value encoder = NULL;
napi_value encoder_onchange = NULL;
napi_threadsafe_function encoder_onchange_cb = NULL;

napi_deferred deferred = NULL;
uv_idle_t cyclic_h;

SimPanel exos_data;
exos_value_handle_t exos_display;
exos_value_handle_t exos_encoder;
exos_artefact_handle_t exos_simpanel;

void cyclic(uv_idle_t *handle)
{
    exos_artefact_cyclic(&exos_simpanel);
    usleep(2000);
}

static void exosValueChanged(exos_value_handle_t *value)
{
    printf("exosValueChanged()\n");
    //handle each subscription value separately
    if (0 == strcmp(value->name, "Encoder"))
    {
        printf("exosValueChanged() - %s\n", value->name);
        exos_data.Encoder = *(uint16_t *)value->data;

        if (encoder_onchange_cb != NULL)
        {
            printf("exosValueChanged() - %s - trig callback with value %i\n", value->name, exos_data.Encoder);
            napi_acquire_threadsafe_function(encoder_onchange_cb);
            napi_call_threadsafe_function(encoder_onchange_cb, &exos_data.Encoder, napi_tsfn_blocking);
            napi_release_threadsafe_function(encoder_onchange_cb, napi_tsfn_release);
        }
    }
}

static void exosValuePublished(exos_value_handle_t *value, uint32_t queue_items)
{
    printf("value %s published! queue size:%i\n", value->name, queue_items);
    //handle each published value separately
    /*if(0 == strcmp(value->name,"Display"))
    {
        int16_t *display = (int16_t *)value->data;
    }*/
}

static void exosValueConnectionChanged(exos_value_handle_t *value)
{
    printf("value %s changed state to %s\n", value->name, exos_state_string(value->connection_state));

    switch (value->connection_state)
    {
    case EXOS_STATE_DISCONNECTED:
        break;
    case EXOS_STATE_CONNECTED:
        //call the value changed event to update the value
        //valueChanged(value);
        break;
    case EXOS_STATE_OPERATIONAL:
        break;
    case EXOS_STATE_ABORTED:
        printf("value %s error %d (%s) occured\n", value->name, value->error, exos_error_string(value->error));
        break;
    }
}

static void exosConnectionChanged(exos_artefact_handle_t *artefact)
{
    printf("Application changed state to %s\n", exos_state_string(artefact->connection_state));

    switch (artefact->connection_state)
    {
    case EXOS_STATE_DISCONNECTED:
        break;
    case EXOS_STATE_CONNECTED:
        break;
    case EXOS_STATE_OPERATIONAL:
        printf("SimPanel operational!\n");
        break;
    case EXOS_STATE_ABORTED:
        printf("application error %d (%s) occured\n", artefact->error, exos_error_string(artefact->error));
        break;
    }
}

static void encoder_onchange_js_cb(napi_env env, napi_value js_cb, void *context, void *data)
{
    napi_value undefined;
    napi_get_undefined(env, &undefined);

    napi_value number_js;
    uint32_t *number = data;
    napi_create_uint32(env, *number, &number_js);

    napi_call_function(env, undefined, js_cb, 1, &number_js, NULL);
}

napi_value encoder_onchange_init(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    encoder_onchange_cb = NULL;

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
        if (napi_ok != napi_create_threadsafe_function(env, argv[0], NULL, work_name, 0, 1, NULL, NULL, NULL, encoder_onchange_js_cb, &encoder_onchange_cb))
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
    napi_value argv[1];
    size_t argc = 1;
    int32_t dummy;

    napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

    if (argc < 1)
    {
        napi_throw_error(env, "EINVAL", "Too few arguments");
        return NULL;
    }

    if (napi_ok != napi_get_value_int32(env, argv[0], &dummy))
    {
        napi_throw_error(env, "EINVAL", "Expected number");
        return NULL;
    }

    exos_data.Display = (int16_t)dummy;
    exos_value_publish(&exos_display);
    printf("Published Display: %i\n", exos_data.Display);

    return NULL;
}

napi_value init_simpanel(napi_env env, napi_value exports)
{
    //create artefact
    if (napi_ok != napi_create_object(env, &simpanel))
        return NULL;

    //create topics
    if (napi_ok != napi_create_object(env, &display))
        return NULL;
    if (napi_ok != napi_create_object(env, &encoder))
        return NULL;

    napi_create_function(env, NULL, 0, display_publish_method, NULL, &display_publish);
    napi_set_named_property(env, display, "publish", display_publish);
    napi_create_function(env, NULL, 0, encoder_onchange_init, NULL, &encoder_onchange);
    napi_set_named_property(env, encoder, "onChange", encoder_onchange);

    //bind topics to artefact
    napi_set_named_property(env, simpanel, "Display", display);
    napi_set_named_property(env, simpanel, "Encoder", encoder);

    //export the simpanel
    napi_set_named_property(env, exports, "SimPanel", simpanel);

    exos_artefact_init(&exos_simpanel, "SimPanel");
    exos_value_init(&exos_display, &exos_simpanel, "Display", &exos_data.Display, sizeof(exos_data.Display));
    exos_value_init(&exos_encoder, &exos_simpanel, "Encoder", &exos_data.Encoder, sizeof(exos_data.Encoder));
    exos_artefact_register_simpanel(&exos_simpanel, exosConnectionChanged);
    exos_value_register_publisher(&exos_display, exosValueConnectionChanged, exosValuePublished);
    exos_value_register_subscription(&exos_encoder, exosValueConnectionChanged, exosValueChanged);

    uv_idle_init(uv_default_loop(), &cyclic_h);
    uv_idle_start(&cyclic_h, cyclic);

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_simpanel);