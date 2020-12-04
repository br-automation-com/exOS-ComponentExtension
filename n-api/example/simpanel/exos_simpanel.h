/*Automatically generated header file from Types.typ*/

#ifndef _SIMPANEL_H_
#define _SIMPANEL_H_

#include "exos_api_internal.h"

#ifdef _SG4
#include <simpanel.h>
#else
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
typedef struct SimPanelSwitches
{
    bool DI1;
    bool DI2;

} SimPanelSwitches;

typedef struct SimPanelButtons
{
    bool DI3;
    bool DI4;
    bool DI5;
    bool DI6;
    bool Encoder;

} SimPanelButtons;

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

typedef struct SimPanelKnobs
{
    int16_t P1;
    int16_t P2;

} SimPanelKnobs;

typedef struct SimPanel
{
    struct SimPanelSwitches Switches;
    struct SimPanelButtons Buttons;
    struct SimPanelLEDs LEDs;
    struct SimPanelKnobs Knobs;
    int16_t Display;
    uint16_t Encoder;

} SimPanel;

#endif // _SG4

const char config_simpanel[] = "{\"name\":\"struct\",\"attributes\":{\"name\":\"<NAME>\",\"nodeId\":\"\",\"dataType\":\"SimPanel\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId0>\"},\"children\":[{\"name\":\"struct\",\"attributes\":{\"name\":\"Switches\",\"nodeId\":\"\",\"dataType\":\"SimPanelSwitches\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId1>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"DI1\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId2>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"DI2\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId3>\"}}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"Buttons\",\"nodeId\":\"\",\"dataType\":\"SimPanelButtons\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId4>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"DI3\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId5>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"DI4\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId6>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"DI5\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId7>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"DI6\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId8>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Encoder\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId9>\"}}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"LEDs\",\"nodeId\":\"\",\"dataType\":\"SimPanelLEDs\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId10>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"DI1\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId11>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"DI2\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId12>\"}},{\"name\":\"struct\",\"attributes\":{\"name\":\"DI3\",\"nodeId\":\"\",\"dataType\":\"SimPanelLED\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId13>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"Green\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId14>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Red\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId15>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Yellow\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId16>\"}}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"DI4\",\"nodeId\":\"\",\"dataType\":\"SimPanelLED\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId17>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"Green\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId18>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Red\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId19>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Yellow\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId20>\"}}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"DI5\",\"nodeId\":\"\",\"dataType\":\"SimPanelLED\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId21>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"Green\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId22>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Red\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId23>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Yellow\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId24>\"}}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"DI6\",\"nodeId\":\"\",\"dataType\":\"SimPanelLED\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId25>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"Green\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId26>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Red\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId27>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Yellow\",\"nodeId\":\"\",\"dataType\":\"BOOL\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId28>\"}}]}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"Knobs\",\"nodeId\":\"\",\"dataType\":\"SimPanelKnobs\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId29>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"P1\",\"nodeId\":\"\",\"dataType\":\"INT\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId30>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"P2\",\"nodeId\":\"\",\"dataType\":\"INT\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId31>\"}}]},{\"name\":\"variable\",\"attributes\":{\"name\":\"Display\",\"nodeId\":\"\",\"dataType\":\"INT\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId32>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"Encoder\",\"nodeId\":\"\",\"dataType\":\"UINT\",\"comment\":\"\",\"arraySize\":0,\"info\":\"<infoId33>\"}}]}";

/*Register this artefact on the Server and create the OPCUA structure for the SimPanel structure*/
EXOS_ERROR_CODE exos_artefact_register_simpanel(exos_artefact_handle_t *artefact, exos_connection_changed_cb connection_changed)
{
    SimPanel data;
    exos_info_t info[] = {
        {EXOS_INFO(data),{}},
        {EXOS_INFO(data.Switches),{}},
        {EXOS_INFO(data.Switches.DI1),{}},
        {EXOS_INFO(data.Switches.DI2),{}},
        {EXOS_INFO(data.Buttons),{}},
        {EXOS_INFO(data.Buttons.DI3),{}},
        {EXOS_INFO(data.Buttons.DI4),{}},
        {EXOS_INFO(data.Buttons.DI5),{}},
        {EXOS_INFO(data.Buttons.DI6),{}},
        {EXOS_INFO(data.Buttons.Encoder),{}},
        {EXOS_INFO(data.LEDs),{}},
        {EXOS_INFO(data.LEDs.DI1),{}},
        {EXOS_INFO(data.LEDs.DI2),{}},
        {EXOS_INFO(data.LEDs.DI3),{}},
        {EXOS_INFO(data.LEDs.DI3.Green),{}},
        {EXOS_INFO(data.LEDs.DI3.Red),{}},
        {EXOS_INFO(data.LEDs.DI3.Yellow),{}},
        {EXOS_INFO(data.LEDs.DI4),{}},
        {EXOS_INFO(data.LEDs.DI4.Green),{}},
        {EXOS_INFO(data.LEDs.DI4.Red),{}},
        {EXOS_INFO(data.LEDs.DI4.Yellow),{}},
        {EXOS_INFO(data.LEDs.DI5),{}},
        {EXOS_INFO(data.LEDs.DI5.Green),{}},
        {EXOS_INFO(data.LEDs.DI5.Red),{}},
        {EXOS_INFO(data.LEDs.DI5.Yellow),{}},
        {EXOS_INFO(data.LEDs.DI6),{}},
        {EXOS_INFO(data.LEDs.DI6.Green),{}},
        {EXOS_INFO(data.LEDs.DI6.Red),{}},
        {EXOS_INFO(data.LEDs.DI6.Yellow),{}},
        {EXOS_INFO(data.Knobs),{}},
        {EXOS_INFO(data.Knobs.P1),{}},
        {EXOS_INFO(data.Knobs.P2),{}},
        {EXOS_INFO(data.Display),{}},
        {EXOS_INFO(data.Encoder),{}}
    };

    _exos_internal_calc_offsets(info,sizeof(info));

    return _exos_internal_artefact_register(artefact, config_simpanel, info, sizeof(info), connection_changed);
}

#endif // _SIMPANEL_H_
