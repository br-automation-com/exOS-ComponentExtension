#include <string.h>
#include <bur/plctypes.h>
#include "../libNodeJS/libnodejs.h"

#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

static libNodeJS_t *nodejs;

static void on_connected_nodejs(void)
{
}

static void on_change_counters_dataset(void)
{
    memcpy(&counters, &(nodejs->counters.value), sizeof(counters));
}
void _INIT ProgramInit(void)
{
    //retrieve the nodejs structure
    nodejs = libNodeJS_init();

    //setup callbacks
    nodejs->on_connected = on_connected_nodejs;
    // nodejs->on_disconnected = .. ;
    // nodejs->on_operational = .. ;
    nodejs->counters.on_change = on_change_counters_dataset;
}

void _CYCLIC ProgramCyclic(void)
{
    if (Enable && !_Enable)
    {
        //connect to the server
        nodejs->connect();
    }
    if (!Enable && _Enable)
    {
        //disconnect from server
        nodejs->disconnect();
    }
    _Enable = Enable;

    //trigger callbacks
    nodejs->process();

    if (nodejs->is_connected)
    {
        if (memcmp(&(nodejs->counters.value), &counters, sizeof(counters)))
        {
            memcpy(&(nodejs->counters.value), &counters, sizeof(counters));
            nodejs->counters.publish();
        }
    
        if (nodejs->run_counter.value != run_counter)
        {
            nodejs->run_counter.value = run_counter;
            nodejs->run_counter.publish();
        }
    
    }
}

void _EXIT ProgramExit(void)
{
    //shutdown
    nodejs->dispose();

}
