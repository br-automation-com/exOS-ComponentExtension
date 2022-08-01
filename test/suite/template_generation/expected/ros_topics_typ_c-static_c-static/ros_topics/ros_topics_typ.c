#include <string.h>
#include <stdbool.h>
#include "libros_topics_typ.h"

/* libros_topics_typ_t datamodel features:

main methods:
    ros_topics_typ_datamodel->connect()
    ros_topics_typ_datamodel->disconnect()
    ros_topics_typ_datamodel->process()
    ros_topics_typ_datamodel->set_operational()
    ros_topics_typ_datamodel->dispose()
    ros_topics_typ_datamodel->get_nettime() : (int32_t) get current nettime

void(void) user callbacks:
    ros_topics_typ_datamodel->on_connected
    ros_topics_typ_datamodel->on_disconnected
    ros_topics_typ_datamodel->on_operational

boolean values:
    ros_topics_typ_datamodel->is_connected
    ros_topics_typ_datamodel->is_operational

logging methods:
    ros_topics_typ_datamodel->log.error(char *)
    ros_topics_typ_datamodel->log.warning(char *)
    ros_topics_typ_datamodel->log.success(char *)
    ros_topics_typ_datamodel->log.info(char *)
    ros_topics_typ_datamodel->log.debug(char *)
    ros_topics_typ_datamodel->log.verbose(char *)

dataset odemetry:
    ros_topics_typ_datamodel->odemetry.publish()
    ros_topics_typ_datamodel->odemetry.value : (ros_topic_odemety_typ)  actual dataset values

dataset twist:
    ros_topics_typ_datamodel->twist.on_change : void(void) user callback function
    ros_topics_typ_datamodel->twist.nettime : (int32_t) nettime @ time of publish
    ros_topics_typ_datamodel->twist.value : (ros_topic_twist_typ)  actual dataset values

dataset config:
    ros_topics_typ_datamodel->config.on_change : void(void) user callback function
    ros_topics_typ_datamodel->config.nettime : (int32_t) nettime @ time of publish
    ros_topics_typ_datamodel->config.value : (ros_config_typ)  actual dataset values
*/

static libros_topics_typ_t *ros_topics_typ_datamodel;
static struct ros_topics_typCyclic *cyclic_inst;

static void on_connected_ros_topics_typ_datamodel(void)
{
}

static void on_change_twist_dataset(void)
{
    memcpy(&(cyclic_inst->pros_topics_typ->twist), &(ros_topics_typ_datamodel->twist.value), sizeof(cyclic_inst->pros_topics_typ->twist));
    
    // Your code here...
}
static void on_change_config_dataset(void)
{
    memcpy(&(cyclic_inst->pros_topics_typ->config), &(ros_topics_typ_datamodel->config.value), sizeof(cyclic_inst->pros_topics_typ->config));
    
    // Your code here...
}
_BUR_PUBLIC void ros_topics_typCyclic(struct ros_topics_typCyclic *inst)
{
    // check if function block has been created before
    if(cyclic_inst != NULL)
    {
        // return error if more than one function blocks have been created
        if(inst != cyclic_inst)
        {
            inst->Operational = false;
            inst->Connected = false;
            inst->Error = true;
            return;
        }
    }
    cyclic_inst = inst;
    // initialize library
    if((libros_topics_typ_t *)inst->_Handle == NULL || (libros_topics_typ_t *)inst->_Handle != ros_topics_typ_datamodel)
    {
        //retrieve the ros_topics_typ_datamodel structure
        ros_topics_typ_datamodel = libros_topics_typ_init();

        //setup callbacks
        ros_topics_typ_datamodel->on_connected = on_connected_ros_topics_typ_datamodel;
        // ros_topics_typ_datamodel->on_disconnected = .. ;
        // ros_topics_typ_datamodel->on_operational = .. ;
        ros_topics_typ_datamodel->twist.on_change = on_change_twist_dataset;
        ros_topics_typ_datamodel->config.on_change = on_change_config_dataset;

        inst->_Handle = (UDINT)ros_topics_typ_datamodel;
    }
    // return error if reference to structure is not set on function block
    if(inst->pros_topics_typ == NULL)
    {
        inst->Operational = false;
        inst->Connected = false;
        inst->Error = true;
        return;
    }
    if (inst->Enable && !inst->_Enable)
    {
        //connect to the server
        ros_topics_typ_datamodel->connect();
    }
    if (!inst->Enable && inst->_Enable)
    {
        //disconnect from server
        cyclic_inst = NULL;
        ros_topics_typ_datamodel->disconnect();
    }
    inst->_Enable = inst->Enable;

    if(inst->Start && !inst->_Start && ros_topics_typ_datamodel->is_connected)
    {
        ros_topics_typ_datamodel->set_operational();
        inst->_Start = inst->Start;
    }
    if(!inst->Start)
    {
        inst->_Start = false;
    }

    //trigger callbacks
    ros_topics_typ_datamodel->process();

    if (ros_topics_typ_datamodel->is_connected)
    {
        if (memcmp(&(ros_topics_typ_datamodel->odemetry.value), &(inst->pros_topics_typ->odemetry), sizeof(inst->pros_topics_typ->odemetry)))
        {
            memcpy(&(ros_topics_typ_datamodel->odemetry.value), &(inst->pros_topics_typ->odemetry), sizeof(ros_topics_typ_datamodel->odemetry.value));
            ros_topics_typ_datamodel->odemetry.publish();
        }
    
        // Your code here...
    }
    inst->Connected = ros_topics_typ_datamodel->is_connected;
    inst->Operational = ros_topics_typ_datamodel->is_operational;
}

UINT _EXIT ProgramExit(unsigned long phase)
{
    //shutdown
    ros_topics_typ_datamodel->dispose();
    cyclic_inst = NULL;
    return 0;
}
