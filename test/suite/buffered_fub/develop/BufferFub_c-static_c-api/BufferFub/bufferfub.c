#include <string.h>
#include <stdbool.h>
#include "libbufferfub.h"

/* libBufferFub_t datamodel features:

main methods:
    bufferfub->connect()
    bufferfub->disconnect()
    bufferfub->process()
    bufferfub->set_operational()
    bufferfub->dispose()
    bufferfub->get_nettime() : (int32_t) get current nettime

void(void) user callbacks:
    bufferfub->on_connected
    bufferfub->on_disconnected
    bufferfub->on_operational

boolean values:
    bufferfub->is_connected
    bufferfub->is_operational

logging methods:
    bufferfub->log.error(char *)
    bufferfub->log.warning(char *)
    bufferfub->log.success(char *)
    bufferfub->log.info(char *)
    bufferfub->log.debug(char *)
    bufferfub->log.verbose(char *)

dataset bufferedSample:
    bufferfub->bufferedSample.on_change : void(void) user callback function
    bufferfub->bufferedSample.nettime : (int32_t) nettime @ time of publish
    bufferfub->bufferedSample.value : (uint32_t)  actual dataset value

dataset setup:
    bufferfub->setup.publish()
    bufferfub->setup.value : (BufferFubSetup_typ)  actual dataset values

dataset cmdSendBurst:
    bufferfub->cmdSendBurst.publish()
    bufferfub->cmdSendBurst.value : (bool)  actual dataset value
*/

static libBufferFub_t *bufferfub;
static struct BufferFubCyclic *cyclic_inst;

static void on_connected_bufferfub(void)
{
}

static void on_change_bufferedsample(void)
{
    cyclic_inst->pBufferFub->bufferedSample = bufferfub->bufferedSample.value;
    
    // Your code here...
}
_BUR_PUBLIC void BufferFubCyclic(struct BufferFubCyclic *inst)
{
    // check if function block has been created before
    if(cyclic_inst != NULL)
    {
        // return error if more than one function blocks have been created
        if(inst != cyclic_inst)
        {
            inst->Operational = false;
            inst->Connected = false;
            inst->Error = true;
            return;
        }
    }
    cyclic_inst = inst;
    // initialize library
    if((libBufferFub_t *)inst->_Handle == NULL || (libBufferFub_t *)inst->_Handle != bufferfub)
    {
        //retrieve the bufferfub structure
        bufferfub = libBufferFub_init();

        //setup callbacks
        bufferfub->on_connected = on_connected_bufferfub;
        // bufferfub->on_disconnected = .. ;
        // bufferfub->on_operational = .. ;
        bufferfub->bufferedSample.on_change = on_change_bufferedsample;

        inst->_Handle = (UDINT)bufferfub;
    }
    // return error if reference to structure is not set on function block
    if(inst->pBufferFub == NULL)
    {
        inst->Operational = false;
        inst->Connected = false;
        inst->Error = true;
        return;
    }
    if (inst->Enable && !inst->_Enable)
    {
        //connect to the server
        bufferfub->connect();
    }
    if (!inst->Enable && inst->_Enable)
    {
        //disconnect from server
        cyclic_inst = NULL;
        bufferfub->disconnect();
    }
    inst->_Enable = inst->Enable;

    if(inst->Start && !inst->_Start && bufferfub->is_connected)
    {
        bufferfub->set_operational();
        inst->_Start = inst->Start;
    }
    if(!inst->Start)
    {
        inst->_Start = false;
    }

    //trigger callbacks
    bufferfub->process();

    if (bufferfub->is_connected)
    {
        if (memcmp(&(bufferfub->setup.value), &(inst->pBufferFub->setup), sizeof(inst->pBufferFub->setup)))
        {
            memcpy(&(bufferfub->setup.value), &(inst->pBufferFub->setup), sizeof(bufferfub->setup.value));
            bufferfub->setup.publish();
        }
    
        if (bufferfub->cmdSendBurst.value != inst->pBufferFub->cmdSendBurst)
        {
            bufferfub->cmdSendBurst.value = inst->pBufferFub->cmdSendBurst;
            bufferfub->cmdSendBurst.publish();
        }
    
        // Your code here...
    }
    inst->Connected = bufferfub->is_connected;
    inst->Operational = bufferfub->is_operational;
}

UINT _EXIT ProgramExit(unsigned long phase)
{
    //shutdown
    bufferfub->dispose();
    cyclic_inst = NULL;
    return 0;
}
