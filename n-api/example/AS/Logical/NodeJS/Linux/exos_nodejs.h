/*Automatically generated header file from NodeJS.typ*/

#ifndef _EXOS_COMP_NODEJS_H_
#define _EXOS_COMP_NODEJS_H_

#ifndef EXOS_INCLUDE_ONLY_DATATYPE
#include "exos_api_internal.h"
#endif

#if defined(_SG4) && !defined(EXOS_STATIC_INCLUDE)
#include <nodejsTYP.h>
#else
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Counters_typ
{
    int16_t c[11];

} Counters_typ;

typedef struct NodeJS
{
    struct Counters_typ counters; //PUBSUB
    bool run_counter; //PUB

} NodeJS;

#endif // _SG4 && !EXOS_STATIC_INCLUDE

#ifndef EXOS_INCLUDE_ONLY_DATATYPE
#ifdef EXOS_STATIC_INCLUDE
EXOS_ERROR_CODE exos_datamodel_connect_nodejs(exos_datamodel_handle_t *datamodel, exos_datamodel_event_cb datamodel_event_callback);
#else
const char config_nodejs[] = "{\"name\":\"struct\",\"attributes\":{\"name\":\"<NAME>\",\"dataType\":\"NodeJS\",\"info\":\"<infoId0>\"},\"children\":[{\"name\":\"struct\",\"attributes\":{\"name\":\"counters\",\"dataType\":\"Counters_typ\",\"comment\":\"PUB SUB\",\"info\":\"<infoId1>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"c\",\"dataType\":\"INT\",\"arraySize\":11,\"info\":\"<infoId2>\",\"info2\":\"<infoId3>\"}}]},{\"name\":\"variable\",\"attributes\":{\"name\":\"run_counter\",\"dataType\":\"BOOL\",\"comment\":\"PUB\",\"info\":\"<infoId4>\"}}]}";

/*Connect the NodeJS datamodel to the server*/
EXOS_ERROR_CODE exos_datamodel_connect_nodejs(exos_datamodel_handle_t *datamodel, exos_datamodel_event_cb datamodel_event_callback)
{
    NodeJS data;
    exos_dataset_info_t datasets[] = {
        {EXOS_DATASET_BROWSE_NAME_INIT,{}},
        {EXOS_DATASET_BROWSE_NAME(counters),{}},
        {EXOS_DATASET_BROWSE_NAME(counters.c),{}},
        {EXOS_DATASET_BROWSE_NAME(counters.c[0]),{11}},
        {EXOS_DATASET_BROWSE_NAME(run_counter),{}}
    };

    _exos_internal_calc_offsets(datasets,sizeof(datasets));

    return _exos_internal_datamodel_connect(datamodel, config_nodejs, datasets, sizeof(datasets), datamodel_event_callback);
}

#endif // EXOS_STATIC_INCLUDE
#endif // EXOS_INCLUDE_ONLY_DATATYPE
#endif // _EXOS_COMP_NODEJS_H_
