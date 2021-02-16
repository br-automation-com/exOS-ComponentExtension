%module(directors="1") libMyApp
%{
#define EXOS_INCLUDE_ONLY_DATATYPE
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "exos_myapp.h"
#include "libmyapp.h"
%}

%feature("director") MyAppEventHandler;
%inline %{
struct MyAppEventHandler
{
    virtual void on_connected(void) {}
    virtual void on_disconnected(void) {}
    virtual void on_operational(void) {}

    virtual void on_change_execute() {}
    virtual void on_change_done() {}
    virtual void on_change_parameters() {}

    virtual ~MyAppEventHandler() {}
    libMyApp_t *myapp;
};
%}

%{
static MyAppEventHandler *pMyAppEventHandler = NULL;

static void libMyApp_on_connected()
{
    pMyAppEventHandler->on_connected();
}

static void libMyApp_on_disconnected()
{
    pMyAppEventHandler->on_disconnected();
}

static void libMyApp_on_operational()
{
    pMyAppEventHandler->on_operational();
}

static void libMyApp_on_change_execute()
{
    pMyAppEventHandler->on_change_execute();
}
static void libMyApp_on_change_done()
{
    pMyAppEventHandler->on_change_done();
}
static void libMyApp_on_change_parameters()
{
    pMyAppEventHandler->on_change_parameters();
}
%}

%inline %{
void add_event_handler(libMyApp_t *myapp, MyAppEventHandler *handler)
{
    pMyAppEventHandler = handler;

    myapp->on_connected = &libMyApp_on_connected;
    myapp->on_disconnected = &libMyApp_on_disconnected;
    myapp->on_operational = &libMyApp_on_operational;
    
    myapp->execute.on_change = &libMyApp_on_change_execute;
    myapp->done.on_change = &libMyApp_on_change_done;
    myapp->parameters.on_change = &libMyApp_on_change_parameters;
    
    pMyAppEventHandler->myapp = myapp;
    handler = NULL;
}
%}

#define EXOS_INCLUDE_ONLY_DATATYPE
%include "stdint.i"
%include "exos_myapp.h"

typedef struct libMyAppexecute
{
    void on_change(void);
    bool value;
} libMyAppexecute_t;

typedef struct libMyAppdone
{
    void publish(void);
    void on_change(void);
    bool value;
} libMyAppdone_t;

typedef struct libMyAppparameters
{
    void on_change(void);
    MyAppPar_t value;
} libMyAppparameters_t;

typedef struct libMyAppresults
{
    void publish(void);
    MyAppRes_t value;
} libMyAppresults_t;

typedef struct libMyApp
{
    void connect(void);
    void disconnect(void);
    void process(void);
    void set_operational(void);
    void dispose(void);
    void on_connected(void);
    void on_disconnected(void);
    void on_operational(void);
    bool is_connected;
    bool is_operational;
    libMyAppexecute_t execute;
    libMyAppdone_t done;
    libMyAppparameters_t parameters;
    libMyAppresults_t results;
} libMyApp_t;

libMyApp_t *libMyApp_init(void);
