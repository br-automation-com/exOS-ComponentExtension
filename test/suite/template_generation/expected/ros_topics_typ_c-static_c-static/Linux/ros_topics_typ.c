#include <unistd.h>
#include "libros_topics_typ.h"
#include "termination.h"
#include <stdio.h>

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
    ros_topics_typ_datamodel->odemetry.on_change : void(void) user callback function
    ros_topics_typ_datamodel->odemetry.nettime : (int32_t) nettime @ time of publish
    ros_topics_typ_datamodel->odemetry.value : (ros_topic_odemety_typ)  actual dataset values

dataset twist:
    ros_topics_typ_datamodel->twist.publish()
    ros_topics_typ_datamodel->twist.value : (ros_topic_twist_typ)  actual dataset values

dataset config:
    ros_topics_typ_datamodel->config.publish()
    ros_topics_typ_datamodel->config.value : (ros_config_typ)  actual dataset values
*/

static libros_topics_typ_t *ros_topics_typ_datamodel;

static void on_connected_ros_topics_typ_datamodel(void)
{
   ros_topics_typ_datamodel->log.success("ros_topics_typ_datamodel connected!");
}

static void on_change_odemetry_dataset(void)
{
   ros_topics_typ_datamodel->log.verbose("ros_topics_typ_datamodel->odemetry changed!");
   // printf("on_change: ros_topics_typ_datamodel->odemetry: 0x%.8x\n", ros_topics_typ_datamodel->odemetry.value);

   // Your code here...
}

int main()
{
    //retrieve the ros_topics_typ_datamodel structure
    ros_topics_typ_datamodel = libros_topics_typ_init();

    //setup callbacks
    ros_topics_typ_datamodel->on_connected = on_connected_ros_topics_typ_datamodel;
    // ros_topics_typ_datamodel->on_disconnected = .. ;
    // ros_topics_typ_datamodel->on_operational = .. ;
    ros_topics_typ_datamodel->odemetry.on_change = on_change_odemetry_dataset;

    //connect to the server
    ros_topics_typ_datamodel->connect();

    catch_termination();
    while (!is_terminated())
    {
        //trigger callbacks and synchronize with AR
        ros_topics_typ_datamodel->process();

        // if (ros_topics_typ_datamodel->is_connected)
        // {
        //     ros_topics_typ_datamodel->twist.value. .. = .. ;
        //     ros_topics_typ_datamodel->twist.publish();
        
        //     ros_topics_typ_datamodel->config.value. .. = .. ;
        //     ros_topics_typ_datamodel->config.publish();
        
        // }
    }

    //shutdown
    ros_topics_typ_datamodel->disconnect();
    ros_topics_typ_datamodel->dispose();

    return 0;
}
