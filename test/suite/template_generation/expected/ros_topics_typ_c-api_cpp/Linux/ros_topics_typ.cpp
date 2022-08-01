#include <string>
#include <csignal>
#include "ros_topics_typDatamodel.hpp"
#include "termination.h"

/* datamodel features:

main methods:
    ros_topics_typ_datamodel.connect()
    ros_topics_typ_datamodel.disconnect()
    ros_topics_typ_datamodel.process()
    ros_topics_typ_datamodel.setOperational()
    ros_topics_typ_datamodel.dispose()
    ros_topics_typ_datamodel.getNettime() : (int32_t) get current nettime

void(void) user lambda callback:
    ros_topics_typ_datamodel.onConnectionChange([&] () {
        // ros_topics_typ_datamodel.connectionState ...
    })

boolean values:
    ros_topics_typ_datamodel.isConnected
    ros_topics_typ_datamodel.isOperational

logging methods:
    ros_topics_typ_datamodel.log.error << "some value:" << 1 << std::endl;
    ros_topics_typ_datamodel.log.warning << "some value:" << 1 << std::endl;
    ros_topics_typ_datamodel.log.success << "some value:" << 1 << std::endl;
    ros_topics_typ_datamodel.log.info << "some value:" << 1 << std::endl;
    ros_topics_typ_datamodel.log.debug << "some value:" << 1 << std::endl;
    ros_topics_typ_datamodel.log.verbose << "some value:" << 1 << std::endl;

dataset odemetry:
    ros_topics_typ_datamodel.odemetry.onChange([&] () {
        ros_topics_typ_datamodel.odemetry.value ...
    })
    ros_topics_typ_datamodel.odemetry.nettime : (int32_t) nettime @ time of publish
    ros_topics_typ_datamodel.odemetry.value : (ros_topic_odemety_typ)  actual dataset values

dataset twist:
    ros_topics_typ_datamodel.twist.publish()
    ros_topics_typ_datamodel.twist.value : (ros_topic_twist_typ)  actual dataset values

dataset config:
    ros_topics_typ_datamodel.config.publish()
    ros_topics_typ_datamodel.config.value : (ros_config_typ)  actual dataset values
*/



int main(int argc, char ** argv)
{
    catch_termination();
    
    ros_topics_typDatamodel ros_topics_typ_datamodel;
    ros_topics_typ_datamodel.connect();
    
    ros_topics_typ_datamodel.onConnectionChange([&] () {
        if (ros_topics_typ_datamodel.connectionState == EXOS_STATE_CONNECTED) {
            // Datamodel connected
        }
        else if (ros_topics_typ_datamodel.connectionState == EXOS_STATE_DISCONNECTED) {    
            // Datamodel disconnected
        }
    });

    ros_topics_typ_datamodel.odemetry.onChange([&] () {
        // ros_topics_typ_datamodel.odemetry.value ...
    });


    while(!is_terminated()) {
        // trigger callbacks
        ros_topics_typ_datamodel.process();
        
        // publish datasets
        
        if (ros_topics_typ_datamodel.isConnected) {
            // ros_topics_typ_datamodel.twist.value = ...
            // ros_topics_typ_datamodel.twist.publish();
            
            // ros_topics_typ_datamodel.config.value = ...
            // ros_topics_typ_datamodel.config.publish();
            
        }
    }

    return 0;
}
