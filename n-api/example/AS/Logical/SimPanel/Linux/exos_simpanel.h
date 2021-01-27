/*Automatically generated header file from SimPanel.typ*/

#ifndef _EXOS_COMP_SIMPANEL_H_
#define _EXOS_COMP_SIMPANEL_H_

#ifndef EXOS_INCLUDE_ONLY_DATATYPE
#include "exos_api_internal.h"
#endif

#if defined(_SG4) && !defined(EXOS_STATIC_INCLUDE)
#include <SimPanel.h>
#else
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct SimPanelKnobs
{
    int16_t P1;
    int16_t P2;

} SimPanelKnobs;

typedef struct SimPanelLED
{
    bool Green;
    bool Red;
    bool Yellow;

} SimPanelLED;

typedef struct SimPanelLEDs
{
    bool DI1;
    bool DI2;
    struct SimPanelLED DI3;
    struct SimPanelLED DI4;
    struct SimPanelLED DI5;
    struct SimPanelLED DI6;

} SimPanelLEDs;

typedef struct SimPanelButtons
{
    bool DI3;
    bool DI4;
    bool DI5;
    bool DI6;
    bool Encoder;

} SimPanelButtons;

typedef struct SimPanelSwitches
{
    bool DI1;
    bool DI2;

} SimPanelSwitches;

typedef struct SimPanel
{
    struct SimPanelSwitches Switches;
    struct SimPanelButtons Buttons;
    struct SimPanelLEDs LEDs;
    struct SimPanelKnobs Knobs; //PUBSUB
    int16_t Display; //SUB
    uint16_t Encoder; //PUB

} SimPanel;

#endif // _SG4 && !EXOS_STATIC_INCLUDE

#ifndef EXOS_INCLUDE_ONLY_DATATYPE
#ifdef EXOS_STATIC_INCLUDE
EXOS_ERROR_CODE exos_datamodel_connect_simpanel(exos_datamodel_handle_t *datamodel, exos_datamodel_event_cb datamodel_event_callback);
#else
const char config_simpanel[] = "{\"name\":\"struct\",\"attributes\":{\"name\":\"<NAME>\",\"dataType\":\"SimPanel\",\"info\":\"<infoId0>\"},\"children\":[{\"name\":\"struct\",\"attributes\":{\"name\":\"Switches\",\"dataType\":\"SimPanelSwitches\",\"info\":\"<infoId1>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"DI1\",\"dataType\":\"BOOL\",\"info\":\"<infoId2>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"DI2\",\"dataType\":\"BOOL\",\"info\":\"<infoId3>\"}}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"Buttons\",\"dataType\":\"SimPanelButtons\",\"info\":\"<infoId4>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"DI3\",\"dataType\":\"BOOL\",\"info\":\"<infoId5>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"DI4\",\"dataType\":\"BOOL\",\"info\":\"<infoId6>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"DI5\",\"dataType\":\"BOOL\",\"info\":\"<infoId7>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"DI6\",\"dataType\":\"BOOL\",\"info\":\"<infoId8>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Encoder\",\"dataType\":\"BOOL\",\"info\":\"<infoId9>\"}}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"LEDs\",\"dataType\":\"SimPanelLEDs\",\"info\":\"<infoId10>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"DI1\",\"dataType\":\"BOOL\",\"info\":\"<infoId11>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"DI2\",\"dataType\":\"BOOL\",\"info\":\"<infoId12>\"}},{\"name\":\"struct\",\"attributes\":{\"name\":\"DI3\",\"dataType\":\"SimPanelLED\",\"info\":\"<infoId13>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"Green\",\"dataType\":\"BOOL\",\"info\":\"<infoId14>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Red\",\"dataType\":\"BOOL\",\"info\":\"<infoId15>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Yellow\",\"dataType\":\"BOOL\",\"info\":\"<infoId16>\"}}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"DI4\",\"dataType\":\"SimPanelLED\",\"info\":\"<infoId17>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"Green\",\"dataType\":\"BOOL\",\"info\":\"<infoId18>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Red\",\"dataType\":\"BOOL\",\"info\":\"<infoId19>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Yellow\",\"dataType\":\"BOOL\",\"info\":\"<infoId20>\"}}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"DI5\",\"dataType\":\"SimPanelLED\",\"info\":\"<infoId21>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"Green\",\"dataType\":\"BOOL\",\"info\":\"<infoId22>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Red\",\"dataType\":\"BOOL\",\"info\":\"<infoId23>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Yellow\",\"dataType\":\"BOOL\",\"info\":\"<infoId24>\"}}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"DI6\",\"dataType\":\"SimPanelLED\",\"info\":\"<infoId25>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"Green\",\"dataType\":\"BOOL\",\"info\":\"<infoId26>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Red\",\"dataType\":\"BOOL\",\"info\":\"<infoId27>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Yellow\",\"dataType\":\"BOOL\",\"info\":\"<infoId28>\"}}]}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"Knobs\",\"dataType\":\"SimPanelKnobs\",\"comment\":\"PUB SUB\",\"info\":\"<infoId29>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"P1\",\"dataType\":\"INT\",\"info\":\"<infoId30>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"P2\",\"dataType\":\"INT\",\"info\":\"<infoId31>\"}}]},{\"name\":\"variable\",\"attributes\":{\"name\":\"Display\",\"dataType\":\"INT\",\"comment\":\" SUB\",\"info\":\"<infoId32>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Encoder\",\"dataType\":\"UINT\",\"comment\":\"PUB\",\"info\":\"<infoId33>\"}}]}";

/*Connect the SimPanel datamodel to the server*/
EXOS_ERROR_CODE exos_datamodel_connect_simpanel(exos_datamodel_handle_t *datamodel, exos_datamodel_event_cb datamodel_event_callback)
{
    SimPanel data;
    exos_dataset_info_t datasets[] = {
        {EXOS_DATASET_BROWSE_NAME_INIT,{}},
        {EXOS_DATASET_BROWSE_NAME(Switches),{}},
        {EXOS_DATASET_BROWSE_NAME(Switches.DI1),{}},
        {EXOS_DATASET_BROWSE_NAME(Switches.DI2),{}},
        {EXOS_DATASET_BROWSE_NAME(Buttons),{}},
        {EXOS_DATASET_BROWSE_NAME(Buttons.DI3),{}},
        {EXOS_DATASET_BROWSE_NAME(Buttons.DI4),{}},
        {EXOS_DATASET_BROWSE_NAME(Buttons.DI5),{}},
        {EXOS_DATASET_BROWSE_NAME(Buttons.DI6),{}},
        {EXOS_DATASET_BROWSE_NAME(Buttons.Encoder),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs.DI1),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs.DI2),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs.DI3),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs.DI3.Green),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs.DI3.Red),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs.DI3.Yellow),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs.DI4),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs.DI4.Green),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs.DI4.Red),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs.DI4.Yellow),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs.DI5),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs.DI5.Green),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs.DI5.Red),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs.DI5.Yellow),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs.DI6),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs.DI6.Green),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs.DI6.Red),{}},
        {EXOS_DATASET_BROWSE_NAME(LEDs.DI6.Yellow),{}},
        {EXOS_DATASET_BROWSE_NAME(Knobs),{}},
        {EXOS_DATASET_BROWSE_NAME(Knobs.P1),{}},
        {EXOS_DATASET_BROWSE_NAME(Knobs.P2),{}},
        {EXOS_DATASET_BROWSE_NAME(Display),{}},
        {EXOS_DATASET_BROWSE_NAME(Encoder),{}}
    };

    _exos_internal_calc_offsets(datasets,sizeof(datasets));

    return _exos_internal_datamodel_connect(datamodel, config_simpanel, datasets, sizeof(datasets), datamodel_event_callback);
}

#endif // EXOS_STATIC_INCLUDE
#endif // EXOS_INCLUDE_ONLY_DATATYPE
#endif // _EXOS_COMP_SIMPANEL_H_
