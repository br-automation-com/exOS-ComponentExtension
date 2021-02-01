#ifndef _LIBNODEJS_H_
#define _LIBNODEJS_H_

#include "exos_nodejs.h"

typedef void (*libNodeJS_event_cb)(void);
typedef void (*libNodeJS_method_fn)(void);

typedef struct libNodeJScountUp
{
    libNodeJS_event_cb on_change;
    int32_t value;
} libNodeJScountUp_t;

typedef struct libNodeJScountDown
{
    libNodeJS_event_cb on_change;
    int32_t value;
} libNodeJScountDown_t;

typedef struct libNodeJSstart
{
    libNodeJS_method_fn publish;
    bool value;
} libNodeJSstart_t;

typedef struct libNodeJSreset
{
    libNodeJS_method_fn publish;
    bool value;
} libNodeJSreset_t;

typedef struct libNodeJS
{
    libNodeJS_method_fn connect;
    libNodeJS_method_fn disconnect;
    libNodeJS_method_fn process;
    libNodeJS_method_fn set_operational;
    libNodeJS_method_fn dispose;
    libNodeJS_event_cb on_connected;
    libNodeJS_event_cb on_disconnected;
    libNodeJS_event_cb on_operational;
    bool is_connected;
    bool is_operational;
    libNodeJSstart_t start;
    libNodeJSreset_t reset;
    libNodeJScountUp_t countUp;
    libNodeJScountDown_t countDown;
} libNodeJS_t;

libNodeJS_t *libNodeJS_init(void);
#endif // _LIBNODEJS_H_
