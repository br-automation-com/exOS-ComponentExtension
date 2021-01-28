%module libNodeJS
%{
#define EXOS_INCLUDE_ONLY_DATATYPE
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "exos_nodejs.h"
#include "libnodejs.h"
%}

#define EXOS_INCLUDE_ONLY_DATATYPE
%include "stdint.i"
%include "exos_nodejs.h"

typedef struct libNodeJScounters
{
    void publish(void);
    Counters_typ value;
} libNodeJScounters_t;

typedef struct libNodeJSrun_counter
{
    bool value;
} libNodeJSrun_counter_t;

typedef struct libNodeJS
{
    void connect(void);
    void disconnect(void);
    void process(void);
    void set_operational(void);
    void dispose(void);
    bool is_connected;
    bool is_operational;
    libNodeJScounters_t counters;
    libNodeJSrun_counter_t run_counter;
} libNodeJS_t;

libNodeJS_t *libNodeJS_init(void);
