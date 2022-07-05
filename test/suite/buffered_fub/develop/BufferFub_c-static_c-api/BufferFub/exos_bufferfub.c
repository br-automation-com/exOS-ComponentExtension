/*Automatically generated c file from BufferFub.typ*/

#include "exos_bufferfub.h"

const char config_bufferfub[] = "{\"name\":\"struct\",\"attributes\":{\"name\":\"<NAME>\",\"dataType\":\"BufferFub\",\"info\":\"<infoId0>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"bufferedSample\",\"dataType\":\"UDINT\",\"comment\":\"SUB\",\"info\":\"<infoId1>\"}},{\"name\":\"struct\",\"attributes\":{\"name\":\"setup\",\"dataType\":\"BufferFubSetup_typ\",\"comment\":\"PUB\",\"info\":\"<infoId2>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"sampleCount\",\"dataType\":\"UDINT\",\"info\":\"<infoId3>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"sampleDelay\",\"dataType\":\"UDINT\",\"info\":\"<infoId4>\"}}]},{\"name\":\"variable\",\"attributes\":{\"name\":\"cmdSendBurst\",\"dataType\":\"BOOL\",\"comment\":\"PUB\",\"info\":\"<infoId5>\"}}]}";

/*Connect the BufferFub datamodel to the server*/
EXOS_ERROR_CODE exos_datamodel_connect_bufferfub(exos_datamodel_handle_t *datamodel, exos_datamodel_event_cb datamodel_event_callback)
{
    BufferFub data;
    exos_dataset_info_t datasets[] = {
        {EXOS_DATASET_BROWSE_NAME_INIT,{}},
        {EXOS_DATASET_BROWSE_NAME(bufferedSample),{}},
        {EXOS_DATASET_BROWSE_NAME(setup),{}},
        {EXOS_DATASET_BROWSE_NAME(setup.sampleCount),{}},
        {EXOS_DATASET_BROWSE_NAME(setup.sampleDelay),{}},
        {EXOS_DATASET_BROWSE_NAME(cmdSendBurst),{}}
    };

    exos_datamodel_calc_dataset_info(datasets, sizeof(datasets));

    return exos_datamodel_connect(datamodel, config_bufferfub, datasets, sizeof(datasets), datamodel_event_callback);
}
