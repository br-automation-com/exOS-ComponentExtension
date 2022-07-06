#ifndef _LIBBUFFERFUB_H_
#define _LIBBUFFERFUB_H_

#include "exos_bufferfub.h"

typedef void (*libBufferFub_event_cb)(void);
typedef void (*libBufferFub_method_fn)(void);
typedef int32_t (*libBufferFub_get_nettime_fn)(void);
typedef void (*libBufferFub_log_fn)(char *log_entry);

typedef struct libBufferFubbufferedSample
{
    libBufferFub_event_cb on_change;
    int32_t nettime;
    uint32_t value;
} libBufferFubbufferedSample_t;

typedef struct libBufferFubsetup
{
    libBufferFub_method_fn publish;
    BufferFubSetup_typ value;
} libBufferFubsetup_t;

typedef struct libBufferFubcmdSendBurst
{
    libBufferFub_method_fn publish;
    bool value;
} libBufferFubcmdSendBurst_t;

typedef struct libBufferFub_log
{
    libBufferFub_log_fn error;
    libBufferFub_log_fn warning;
    libBufferFub_log_fn success;
    libBufferFub_log_fn info;
    libBufferFub_log_fn debug;
    libBufferFub_log_fn verbose;
} libBufferFub_log_t;

typedef struct libBufferFub
{
    libBufferFub_method_fn connect;
    libBufferFub_method_fn disconnect;
    libBufferFub_method_fn process;
    libBufferFub_method_fn set_operational;
    libBufferFub_method_fn dispose;
    libBufferFub_get_nettime_fn get_nettime;
    libBufferFub_log_t log;
    libBufferFub_event_cb on_connected;
    libBufferFub_event_cb on_disconnected;
    libBufferFub_event_cb on_operational;
    bool is_connected;
    bool is_operational;
    libBufferFubbufferedSample_t bufferedSample;
    libBufferFubsetup_t setup;
    libBufferFubcmdSendBurst_t cmdSendBurst;
} libBufferFub_t;

#ifdef __cplusplus
extern "C" {
#endif
libBufferFub_t *libBufferFub_init(void);
#ifdef __cplusplus
}
#endif
#endif // _LIBBUFFERFUB_H_
