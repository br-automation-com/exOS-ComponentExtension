/*Automatically generated c file from ros_topics_typ.typ*/

#include "exos_ros_topics_typ.h"

const char config_ros_topics_typ[] = "{\"name\":\"struct\",\"attributes\":{\"name\":\"<NAME>\",\"dataType\":\"ros_topics_typ\",\"info\":\"<infoId0>\"},\"children\":[{\"name\":\"struct\",\"attributes\":{\"name\":\"odemetry\",\"dataType\":\"ros_topic_odemety_typ\",\"comment\":\"PUB\",\"info\":\"<infoId1>\"},\"children\":[{\"name\":\"struct\",\"attributes\":{\"name\":\"pose\",\"dataType\":\"ros_topic_poseCor_typ\",\"comment\":\"PUB\",\"info\":\"<infoId2>\"},\"children\":[{\"name\":\"struct\",\"attributes\":{\"name\":\"pose\",\"dataType\":\"ros_topic_pose_typ\",\"info\":\"<infoId3>\"},\"children\":[{\"name\":\"struct\",\"attributes\":{\"name\":\"position\",\"dataType\":\"ros_topic_point_typ\",\"info\":\"<infoId4>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"y\",\"dataType\":\"LREAL\",\"info\":\"<infoId5>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"z\",\"dataType\":\"LREAL\",\"info\":\"<infoId6>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"x\",\"dataType\":\"LREAL\",\"info\":\"<infoId7>\"}}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"orientation\",\"dataType\":\"ros_topic_quaternion_typ\",\"info\":\"<infoId8>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"y\",\"dataType\":\"LREAL\",\"info\":\"<infoId9>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"z\",\"dataType\":\"LREAL\",\"info\":\"<infoId10>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"w\",\"dataType\":\"LREAL\",\"info\":\"<infoId11>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"x\",\"dataType\":\"LREAL\",\"info\":\"<infoId12>\"}}]}]},{\"name\":\"variable\",\"attributes\":{\"name\":\"covariance\",\"dataType\":\"LREAL\",\"arraySize\":64,\"info\":\"<infoId13>\",\"info2\":\"<infoId14>\"}}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"twist\",\"dataType\":\"ros_topic_twistCor_typ\",\"comment\":\"PUB\",\"info\":\"<infoId15>\"},\"children\":[{\"name\":\"struct\",\"attributes\":{\"name\":\"twist\",\"dataType\":\"ros_topic_twist_typ\",\"info\":\"<infoId16>\"},\"children\":[{\"name\":\"struct\",\"attributes\":{\"name\":\"angular\",\"dataType\":\"ros_topic_vector3_typ\",\"comment\":\"SUB\",\"info\":\"<infoId17>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"y\",\"dataType\":\"LREAL\",\"info\":\"<infoId18>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"z\",\"dataType\":\"LREAL\",\"info\":\"<infoId19>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"x\",\"dataType\":\"LREAL\",\"info\":\"<infoId20>\"}}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"linear\",\"dataType\":\"ros_topic_vector3_typ\",\"comment\":\"SUB\",\"info\":\"<infoId21>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"y\",\"dataType\":\"LREAL\",\"info\":\"<infoId22>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"z\",\"dataType\":\"LREAL\",\"info\":\"<infoId23>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"x\",\"dataType\":\"LREAL\",\"info\":\"<infoId24>\"}}]}]},{\"name\":\"variable\",\"attributes\":{\"name\":\"covariance\",\"dataType\":\"LREAL\",\"arraySize\":64,\"info\":\"<infoId25>\",\"info2\":\"<infoId26>\"}}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"header\",\"dataType\":\"ros_topic_header_typ\",\"comment\":\"PUB\",\"info\":\"<infoId27>\"},\"children\":[{\"name\":\"struct\",\"attributes\":{\"name\":\"stamp\",\"dataType\":\"ros_topic_time_typ\",\"info\":\"<infoId28>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"nsec\",\"dataType\":\"UDINT\",\"info\":\"<infoId29>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"sec\",\"dataType\":\"UDINT\",\"info\":\"<infoId30>\"}}]},{\"name\":\"variable\",\"attributes\":{\"name\":\"seq\",\"dataType\":\"UDINT\",\"info\":\"<infoId31>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"frame_id\",\"dataType\":\"STRING\",\"stringLength\":21,\"info\":\"<infoId32>\"}}]}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"twist\",\"dataType\":\"ros_topic_twist_typ\",\"comment\":\"SUB\",\"info\":\"<infoId33>\"},\"children\":[{\"name\":\"struct\",\"attributes\":{\"name\":\"angular\",\"dataType\":\"ros_topic_vector3_typ\",\"comment\":\"SUB\",\"info\":\"<infoId34>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"y\",\"dataType\":\"LREAL\",\"info\":\"<infoId35>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"z\",\"dataType\":\"LREAL\",\"info\":\"<infoId36>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"x\",\"dataType\":\"LREAL\",\"info\":\"<infoId37>\"}}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"linear\",\"dataType\":\"ros_topic_vector3_typ\",\"comment\":\"SUB\",\"info\":\"<infoId38>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"y\",\"dataType\":\"LREAL\",\"info\":\"<infoId39>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"z\",\"dataType\":\"LREAL\",\"info\":\"<infoId40>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"x\",\"dataType\":\"LREAL\",\"info\":\"<infoId41>\"}}]}]},{\"name\":\"struct\",\"attributes\":{\"name\":\"config\",\"dataType\":\"ros_config_typ\",\"comment\":\"SUB\",\"info\":\"<infoId42>\"},\"children\":[{\"name\":\"variable\",\"attributes\":{\"name\":\"maxSpeed\",\"dataType\":\"LREAL\",\"comment\":\"SUB\",\"info\":\"<infoId43>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"minSpeed\",\"dataType\":\"LREAL\",\"comment\":\"SUB\",\"info\":\"<infoId44>\"}},{\"name\":\"variable\",\"attributes\":{\"name\":\"baseWidth\",\"dataType\":\"LREAL\",\"comment\":\"SUB\",\"info\":\"<infoId45>\"}}]}]}";

/*Connect the ros_topics_typ datamodel to the server*/
EXOS_ERROR_CODE exos_datamodel_connect_ros_topics_typ(exos_datamodel_handle_t *datamodel, exos_datamodel_event_cb datamodel_event_callback)
{
    ros_topics_typ data;
    exos_dataset_info_t datasets[] = {
        {EXOS_DATASET_BROWSE_NAME_INIT,{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.pose),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.pose.pose),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.pose.pose.position),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.pose.pose.position.y),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.pose.pose.position.z),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.pose.pose.position.x),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.pose.pose.orientation),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.pose.pose.orientation.y),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.pose.pose.orientation.z),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.pose.pose.orientation.w),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.pose.pose.orientation.x),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.pose.covariance),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.pose.covariance[0]),{64}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.twist),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.twist.twist),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.twist.twist.angular),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.twist.twist.angular.y),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.twist.twist.angular.z),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.twist.twist.angular.x),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.twist.twist.linear),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.twist.twist.linear.y),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.twist.twist.linear.z),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.twist.twist.linear.x),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.twist.covariance),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.twist.covariance[0]),{64}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.header),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.header.stamp),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.header.stamp.nsec),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.header.stamp.sec),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.header.seq),{}},
        {EXOS_DATASET_BROWSE_NAME(odemetry.header.frame_id),{}},
        {EXOS_DATASET_BROWSE_NAME(twist),{}},
        {EXOS_DATASET_BROWSE_NAME(twist.angular),{}},
        {EXOS_DATASET_BROWSE_NAME(twist.angular.y),{}},
        {EXOS_DATASET_BROWSE_NAME(twist.angular.z),{}},
        {EXOS_DATASET_BROWSE_NAME(twist.angular.x),{}},
        {EXOS_DATASET_BROWSE_NAME(twist.linear),{}},
        {EXOS_DATASET_BROWSE_NAME(twist.linear.y),{}},
        {EXOS_DATASET_BROWSE_NAME(twist.linear.z),{}},
        {EXOS_DATASET_BROWSE_NAME(twist.linear.x),{}},
        {EXOS_DATASET_BROWSE_NAME(config),{}},
        {EXOS_DATASET_BROWSE_NAME(config.maxSpeed),{}},
        {EXOS_DATASET_BROWSE_NAME(config.minSpeed),{}},
        {EXOS_DATASET_BROWSE_NAME(config.baseWidth),{}}
    };

    exos_datamodel_calc_dataset_info(datasets, sizeof(datasets));

    return exos_datamodel_connect(datamodel, config_ros_topics_typ, datasets, sizeof(datasets), datamodel_event_callback);
}
