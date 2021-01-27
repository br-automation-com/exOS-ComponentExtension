/*Automatically generated header file from DataX.typ*/

#ifndef _EXOS_COMP_DATAX_H_
#define _EXOS_COMP_DATAX_H_

#ifndef EXOS_INCLUDE_ONLY_DATATYPE
#include "exos_api_internal.h"
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct DataX
{
    bool enable; //PUB
    bool active; //SUB
    int32_t countUp; //SUB
    int32_t countDown; //SUB

} DataX;

#ifndef EXOS_INCLUDE_ONLY_DATATYPE
#ifdef EXOS_STATIC_INCLUDE
EXOS_ERROR_CODE exos_datamodel_connect_datax(exos_datamodel_handle_t *datamodel, exos_datamodel_event_cb datamodel_event_callback);
#else
const char config_datax[] = "{\"name\":\"struct\",\"attributes\":{\"name\":\"<NAME>\",\"dataType\":\"DataX\",\"info\":\"<infoId0>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"enable\",\"dataType\":\"BOOL\",\"comment\":\"PUB\",\"info\":\"<infoId1>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"active\",\"dataType\":\"BOOL\",\"comment\":\" SUB\",\"info\":\"<infoId2>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"countUp\",\"dataType\":\"DINT\",\"comment\":\" SUB\",\"info\":\"<infoId3>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"countDown\",\"dataType\":\"DINT\",\"comment\":\" SUB\",\"info\":\"<infoId4>\"}}]}";

/*Connect the DataX datamodel to the server*/
EXOS_ERROR_CODE exos_datamodel_connect_datax(exos_datamodel_handle_t *datamodel, exos_datamodel_event_cb datamodel_event_callback)
{
    DataX data;
    exos_dataset_info_t datasets[] = {
        {EXOS_DATASET_BROWSE_NAME_INIT,{}},
        {EXOS_DATASET_BROWSE_NAME(enable),{}},
        {EXOS_DATASET_BROWSE_NAME(active),{}},
        {EXOS_DATASET_BROWSE_NAME(countUp),{}},
        {EXOS_DATASET_BROWSE_NAME(countDown),{}}
    };

    _exos_internal_calc_offsets(datasets,sizeof(datasets));

    return _exos_internal_datamodel_connect(datamodel, config_datax, datasets, sizeof(datasets), datamodel_event_callback);
}

#endif // EXOS_STATIC_INCLUDE
#endif // EXOS_INCLUDE_ONLY_DATATYPE
#endif // _EXOS_COMP_DATAX_H_
