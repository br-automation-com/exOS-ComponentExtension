#ifndef _ROS_TOPICS_TYPDATAMODEL_H_
#define _ROS_TOPICS_TYPDATAMODEL_H_

#include <string>
#include <iostream>
#include <string.h>
#include <functional>
#include "ros_topics_typDataset.hpp"

class ros_topics_typDatamodel
{
private:
    exos_datamodel_handle_t datamodel = {};
    std::function<void()> _onConnectionChange = [](){};

    void datamodelEvent(exos_datamodel_handle_t *datamodel, const EXOS_DATAMODEL_EVENT_TYPE event_type, void *info);
    static void _datamodelEvent(exos_datamodel_handle_t *datamodel, const EXOS_DATAMODEL_EVENT_TYPE event_type, void *info) {
        ros_topics_typDatamodel* inst = static_cast<ros_topics_typDatamodel*>(datamodel->user_context);
        inst->datamodelEvent(datamodel, event_type, info);
    }

public:
    ros_topics_typDatamodel();
    void process();
    void connect();
    void disconnect();
    void setOperational();
    int getNettime();
    void onConnectionChange(std::function<void()> f) {_onConnectionChange = std::move(f);};

    bool isOperational = false;
    bool isConnected = false;
    EXOS_CONNECTION_STATE connectionState = EXOS_STATE_DISCONNECTED;

    ros_topics_typLogger log;

    ros_topics_typDataset<ros_topic_odemety_typ> odemetry;
    ros_topics_typDataset<ros_topic_twist_typ> twist;
    ros_topics_typDataset<ros_config_typ> config;

    ~ros_topics_typDatamodel();
};

#endif
