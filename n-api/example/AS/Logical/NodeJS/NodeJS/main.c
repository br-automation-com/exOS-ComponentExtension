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

static void on_change_countup(void)
{
    countUp = nodejs->countUp.value;
}
static void on_change_countdown(void)
{
    countDown = nodejs->countDown.value;
}
void _INIT ProgramInit(void)
{
    //retrieve the nodejs structure
    nodejs = libNodeJS_init();

    //setup callbacks
    nodejs->on_connected = on_connected_nodejs;
    // nodejs->on_disconnected = .. ;
    // nodejs->on_operational = .. ;
    nodejs->countUp.on_change = on_change_countup;
    nodejs->countDown.on_change = on_change_countdown;
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
        if (nodejs->start.value != start)
        {
            nodejs->start.value = start;
            nodejs->start.publish();
        }
    
        if (nodejs->reset.value != reset)
        {
            nodejs->reset.value = reset;
            nodejs->reset.publish();
        }
    
    }
}

void _EXIT ProgramExit(void)
{
    //shutdown
    nodejs->dispose();

}
