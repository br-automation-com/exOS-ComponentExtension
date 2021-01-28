#ifndef _LIBNODEJS_H_
#define _LIBNODEJS_H_

#include "exos_nodejs.h"

typedef void (*libNodeJS_event_cb)(void);
typedef void (*libNodeJS_method_fn)(void);

typedef struct libNodeJScounters
{
    libNodeJS_method_fn publish;
    libNodeJS_event_cb on_change;
    Counters_typ value;
} libNodeJScounters_t;

typedef struct libNodeJSrun_counter
{
    libNodeJS_event_cb on_change;
    bool value;
} libNodeJSrun_counter_t;

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
    libNodeJScounters_t counters;
    libNodeJSrun_counter_t run_counter;
} libNodeJS_t;

libNodeJS_t *libNodeJS_init(void);
#endif // _LIBNODEJS_H_
