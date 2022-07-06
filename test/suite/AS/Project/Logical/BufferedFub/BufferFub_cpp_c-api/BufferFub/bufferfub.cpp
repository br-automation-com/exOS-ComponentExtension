#include <string.h>
#include <stdbool.h>
#include "BufferFubDatamodel.hpp"

/* datamodel features:

main methods:
    bufferfub->connect()
    bufferfub->disconnect()
    bufferfub->process()
    bufferfub->setOperational()
    bufferfub->dispose()
    bufferfub->getNettime() : (int32_t) get current nettime

void(void) user lambda callback:
    bufferfub->onConnectionChange([&] () {
        // bufferfub->connectionState ...
    })

boolean values:
    bufferfub->isConnected
    bufferfub->isOperational

logging methods:
    bufferfub->log.error << "some value:" << 1 << std::endl;
    bufferfub->log.warning << "some value:" << 1 << std::endl;
    bufferfub->log.success << "some value:" << 1 << std::endl;
    bufferfub->log.info << "some value:" << 1 << std::endl;
    bufferfub->log.debug << "some value:" << 1 << std::endl;
    bufferfub->log.verbose << "some value:" << 1 << std::endl;

dataset bufferedSample:
    bufferfub->bufferedSample.onChange([&] () {
        bufferfub->bufferedSample.value ...
    })
    bufferfub->bufferedSample.nettime : (int32_t) nettime @ time of publish
    bufferfub->bufferedSample.value : (uint32_t)  actual dataset value

dataset setup:
    bufferfub->setup.publish()
    bufferfub->setup.value : (BufferFubSetup_typ)  actual dataset values

dataset cmdSendBurst:
    bufferfub->cmdSendBurst.publish()
    bufferfub->cmdSendBurst.value : (bool)  actual dataset value
*/


_BUR_PUBLIC void BufferFubInit(struct BufferFubInit *inst)
{
    BufferFubDatamodel* bufferfub = new BufferFubDatamodel();
    if (NULL == bufferfub)
    {
        inst->Handle = 0;
        return;
    }
    inst->Handle = (UDINT)bufferfub;
}

_BUR_PUBLIC void BufferFubCyclic(struct BufferFubCyclic *inst)
{
    // return error if reference to structure is not set on function block
    if(NULL == (void*)inst->Handle || NULL == inst->pBufferFub)
    {
        inst->Operational = false;
        inst->Connected = false;
        inst->Error = true;
        return;
    }
    BufferFubDatamodel* bufferfub = static_cast<BufferFubDatamodel*>((void*)inst->Handle);
    if (inst->Enable && !inst->_Enable)
    {
        bufferfub->bufferedSample.onChange([&] () {
            inst->pBufferFub->bufferedSample = bufferfub->bufferedSample.value;
        });
        bufferfub->connect();
    }
    if (!inst->Enable && inst->_Enable)
    {
        bufferfub->disconnect();
    }
    inst->_Enable = inst->Enable;

    if(inst->Start && !inst->_Start && bufferfub->isConnected)
    {
        bufferfub->setOperational();
        inst->_Start = inst->Start;
    }
    if(!inst->Start)
    {
        inst->_Start = false;
    }

    //trigger callbacks
    bufferfub->process();

    if (bufferfub->isConnected)
    {
        //publish the setup dataset as soon as there are changes
        if (0 != memcmp(&inst->pBufferFub->setup, &bufferfub->setup.value, sizeof(bufferfub->setup.value)))
        {
            memcpy(&bufferfub->setup.value, &inst->pBufferFub->setup, sizeof(bufferfub->setup.value));
            bufferfub->setup.publish();
        }
        //publish the cmdSendBurst dataset as soon as there are changes
        if (inst->pBufferFub->cmdSendBurst != bufferfub->cmdSendBurst.value)
        {
            bufferfub->cmdSendBurst.value = inst->pBufferFub->cmdSendBurst;
            bufferfub->cmdSendBurst.publish();
        }
        // Your code here...
    }

    inst->Connected = bufferfub->isConnected;
    inst->Operational = bufferfub->isOperational;
}

_BUR_PUBLIC void BufferFubExit(struct BufferFubExit *inst)
{
    BufferFubDatamodel* bufferfub = static_cast<BufferFubDatamodel*>((void*)inst->Handle);
    delete bufferfub;
}

