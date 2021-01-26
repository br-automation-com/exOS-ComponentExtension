/**
 * 
 * \page exos_log
 * 
 * \section intro_log Introduction
 * 
 * The exos-api comes with the functionality to collect log messages from various programs throughout the system and collect 
 * these in a centralized logger. By default, this is the Automation Runtime logger.
 * 
 * \arg Logs are collected centrally on each system, meaning theres one log-server in Linux, one in AR.
 * 
 * \arg The log-server in Linux forwards all log messages to the AR log-server, where they are written into the AR logger. 
 * Additionally to writing the Linux log messages into the AR Logger, the AR log-server of course also collects all local log 
 * messages from AR applications and writes these to the logger aswell.
 * 
 * \arg The current log filter, meaning log-type, log-level and excluded modules can be configured using the \c exos_log_change_config from AR or Linux. 
 *  This function can be called at any time, and will change the configuration for all modules that log to the system.
 * \code
 * EXOS_ERROR_CODE exos_log_change_config(exos_log_handle_t *log, const exos_log_config_t *config);
 * \endcode
 * If not called, the systems will log using the default log configuration, that is defined via the \c exos-api.conf in Linux and 
 * the \c .exosconf Technology Package module in Automation Studio.
 * \see exos_log_change_config
 * 
 * Due to the mechanism of distributing a log configuration to all modules in the system, Applications that are using the exos_log 
 * will not forward any information if the filters do not match. In other words, the exos_log_xxx will just have a quick return, as filtering occurs locally.
 * 
 * All logs are seperable by the name you pass to the \c exos_log_init function, further the log distiquishes logs from AR and GPOS via the \c Facility in the logger.
 * 
 * \see exos_log.h Interface documentation
 * 
 * \section impl Implementation
 * \arg As a communication mechanism, there are two zmq queues on each system that sends and receives logdata and configuration.
 * With this, each module is also be able to receive this configuration (log-level..) which is broadcaseted by the log-server on the system
 * 
 * \arg For the reasons mentioned above, each module should have an init function 
 * \code
 * EXOS_ERROR_CODE exos_log_init(exos_log_handle_t *log, const char *module_name);
 * \endcode
 * 
 * \arg Further, on order to receive configuration (and activate the local logger), you need a cyclic recv function
 * \code 
 * EXOS_ERROR_CODE exos_log_process(const exos_log_handle_t *log);
 * \endcode
 * 
 * \arg To log at a certain severity level, there is functions on each level for logging
 * \code
 * EXOS_ERROR_CODE exos_log_error(const exos_log_handle_t *log, char *format, ...);
 * EXOS_ERROR_CODE exos_log_success(const exos_log_handle_t *log, EXOS_LOG_TYPE type, char *format, ...);
 * EXOS_ERROR_CODE exos_log_info(const exos_log_handle_t *log, EXOS_LOG_TYPE type, char *format, ...);
 * EXOS_ERROR_CODE exos_log_debug(const exos_log_handle_t *log, EXOS_LOG_TYPE type, char *format, ...);
 * \endcode
 * 
 * \section defines Local Defines
 * 
 * The functions \c exos_log_error, \c exos_log_success are rather cumbersome to write, having to pass a pointer
 * to the log handle and specifying the Log Type for every message. Therefore it is suggested that every application 
 * should define its own simplified log Macros, for example:
 * \code
 * #define INFO(_format_, ...) exos_log_info(&logger, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);
 * #define DEBUG(_format_, ...) exos_log_info(&logger, EXOS_LOG_TYPE_USER + EXOS_LOG_TYPE_VERBOSE, _format_, ##__VA_ARGS__);
 *  .. 
 *      
 * int main(void)
 * {
 *      exos_log_init(&log, "watertank");
 *      INFO("Application Started!");
 *      ..
 * 
 * \endcode
 * 
 * \section EXOS_ASSERT_OK
 * 
 * A lot of example code is using the \c EXOS_ASSERT_OK macro.
 * With the exos_log functionality, this macro needs an additional \c EXOS_ASSERT_LOG define, pointing out the reference to the exos_log_handle_t
 * 
 * \code
 * 
 * #define EXOS_ASSERT_LOG &logger  //needs to be defined before including exos_log.h
 * #include "exos_log.h"
 * 
 * exos_log_handle_t logger;
 * 
 * int main(void)
 * {
 *      ...
 *      exos_log_init(&logger, "MyApplication");
 *  
 *      ...
 *      while(true)
 *      {
 *          EXOS_ASSERT_OK(exos_value_init(...)); //using the &logger handle to print out errors / send out error messages
 *          ...
 *          exos_log_process(&log);
 *          ...
 *      }
 * }
 * \endcode
 * 
*/

#ifndef _EXOS_LOG_H_
#define _EXOS_LOG_H_

#include "exos_api.h" // EXOS_ERROR_CODE and exos_get_error_string

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define EXOS_LOG_EXCLUDE_LIST_LEN 20 /**< The current maximum number of modules that can be excluded (filtered at the source) using exos_log_config is 20, let us know why you would need more*/
#define EXOS_LOG_MAX_NAME_LENGTH 35  /**< Object Names (used in exos_log_init) cannot preceed 35 characters as a limitation in the AR logger */
#define EXOS_LOG_MESSAGE_LENGTH 256  /**< Messages cannot preceed 256 characters as a limitation in the AR logger*/

#ifdef _SG4
#include <bur/plctypes.h>
typedef STRING exos_string;
typedef BOOL exos_bool;
#else
typedef char exos_string;
typedef bool exos_bool;
#endif

/**
 * \brief Log Levels
 * 
 * The log level defines the severity of the certain log message. This needs to be considered from two sides:
 * 
 * \arg From the logging perspective, that is - when you want to log something, the level specifies the importance or severity of the message,
 * meaning that for example \c EXOS_LOG_LEVEL_SUCCESS has a higher importance than for example \c EXOS_LOG_LEVEL_INFO. As the lowest severity
 * level, there is the \c EXOS_LOG_LEVEL_DEBUG, which can be used for logging information that only needed in debugging situations.
 * 
 * \arg From the logger, or log-server perspective, the Level should be considered as a filter, that enables the capturing or filtering of
 * messages on different levels. For example, if you set the Level to ERROR, only error messages logged with \c exos_log_error will be visible.
 * On the other hand, if you set it to \c EXOS_LOG_LEVEL_DEBUG, then \c EXOS_LOG_LEVEL_ERROR, \c EXOS_LOG_LEVEL_WARNING, \c EXOS_LOG_LEVEL_SUCCESS, 
 * \c EXOS_LOG_LEVEL_INFO and \c EXOS_LOG_LEVEL_DEBUG messages will be logged. As mentioned in the top, the filtering of the Level takes place on the 
 * invidiual module or application via the configuration broadcasted by the log-server.
 */
typedef enum
{
    EXOS_LOG_LEVEL_ERROR,   /**< As a filter: Log only error. As a module: Log errors for events that has fatal or severe impact on your application */
    EXOS_LOG_LEVEL_WARNING, /**< As a filter: Log error and warning. As a module: Log warnings for events that might have substantial impact on your application */
    EXOS_LOG_LEVEL_SUCCESS, /**< As a filter: Log error, warning and success. As a module: Log success messages that are of significant importance to your machine, i.e. when you want to indicate that the system started up correctly */
    EXOS_LOG_LEVEL_INFO,    /**< As a filter: Log error, warning success and info. As a module: Log info for more elaborate information of your applications events*/
    EXOS_LOG_LEVEL_DEBUG,   /**< As a filter: Log everything. As a module: Log debug messages if you are tracing down specific things in your applications. Combind with \c EXOS_LOG_TYPE_VERBOSE if needed.*/
} EXOS_LOG_LEVEL;

/**
 * \brief Log Types 
 * 
 * Additional to levels, you might want to set a switch that in some cases might flood the logger, both on SUCCESS INFO and DEBUG level.
 * This switch should describe the type of the log entry, and you should be able to select one or more (bit coded), i.e. its not a level.
 * 
 * Additional to the log-level, you might want to set a switch which \b type of log messages that you want to see, independent of being on SUCCESS, INFO or DEBUG level.
 * 
 * \note As for its bit-coded nature, Log Types can be combinded, for example, the following would be the very most internal user debug verbose information
 * \code
 *      ..
 *          exos_log_debug(&logger, EXOS_LOG_TYPE_USER + EXOS_LOG_TYPE_VERBOSE, "this is information that is just needed in extreme cases");
 *      ..
 * \endcode
 */
typedef enum
{
    /** Things you want to log that you would like to pass to the log-server regardless of 
     * the type-filter that is active on the log-server. only filtered with the current level.
     */
    EXOS_LOG_TYPE_ALWAYS = 0,

    /** If you want to be able to filter out application related log messages in future 
     * (and for example only see system logs), this is the type you should use if no other 
     * reason not to. 
     */
    EXOS_LOG_TYPE_USER = 1,

    /** All exos-internal modules log with the \c EXOS_LOG_TYPE_SYSTEM switch, and you shouldnt 
     * use this switch when logging in your application (unless you see a good reason for it) */
    EXOS_LOG_TYPE_SYSTEM = 2,

    /** If you have log messages that might at times flood the logger, you should set the 
     * \c EXOS_TYPE_VERBOSE option to indicate that this data is indeed more than what is 
     * normally interesting to look at.
     * If combinded with the \c EXOS_LOG_TYPE_SYSTEM switch, the internal shared memory logging
     * is activated. This will surely give more extensive information in the logger, but can 
     * be useful when tracing errors in the communication*/
    EXOS_LOG_TYPE_VERBOSE = 4,
} EXOS_LOG_TYPE;

/**
 * \private
*/
typedef struct exos_log_private
{
    uint32_t _magic;
    void *_log;
    void *_reserved[8];
} exos_log_private_t;

typedef struct exos_log_handle exos_log_handle_t;
typedef struct exos_log_config_type exos_log_config_type_t;
typedef struct exos_log_config exos_log_config_t;

/**
 * \brief callback that is triggered when a config changes
 * 
 * This callback is triggered the first time a config is received (meaning the point in time where the module will forward its buffered messages to the log server),
 * and at any other given time when a module in the system calls \c exos_log_change_config.
 * 
 * The callback can conveniently be combinded with exos_log_change_config, in case you want to make relative changes to the log, for example start out from the system log configuration.
 * 
 * \note \b IMPORTANT make sure that exos_log_change is not called automatically within this callback, otherwise you will create an infinite loop of meaningless config changes.
 * Although the system will not break down, changing the config whever changed will probably just be annoying for all other participants in the system.
 * 
 * \code
 *      exos_log_handle_t logger;
 *      bool change_config = false;
 *      exos_log_config_t config = {};
 * 
 *      //make sure this module is NEVER EVER excluded from logging J
 *      void config_changed(exos_log_handle_t *log, const exos_log_config_t *new_config, void *user_context)
 *      {
 *          //takeover the system configuration, or the last change from the system into the local config structure
 *          memcpy(&config, new_config, sizeof(exos_log_config_t));
 *          if(log->excluded)
 *          {
 *              exos_log_stdout("I, %s, was excluded from logging :( - but not if I have a say!", log->name);
 * 
 *              //ENABLE me again!  =)
 *              for(int i=0; i<EXOS_LOG_EXCLUDE_LIST_LEN; i++)
 *              {
 *                  if(0 == strcmp(log->name,config.excluded[i])) //remove me from the exclude list!
 *                  {
 *                      config.excluded[0] = '\0';
 *                      //ha! cannot avoid me!
 *                      exos_log_change_config(log, &config);
 *                  }
 *              }
 *          }
 *      }
 * 
 * 
 *      int main()
 *      {
 *          exos_log_init(&logger, "MyApplication");
 *          exos_log_config_listener(&logger, config_changed, NULL); //we dont have a specific user context here, variables are accesses globally..
 *          ...
 * 
 *          while( .. )
 *          {
 *              exos_log_process(&logger); //this is where the callback is triggered
 *          }
 * 
 * \endcode
 * 
 * \note you can easily find out whether or not this module has been excluded from logging using \c log->excluded. 
 * This information is basically part of the \c config structure given as a string, but for convenience reasons, you can also access it as a boolean variable.
 *  
 * \param log pointer to the \c exos_log_handle that triggered the event
 * \param new_config pointer to the \c exos_log_config structure that contains the new log config
 * \param user_context user defined context pointer that is passed in the exos_log_config_listener, and that can be reestablished in the callback.
 * 
 * \see exos_log_config_listener
 * \see exos_log_change_config
*/
typedef void (*exos_config_change_cb)(const exos_log_handle_t *log, const exos_log_config_t *new_config, void *user_context);

/** 
 * \brief Handle for log within an application
 * 
 * The \c exos_log_handle_t is used as a general reference for logging from an application. Whereas it might be uncommon to
 * have to specify a handle for logging something (normally logs would just go to the stdout), the handle enables the application
 * to send log-messages across the systems to be collected on a central point. Additionally, the keeping of a handle allows the
 * applications to receive log configuration (filter), that log entries in the code get a quick return instead of bloating the 
 * system with log messages that are then filtered on the log-server. It allows for applications with extensive log information
 * to run effectively in normal production, but still be able to log more intensively if the user / operator needs to look for
 * problems in the applications.
 */
struct exos_log_handle
{
    char *name;                             /**< Read-only name of the application, initialized with \c exos_log_init*/
    bool ready;                             /**< Read only. Application has received config and is ready to log */
    bool excluded;                          /**< Read only. Application has received config and has been excluded, which means it will not send any messages to the log-server. */
    bool console;                           /**< Read only. Application outputs all log-messages (that are not filtered) on the console (stdout) */
    exos_config_change_cb config_change_cb; /**< The change callback set with exos_log_config_listener*/
    void *config_change_user_context;       /**< The user context set for the exos_config_change_cb callback. set with exos_log_config_listener*/
    void *_reserved[6];                     /**< \private */
    exos_log_private_t _private;            /**< \private*/
};

/**
 * \brief Initialize the log with an application name
 *   
 * Every application has a 35 character long name that should preferrably be unique within the system to easier identify where the 
 * specific log message originated from. The user can of course choose to have the same name for an AR and Linux application that
 * belong together, as in the AR logger, the \c Facility determines if the log message came from AR(0) or Linux(1).
 * 
 * This function needs to be called before using the log handle with any other function. When exiting the application, or when
 * the application decides its time to stop logging - you should call the \c exos_log_delete to free up all used resources.
 * 
 * Example usage
 * \code
 *      exos_log_handle_t logger;
 * 
 *      int main()
 *      {
 *          exos_log_init(&logger, "MyApplication");
 *          //start using the logger handle from here..
 *          ..
 *      }
 * \endcode
 * 
 * \param log pointer to the \c exos_log_handle that is being initialized
 * \param name string that defines the (unique) name of the application sending out log messages
 * 
 * \see exos_log_handle
 * \see exos_log_delete 
 */
EXOS_ERROR_CODE exos_log_init(exos_log_handle_t *log, const char *name);

/**
 * \brief cyclic recieve function for the application
 * 
 * This function is used to receive the log configuration (filter) that is broadcasted by the log server.
 * As soon as a configuration has been recieved (i.e. the module knows which filter the log-server / system
 * has set), the application can send out its logs to the log-server.
 * 
 * The \c exos_log_process needs to be called cyclically, similar to \c exos_artefact_cyclic
 * 
 * \note Until the application has received the log configuration (filter) from the log server, the logged 
 * messages are stored in an internal buffer (with 100 entries) in the \c exos_log_handle_t. As soon as the
 * application receives the configuration, alle previously buffered log messages are sent out to the log-server,
 * unless a level, type or exclude filter applies.
*/
EXOS_ERROR_CODE exos_log_process(const exos_log_handle_t *log);

/**
 * \brief release all resources for a log handle
 * 
 * Every log handle allocates a number of resources (preoperational send buffer, message queue handles ..) with 
 * the \c exos_log_init function that should be released when the application ends or for whatever reason decides 
 * to stop logging.
 * 
 * This function is used to free up the allocated resources.
*/
EXOS_ERROR_CODE exos_log_delete(exos_log_handle_t *log);

/**
 * \brief Log error message
 * 
 * Error messages are always passed on to the log server, and therefore has no type filter
 * possibility.
 * 
 * Only log errors if you have a problem.
*/
EXOS_ERROR_CODE exos_log_error(const exos_log_handle_t *log, char *format, ...);

/**
 * \brief Log warning messages
 * 
 * Warning messages represent events that might have severe impact on your application, but cannot be direcly
 * regarded as errors, as the application might be able to recover from the situation.
 * 
 * If you want to log warnings that you wish to be visible by default, you should log with the
 * type flag \c EXOS_LOG_TYPE_ALWAYS.
*/
EXOS_ERROR_CODE exos_log_warning(const exos_log_handle_t *log, EXOS_LOG_TYPE type, char *format, ...);

/**
 * \brief Log success messages
 * 
 * Success messages constitute rather important information, as it might be a recovery from
 * an error. Therefore the default log filter level is \c EXOS_LOG_LEVEL_SUCCESS.
 * 
 * If you want to log messages that you wish to be visible by default, you should log with the
 * type flag \c EXOS_LOG_TYPE_ALWAYS.
*/
EXOS_ERROR_CODE exos_log_success(const exos_log_handle_t *log, EXOS_LOG_TYPE type, char *format, ...);
/**
 * \brief Log info messages
 * 
 * Info messages are rather pointing to something beyond what is necessary to know for normal 
 * operation. It makes sense to combind the \c exos_log_info message with the flas \c EXOS_LOG_TYPE_USER
 * in order to filter out these messages by the type.
*/
EXOS_ERROR_CODE exos_log_info(const exos_log_handle_t *log, EXOS_LOG_TYPE type, char *format, ...);

/**
 * \brief Log debug messages
 * 
 * For debug purposes, where detailed information is needed, there is one level beyond \c EXOS_LOG_LEVEL_INFO,
 * that should be used for dispatching debug information. If you combind this function with the flags
 * \c EXOS_LOG_TYPE_USER \c + \c EXOS_LOG_TYPE_VERBOSE, you indicate that the information you are logging
 * may very well flood the logger, which could happen when logging the cyclically changing value changes.
*/
EXOS_ERROR_CODE exos_log_debug(const exos_log_handle_t *log, EXOS_LOG_TYPE type, char *format, ...);

/**
 * \brief Configuration of Type filter
 * 
 * As the type filter is bit coded, the \c exos_config_t structure uses individual booleans instead of the 
 * \c EXOS_LOG_TYPE enum, which would strictly speaking only allow for one type filter. In this way,
 * all or none of the type filters can be set.
*/
struct exos_log_config_type
{
    /**
     * Allow the logging of messages with the \c EXOS_LOG_TYPE_USER bit set
    */
    exos_bool user;
    /**
     * Allow the logging of messages with the \c EXOS_LOG_TYPE_SYSTEM bit set
    */
    exos_bool system;
    /**
     * Allow the logging of messages with the \c EXOS_LOG_TYPE_VERBOSE bit set.
     * If this flag is set together with the level \c EXOS_LOG_LEVEL_DEBUG, this 
     * means that you are logging basically anything coming in, depending if you
     * have set teh user or system flags aswell
    */
    exos_bool verbose;
};

/**
 * \brief Configuration structure broadcasted globally
 * 
 * The \c exos_log_config structure contains the configuration options (filters) that can be set for the
 * logger system during runtime. With the \c exos_log_change_config, this log configuration is broadcasted
 * to all modules/applications in the system (i.e. received at every \c exos_log_handle_t via the 
 * \c exos_log_process function). 
 * 
 * The config structure contains the filtering level that should apply to the rest of the system, for example
 * setting the \c level to \c EXOS_LOG_LEVEL_ERROR will inhibit all modules to log anything else but errors.
 * 
 * \see exos_log_change_config
*/
struct exos_log_config
{

    EXOS_LOG_LEVEL level;
    exos_log_config_type_t type;

    /**
     * Enable applications to log messages on the console (stdout) in parallel to sending the log messages to
     * the log server.
     * 
     * \note Only log messages that pass the set filter will be output on the console.
    */
    exos_bool console;
    /**
     * Here you are able to disable some of the modules to output something to the logger.
     * You set this exclusion by entering the name of the modules you want to prevent sending out log messages, regardless of the current filter.
     * For example, you might want detailed info from exos_msg, but not from the api. In this case you would set config.exclude[n] = "api_MyApplication" if the
     * \c name passed to the \c exos_log_init function was "MyApplication".
    */
    exos_string exclude[EXOS_LOG_EXCLUDE_LIST_LEN][EXOS_LOG_MAX_NAME_LENGTH + 1];
};

/**
 * \brief Change the log configuration (filter) during runtime
 * 
 * This function allows to set the current global filter for all applications in the system.
 * 
 * Many times, it is necessary to switch to more detailed level of logging (debugging) in case
 * it is uncertain what the system does. This function allows to set the various filters 
 * during runtime (when the system is active) in order to see more deeply into what the 
 * application is up to (provided that the application logs on different levels).
 * 
 * Example usage:
 * \code
 *      exos_log_handle_t logger;
 *      bool change_config = false;
 * 
 *      int main()
 *      {
 *          exos_log_init(&logger, "MyApplication");
 * 
 *          ...
 * 
 *          if(change_config)
 *          {
 *              //set the level to INFO, and allow both USER and SYSTEM messages,
 *              // but prevent the exos_api (library) to log anything from this module.
 *              // Print all logged messages on the console
 *              exos_log_config_t config = {};
 *              config.level = EXOS_LOG_LEVEL_INFO;
 *              config.type.user = true;
 *              config.type.system = true;
 *              config.console = true;
 *              strcpy(config.exclude[0],"api_MyApplication");
 *              exos_log_change_config(&logger, &config);
 *              change_config = false;
 *          }
 * 
 * \endcode
 * 
 * \note Changing this configuration will change the logging filter at the source,
 * meaning that if you for example set the level to EXOS_LOG_LEVEL_ERROR, you will inhibit
 * all programs/modules from logging / sending any log messages above the error level.
 * 
 * \param log pointer to the \c exos_log_handle
 * \param config pointer to the \c exos_log_config structure that contains the log config you want to set.
 * \see exos_log_config
*/
EXOS_ERROR_CODE exos_log_change_config(exos_log_handle_t *log, const exos_log_config_t *config);

/**
 * \brief listen to config changes
 * 
 * This function initialized a listener that triggers a callback whenever a new config is received.
 * 
 * As the logging system works in a way that a config is necessary for the logger module to actually log anything,
 * this callback also indicates the first time it is connected to the logging system, i.e. after the log server has
 * received the initialization message sent with \c exos_log_init(). After this first callback, new callbacks
 * are triggered whenever a module in the system broadcasts a new log config via \c exos_log_change_config().
 * 
 * Wheras most information can be found in the log_handle itself, having a callback simplifies the usage, as you
 * dont have to make cyclic checks of the current status.
 * 
 * \param log pointer to the \c exos_log_handle
 * \param config_change_callback the function that is called when a new config has been recieved.
 * \param user_context the context (structure, class etc) that you want to pass on to the callback
 * 
 * \see exos_config_change_cb
 * \see exos_log_change_config
 * 
*/
EXOS_ERROR_CODE exos_log_config_listener(exos_log_handle_t *log, exos_config_change_cb config_change_callback, void *user_context);

/**
 * \brief log output to the console
 * 
 * In Linux, \c exos_log_stdout goes directly to the console output of the individual program
 * In Automation Runtime, the console output is forwarded as an ASCII stream to a configured TCP port (default 47000)
 * 
 * This option lets you output messages for debug or testing purposes without having them collected in the log.
 * 
 * For Automation Runtime, there are also functions exported to integrate the unity testing framework to this console output.
 * See the exos_log_console.h for more on that.
 * 
 */
void exos_log_stdout(const char *format, ...);

#define ANSI_COLOR_RED "\x1b[31m"     /**< \private */
#define ANSI_COLOR_GREEN "\x1b[32m"   /**< \private */
#define ANSI_COLOR_RESET "\x1b[0m"    /**< \private */
#define ANSI_COLOR_YELLOW "\x1b[33m"  /**< \private */
#define ANSI_COLOR_BLUE "\x1b[34m"    /**< \private */
#define ANSI_COLOR_WHITE "\x1b[37m"   /**< \private */
#define ANSI_COLOR_MAGENTA "\x1b[35m" /**< \private */
#define ANSI_COLOR_CYAN "\x1b[36m"    /**< \private */

#ifndef EXOS_ASSERT_CALLBACK
#define EXOS_ASSERT_CALLBACK
#endif

//EXOS_ASSERT_LOG should be defined as something like: &log
#ifdef EXOS_ASSERT_LOG
#define ERR_ASSERT(_format_, ...) exos_log_error(EXOS_ASSERT_LOG, _format_, ##__VA_ARGS__);
#else

//EXOS_ASSERT_LOG undefined, use stdout
#ifdef _SG4
#define ERR_ASSERT(_format_, ...)
#else
#define ERR_ASSERT(_format_, ...) exos_log_stdout(ANSI_COLOR_RED _format_ ANSI_COLOR_RESET "\n", ##__VA_ARGS__);
#endif

#endif //EXOS_ASSERT_LOG

#define EXOS_ASSERT(_error_, _exp_)                                                                                     \
    do                                                                                                                  \
    {                                                                                                                   \
        EXOS_ERROR_CODE err = _exp_;                                                                                    \
        if (_error_ != err)                                                                                             \
        {                                                                                                               \
            ERR_ASSERT("Error in file %s:%d", __FILE__, __LINE__);                                                      \
            ERR_ASSERT("%s returned %d (%s) instead of expected %d", #_exp_, err, exos_get_error_string(err), _error_); \
            EXOS_ASSERT_CALLBACK;                                                                                       \
        }                                                                                                               \
    } while (0)

#define EXOS_ASSERT_OK(_exp_) EXOS_ASSERT(EXOS_ERROR_OK, _exp_)

#endif //_EXOS_LOG_H_
