#ifndef _EXOS_API_H_
#define _EXOS_API_H_
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
/**
 * The version of the exos api
 */
#define EXOS_API_VERSION "1.0.2-1"
/*
NUMERIC version
0-1000 build revision
10 000 - 90 000 1-9 patch version (limited from AS)
100 000 - 9 999 999 1-99 minor version
10 000 000 - 100 000 000 1-10 major version
*/
#define EXOS_API_NUMERIC_VERSION 10020001
#ifdef _SG4
#include <bur/plctypes.h>
#else
#define _BUR_PUBLIC
#endif

typedef enum
{
    EXOS_ERROR_OK = 0,
    EXOS_ERROR_NOT_IMPLEMENTED = 5000,
    EXOS_ERROR_PARAMETER_NULL,
    EXOS_ERROR_BAD_DATAMODEL_HANDLE,
    EXOS_ERROR_BAD_DATASET_HANDLE,
    EXOS_ERROR_BAD_LOG_HANDLE,
    EXOS_ERROR_BAD_SYNC_HANDLE,
    EXOS_ERROR_NOT_ALLOWED,
    EXOS_ERROR_NOT_FOUND,
    EXOS_ERROR_STRING_FORMAT,
    EXOS_ERROR_MESSAGE_FORMAT,
    EXOS_ERROR_NO_DATA,
    EXOS_ERROR_BUFFER_OVERFLOW,
    EXOS_ERROR_TIMEOUT,
    EXOS_ERROR_BAD_DATASET_SIZE,
    EXOS_ERROR_USER,
    EXOS_ERROR_SYSTEM,
    EXOS_ERROR_SYSTEM_SOCKET,
    EXOS_ERROR_SYSTEM_SOCKET_USAGE,
    EXOS_ERROR_SYSTEM_MALLOC,
    EXOS_ERROR_SYSTEM_LXI,
    EXOS_ERROR_NOT_READY,
    EXOS_ERROR_SERVER_SHUTDOWN,
    EXOS_ERROR_BAD_STATE
} EXOS_ERROR_CODE;

char *exos_get_error_string(EXOS_ERROR_CODE error);

typedef enum
{
    EXOS_STATE_DISCONNECTED,
    EXOS_STATE_CONNECTED,
    EXOS_STATE_OPERATIONAL,
    EXOS_STATE_ABORTED
} EXOS_CONNECTION_STATE;

char *exos_get_state_string(EXOS_CONNECTION_STATE state);

typedef enum
{
    EXOS_DATAMODEL_EVENT_CONNECTION_CHANGED
} EXOS_DATAMODEL_EVENT_TYPE;

typedef struct exos_datamodel_handle exos_datamodel_handle_t;
typedef void (*exos_datamodel_event_cb)(exos_datamodel_handle_t *datamodel, const EXOS_DATAMODEL_EVENT_TYPE event_type, void *info);

EXOS_ERROR_CODE exos_datamodel_init(exos_datamodel_handle_t *datamodel, const char *datamodel_instance_name, const char *user_alias);

EXOS_ERROR_CODE exos_datamodel_connect_(exos_datamodel_handle_t *datamodel, exos_datamodel_event_cb datamodel_event_callback);
EXOS_ERROR_CODE exos_datamodel_set_operational(exos_datamodel_handle_t *datamodel);
EXOS_ERROR_CODE exos_datamodel_disconnect(exos_datamodel_handle_t *datamodel);
EXOS_ERROR_CODE exos_datamodel_delete(exos_datamodel_handle_t *datamodel);

typedef struct exos_datamodel_private
{
    uint32_t _magic;
    void *_artefact;
    void *_reserved[8];
} exos_datamodel_private_t;

struct exos_datamodel_handle
{
    char *name;
    EXOS_CONNECTION_STATE connection_state;
    EXOS_ERROR_CODE error;
    void *user_context;
    long user_tag;
    char *user_alias;
    exos_datamodel_event_cb datamodel_event_callback;
    void *_reserved[8];
    exos_datamodel_private_t _private;
};

typedef struct exos_dataset_handle exos_dataset_handle_t;

exos_dataset_handle_t *exos_datamodel_get_dataset(exos_datamodel_handle_t *datamodel, exos_dataset_handle_t *previous);
EXOS_ERROR_CODE exos_datamodel_process(exos_datamodel_handle_t *datamodel);
int32_t exos_datamodel_get_nettime(exos_datamodel_handle_t *datamodel, bool *in_sync);

typedef enum
{
    EXOS_DATASET_EVENT_CONNECTION_CHANGED,
    EXOS_DATASET_EVENT_UPDATED,
    EXOS_DATASET_EVENT_PUBLISHED,
    EXOS_DATASET_EVENT_DELIVERED
    //EXOS_DATASET_RECIEVED
} EXOS_DATASET_EVENT_TYPE;

typedef struct exos_buffer_info
{
    uint32_t size;
    uint32_t free;
    uint32_t used;
} exos_send_buffer_info_t;

typedef void (*exos_dataset_event_cb)(exos_dataset_handle_t *dataset, EXOS_DATASET_EVENT_TYPE event_type, void *info);

typedef enum
{
    EXOS_DATASET_SUBSCRIBE = 1,
    EXOS_DATASET_PUBLISH = 16
} EXOS_DATASET_TYPE;

typedef struct exos_dataset_private
{
    uint32_t _magic;
    void *_value;
    void *_reserved[8];
} exos_dataset_private_t;
struct exos_dataset_handle
{
    char *name;
    EXOS_DATASET_TYPE type;
    exos_datamodel_handle_t *datamodel;
    void *data;
    size_t size;
    EXOS_ERROR_CODE error;
    EXOS_CONNECTION_STATE connection_state;
    exos_send_buffer_info_t send_buffer;
    int32_t nettime;
    long user_tag;
    void *user_context;
    exos_dataset_event_cb dataset_event_callback;
    void *_reserved[8];
    exos_dataset_private_t _private;
};

EXOS_ERROR_CODE exos_dataset_init(exos_dataset_handle_t *dataset, exos_datamodel_handle_t *datamodel, const char *browse_name, const void *data, size_t size);

EXOS_ERROR_CODE exos_dataset_connect(exos_dataset_handle_t *dataset, EXOS_DATASET_TYPE type, exos_dataset_event_cb dataset_event_callback);
EXOS_ERROR_CODE exos_dataset_publish(exos_dataset_handle_t *dataset);

EXOS_ERROR_CODE exos_dataset_disconnect(exos_dataset_handle_t *dataset);
EXOS_ERROR_CODE exos_dataset_delete(exos_dataset_handle_t *dataset);

#endif
