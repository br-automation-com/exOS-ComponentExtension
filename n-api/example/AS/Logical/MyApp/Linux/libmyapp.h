#ifndef _LIBMYAPP_H_
#define _LIBMYAPP_H_

#include "exos_myapp.h"

typedef void (*libMyApp_event_cb)(void);
typedef void (*libMyApp_method_fn)(void);

typedef struct libMyAppexecute
{
    libMyApp_event_cb on_change;
    bool value;
} libMyAppexecute_t;

typedef struct libMyAppdone
{
    libMyApp_method_fn publish;
    libMyApp_event_cb on_change;
    bool value;
} libMyAppdone_t;

typedef struct libMyAppparameters
{
    libMyApp_event_cb on_change;
    MyAppPar_t value;
} libMyAppparameters_t;

typedef struct libMyAppresults
{
    libMyApp_method_fn publish;
    MyAppRes_t value;
} libMyAppresults_t;

typedef struct libMyApp
{
    libMyApp_method_fn connect;
    libMyApp_method_fn disconnect;
    libMyApp_method_fn process;
    libMyApp_method_fn set_operational;
    libMyApp_method_fn dispose;
    libMyApp_event_cb on_connected;
    libMyApp_event_cb on_disconnected;
    libMyApp_event_cb on_operational;
    bool is_connected;
    bool is_operational;
    libMyAppexecute_t execute;
    libMyAppdone_t done;
    libMyAppparameters_t parameters;
    libMyAppresults_t results;
} libMyApp_t;

#ifdef __cplusplus
extern "C" {
#endif
libMyApp_t *libMyApp_init(void);
#ifdef __cplusplus
}
#endif
#endif // _LIBMYAPP_H_
