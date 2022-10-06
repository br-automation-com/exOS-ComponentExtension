/*Automatically generated c file from BufferFub2.typ*/

#include "exos_bufferfub2.h"

const char config_bufferfub2[] = "{\"name\":\"struct\",\"attributes\":{\"name\":\"<NAME>\",\"dataType\":\"BufferFub2\",\"info\":\"<infoId0>\"},\"children\":[{\"name\":\"struct\",\"attributes\":{\"name\":\"bufferedSample\",\"dataType\":\"BufferFub2Sample_typ\",\"comment\":\"SUB\",\"info\":\"<infoId1>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"id\",\"dataType\":\"UDINT\",\"info\":\"<infoId2>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"data\",\"dataType\":\"REAL\",\"info\":\"<infoId3>\"}},{\"name\":\"struct\",\"attributes\":{\"name\":\"subStruct\",\"dataType\":\"BufferFub2SubStruct_typ\",\"info\":\"<infoId4>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"moreid\",\"dataType\":\"UDINT\",\"info\":\"<infoId5>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"moredata\",\"dataType\":\"REAL\",\"info\":\"<infoId6>\"}}]}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"setup\",\"dataType\":\"BufferFub2Setup_typ\",\"comment\":\"PUB\",\"info\":\"<infoId7>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"sampleCount\",\"dataType\":\"UDINT\",\"info\":\"<infoId8>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"sampleDelay\",\"dataType\":\"UDINT\",\"info\":\"<infoId9>\"}}]},{\"name\":\"variable\",\"attributes\":{\"name\":\"cmdSendBurst\",\"dataType\":\"BOOL\",\"comment\":\"PUB\",\"info\":\"<infoId10>\"}}]}";

/*Connect the BufferFub2 datamodel to the server*/
EXOS_ERROR_CODE exos_datamodel_connect_bufferfub2(exos_datamodel_handle_t *datamodel, exos_datamodel_event_cb datamodel_event_callback)
{
    BufferFub2 data;
    exos_dataset_info_t datasets[] = {
        {EXOS_DATASET_BROWSE_NAME_INIT,{}},
        {EXOS_DATASET_BROWSE_NAME(bufferedSample),{}},
        {EXOS_DATASET_BROWSE_NAME(bufferedSample.id),{}},
        {EXOS_DATASET_BROWSE_NAME(bufferedSample.data),{}},
        {EXOS_DATASET_BROWSE_NAME(bufferedSample.subStruct),{}},
        {EXOS_DATASET_BROWSE_NAME(bufferedSample.subStruct.moreid),{}},
        {EXOS_DATASET_BROWSE_NAME(bufferedSample.subStruct.moredata),{}},
        {EXOS_DATASET_BROWSE_NAME(setup),{}},
        {EXOS_DATASET_BROWSE_NAME(setup.sampleCount),{}},
        {EXOS_DATASET_BROWSE_NAME(setup.sampleDelay),{}},
        {EXOS_DATASET_BROWSE_NAME(cmdSendBurst),{}}
    };

    exos_datamodel_calc_dataset_info(datasets, sizeof(datasets));

    return exos_datamodel_connect(datamodel, config_bufferfub2, datasets, sizeof(datasets), datamodel_event_callback);
}
