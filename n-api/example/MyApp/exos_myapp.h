/*Automatically generated header file from MyApp.typ*/

#ifndef _EXOS_COMP_MYAPP_H_
#define _EXOS_COMP_MYAPP_H_

#ifndef EXOS_INCLUDE_ONLY_DATATYPE
#include "exos_api_internal.h"
#endif

#if defined(_SG4) && !defined(EXOS_STATIC_INCLUDE)
#include <myappTYP.h>
#else
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct MyAppPar_t
{
    int16_t x;
    int16_t y;

} MyAppPar_t;

typedef struct MyAppRes_t
{
    int32_t product;

} MyAppRes_t;

typedef struct MyApp
{
    bool execute; //PUB
    bool done; //PUBSUB
    struct MyAppPar_t parameters[10]; //PUB
    struct MyAppRes_t results[10]; //SUB

} MyApp;

#endif // _SG4 && !EXOS_STATIC_INCLUDE

#ifndef EXOS_INCLUDE_ONLY_DATATYPE
#ifdef EXOS_STATIC_INCLUDE
EXOS_ERROR_CODE exos_datamodel_connect_myapp(exos_datamodel_handle_t *datamodel, exos_datamodel_event_cb datamodel_event_callback);
#else
const char config_myapp[] = "{\"name\":\"struct\",\"attributes\":{\"name\":\"<NAME>\",\"nodeId\":\"\",\"dataType\":\"MyApp\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId0>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"execute\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"PUB\",\"arraySize\":0,\"info\":\"<infoId1>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"done\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"PUBSUB\",\"arraySize\":0,\"info\":\"<infoId2>\"}},{\"name\":\"struct\",\"attributes\":{\"name\":\"parameters\",\"nodeId\":\"\",\"dataType\":\"MyAppPar_t\",\"comment\":\"PUB\",\"arraySize\":10,\"info\":\"<infoId3>\",\"info2\":\"<infoId4>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"x\",\"nodeId\":\"\",\"dataType\":\"INT\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId5>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"y\",\"nodeId\":\"\",\"dataType\":\"INT\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId6>\"}}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"results\",\"nodeId\":\"\",\"dataType\":\"MyAppRes_t\",\"comment\":\"SUB\",\"arraySize\":10,\"info\":\"<infoId7>\",\"info2\":\"<infoId8>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"product\",\"nodeId\":\"\",\"dataType\":\"DINT\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId9>\"}}]}]}";

/*Connect the MyApp datamodel to the server*/
EXOS_ERROR_CODE exos_datamodel_connect_myapp(exos_datamodel_handle_t *datamodel, exos_datamodel_event_cb datamodel_event_callback)
{
    MyApp data;
    exos_dataset_info_t datasets[] = {
        {EXOS_DATASET_BROWSE_NAME_INIT,{}},
        {EXOS_DATASET_BROWSE_NAME(execute),{}},
        {EXOS_DATASET_BROWSE_NAME(done),{}},
        {EXOS_DATASET_BROWSE_NAME(parameters),{}},
        {EXOS_DATASET_BROWSE_NAME(parameters[0]),{10}},
        {EXOS_DATASET_BROWSE_NAME(parameters[0].x),{10}},
        {EXOS_DATASET_BROWSE_NAME(parameters[0].y),{10}},
        {EXOS_DATASET_BROWSE_NAME(results),{}},
        {EXOS_DATASET_BROWSE_NAME(results[0]),{10}},
        {EXOS_DATASET_BROWSE_NAME(results[0].product),{10}}
    };

    _exos_internal_calc_offsets(datasets,sizeof(datasets));

    return _exos_internal_datamodel_connect(datamodel, config_myapp, datasets, sizeof(datasets), datamodel_event_callback);
}

#endif // EXOS_STATIC_INCLUDE
#endif // EXOS_INCLUDE_ONLY_DATATYPE
#endif // _EXOS_COMP_MYAPP_H_
