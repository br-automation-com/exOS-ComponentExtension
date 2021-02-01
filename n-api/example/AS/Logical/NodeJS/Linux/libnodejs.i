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

typedef struct libNodeJSstart
{
    bool value;
} libNodeJSstart_t;

typedef struct libNodeJSreset
{
    bool value;
} libNodeJSreset_t;

typedef struct libNodeJScountUp
{
    void publish(void);
    int32_t value;
} libNodeJScountUp_t;

typedef struct libNodeJScountDown
{
    void publish(void);
    int32_t value;
} libNodeJScountDown_t;

typedef struct libNodeJS
{
    void connect(void);
    void disconnect(void);
    void process(void);
    void set_operational(void);
    void dispose(void);
    bool is_connected;
    bool is_operational;
    libNodeJSstart_t start;
    libNodeJSreset_t reset;
    libNodeJScountUp_t countUp;
    libNodeJScountDown_t countDown;
} libNodeJS_t;

libNodeJS_t *libNodeJS_init(void);
