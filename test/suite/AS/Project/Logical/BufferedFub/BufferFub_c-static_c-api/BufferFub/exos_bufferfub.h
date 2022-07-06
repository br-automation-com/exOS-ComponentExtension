/*Automatically generated header file from BufferFub.typ*/

#ifndef _EXOS_COMP_BUFFERFUB_H_
#define _EXOS_COMP_BUFFERFUB_H_

#include "exos_api.h"

#if defined(_SG4)
#include <BufferFub.h>
#else
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct BufferFubSetup_typ
{
    uint32_t sampleCount; //number of samples sent each time cmdSendBurst is set
    uint32_t sampleDelay; //ms between samples

} BufferFubSetup_typ;

typedef struct BufferFub
{
    uint32_t bufferedSample; //SUB BUFFERED
    struct BufferFubSetup_typ setup; //PUB
    bool cmdSendBurst; //PUB

} BufferFub;

#endif // _SG4

EXOS_ERROR_CODE exos_datamodel_connect_bufferfub(exos_datamodel_handle_t *datamodel, exos_datamodel_event_cb datamodel_event_callback);

#endif // _EXOS_COMP_BUFFERFUB_H_
