/*Automatically generated header file from blaha.typ*/

#ifndef _EXOS_COMP_NODEJS_H_
#define _EXOS_COMP_NODEJS_H_

#ifndef EXOS_INCLUDE_ONLY_DATATYPE
#include "exos_api_internal.h"
#endif

#if defined(_SG4) && !defined(EXOS_STATIC_INCLUDE)
#include <blahaTYP.h>
#else
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct NodeJS
{
    bool start; //PUB
    bool reset; //PUB
    int32_t countUp; //SUB
    int32_t countDown; //SUB

} NodeJS;

#endif // _SG4 && !EXOS_STATIC_INCLUDE

#ifndef EXOS_INCLUDE_ONLY_DATATYPE
#ifdef EXOS_STATIC_INCLUDE
EXOS_ERROR_CODE exos_datamodel_connect_nodejs(exos_datamodel_handle_t *datamodel, exos_datamodel_event_cb datamodel_event_callback);
#else
const char config_nodejs[] = "{\"name\":\"struct\",\"attributes\":{\"name\":\"<NAME>\",\"nodeId\":\"\",\"dataType\":\"NodeJS\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId0>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"start\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"PUB\",\"arraySize\":0,\"info\":\"<infoId1>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"reset\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"PUB\",\"arraySize\":0,\"info\":\"<infoId2>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"countUp\",\"nodeId\":\"\",\"dataType\":\"DINT\",\"comment\":\"SUB\",\"arraySize\":0,\"info\":\"<infoId3>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"countDown\",\"nodeId\":\"\",\"dataType\":\"DINT\",\"comment\":\"SUB\",\"arraySize\":0,\"info\":\"<infoId4>\"}}]}";

/*Connect the NodeJS datamodel to the server*/
EXOS_ERROR_CODE exos_datamodel_connect_nodejs(exos_datamodel_handle_t *datamodel, exos_datamodel_event_cb datamodel_event_callback)
{
    NodeJS data;
    exos_dataset_info_t datasets[] = {
        {EXOS_DATASET_BROWSE_NAME_INIT,{}},
        {EXOS_DATASET_BROWSE_NAME(start),{}},
        {EXOS_DATASET_BROWSE_NAME(reset),{}},
        {EXOS_DATASET_BROWSE_NAME(countUp),{}},
        {EXOS_DATASET_BROWSE_NAME(countDown),{}}
    };

    _exos_internal_calc_offsets(datasets,sizeof(datasets));

    return _exos_internal_datamodel_connect(datamodel, config_nodejs, datasets, sizeof(datasets), datamodel_event_callback);
}

#endif // EXOS_STATIC_INCLUDE
#endif // EXOS_INCLUDE_ONLY_DATATYPE
#endif // _EXOS_COMP_NODEJS_H_
