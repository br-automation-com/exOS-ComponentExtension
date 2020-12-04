#include <node_api.h>
#include <stdint.h>
#include <stdio.h>
#include <exos_api.h>
#include "exos_simpanel.h"
#include <uv.h>
#include <unistd.h>

napi_value simpanel = NULL;
napi_value display = NULL;
napi_value display_publish = NULL;
napi_value display_onchange = NULL;
napi_threadsafe_function display_onchange_cb = NULL;
napi_deferred deferred = NULL;
int32_t display_value;
uv_idle_t cyclic_h;

SimPanel exos_data;
exos_value_handle_t exos_display;
exos_artefact_handle_t exos_simpanel;

void cyclic(uv_idle_t *handle)
{
    exos_artefact_cyclic(&exos_simpanel);
    usleep(2000);
}

static void CallJs(napi_env env, napi_value js_cb, void *context, void *data)
{
    napi_value undefined;
    napi_get_undefined(env, &undefined);

    napi_value number_js;
    int32_t *number = data;
    napi_create_int32(env, *number, &number_js);

    napi_call_function(env, undefined, js_cb, 1, &number_js, NULL);
}

napi_value display_onchange_method(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    display_onchange_cb = NULL;

    napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

    if (argc < 1)
    {
        napi_throw_error(env, "EINVAL", "Too few arguments");
        return NULL;
    }

    napi_value work_name;
    napi_create_string_utf8(env,
                            "ValucChanged Callback",
                            NAPI_AUTO_LENGTH,
                            &work_name);

    napi_valuetype cb_typ;
    napi_typeof(env, argv[0], &cb_typ);
    if (cb_typ == napi_function)
    {
        if (napi_ok != napi_create_threadsafe_function(env, argv[0], NULL, work_name, 0, 1, NULL, NULL, NULL, CallJs, &display_onchange_cb))
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

    napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

    if (argc < 1)
    {
        napi_throw_error(env, "EINVAL", "Too few arguments");
        return NULL;
    }

    if (napi_ok != napi_get_value_int32(env, argv[0], &display_value))
    {
        napi_throw_error(env, "EINVAL", "Expected number");
        return NULL;
    }

    exos_data.Display = (int16_t)display_value;
    exos_value_publish(&exos_display);
    //printf("Published Display: %i\n", display_value);

    if (display_onchange_cb == NULL)
        printf("No callback!\n");
    else
    {

        napi_acquire_threadsafe_function(display_onchange_cb);
        napi_call_threadsafe_function(display_onchange_cb, &display_value, napi_tsfn_blocking);
        napi_release_threadsafe_function(display_onchange_cb, napi_tsfn_release);
    }

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
    napi_create_function(env, NULL, 0, display_publish_method, NULL, &display_publish);
    napi_set_named_property(env, display, "Publish", display_publish);

    napi_create_function(env, NULL, 0, display_onchange_method, NULL, &display_onchange);
    napi_set_named_property(env, display, "OnChange", display_onchange);

    //bind topics to artefact
    napi_set_named_property(env, simpanel, "Display", display);

    //export the simpanel
    napi_set_named_property(env, exports, "SimPanel", simpanel);

    exos_artefact_init(&exos_simpanel, "SimPanel");
    exos_value_init(&exos_display, &exos_simpanel, "Display", &exos_data.Display, sizeof(exos_data.Display));
    exos_artefact_register_simpanel(&exos_simpanel, NULL);
    exos_value_register_publisher(&exos_display, NULL, NULL);

    uv_idle_init(uv_default_loop(), &cyclic_h);
    uv_idle_start(&cyclic_h, cyclic);

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_simpanel);