/*Automatically generated header file from BufferFub2.typ*/

#ifndef _EXOS_COMP_BUFFERFUB2_H_
#define _EXOS_COMP_BUFFERFUB2_H_

#include "exos_api.h"

#if defined(_SG4)
#include <BufferFub2.h>
#else
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct BufferFub2Setup_typ
{
    uint32_t sampleCount; //number of samples sent each time cmdSendBurst is set
    uint32_t sampleDelay; //ms between samples

} BufferFub2Setup_typ;

typedef struct BufferFub2SubStruct_typ
{
    uint32_t moreid; //id number
    float moredata; //some data

} BufferFub2SubStruct_typ;

typedef struct BufferFub2Sample_typ
{
    uint32_t id; //id number
    float data; //some data
    struct BufferFub2SubStruct_typ subStruct; //a sub structure

} BufferFub2Sample_typ;

typedef struct BufferFub2
{
    struct BufferFub2Sample_typ bufferedSample; //SUB BUFFERED
    struct BufferFub2Setup_typ setup; //PUB
    bool cmdSendBurst; //PUB

} BufferFub2;

#endif // _SG4

EXOS_ERROR_CODE exos_datamodel_connect_bufferfub2(exos_datamodel_handle_t *datamodel, exos_datamodel_event_cb datamodel_event_callback);

#endif // _EXOS_COMP_BUFFERFUB2_H_
