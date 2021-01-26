#ifndef _EXOS_LOG_CONSOLE_H_
#define _EXOS_LOG_CONSOLE_H_

#include <bur/plctypes.h>
/**
 * Console output functions for AR where printf and posix stdout functions are missing
 * It uses a TCP server on a specified to send out ASCII data (internally buffered)
*/
#define EXOS_LOG_CONSOLE_LENGTH 1024
#define EXOS_LOG_CONSOLE_BUFFER 1024 //number of messages stored in the buffer between the cyclic calls which pushes out messages

/*single (consequtive) line functionality for unity
  note that the current line position pointer is stored internally and therefore 
  it does not support multiple threads accessing these functions */

//reset: resets the current line and character position
void exos_log_console_reset();
//flush: sends out the line to the console buffer and resets the line
void exos_log_console_flush();
//char writes a character to the current line position, and increases the line position by 1
void exos_log_console_char(int a);
//eol adds a \n at the end of the current line, flushes the line to the console buffer and resets the line
void exos_log_console_eol();

/*functions available across more programs*/

//prints out an ascii string to the console buffer (without \n)
//the exos_log_stdout for example forwards its output to this function
void exos_log_console_line(char *line);

//prints out a line to the console buffer (including \n at the end)
//same functionality as exos_log_stdout
void exos_log_console_printf(const char *format, ...);

//internal console server functions. dont use
void exos_log_console_init();
void exos_log_console_cyclic(unsigned short port);
void exos_log_console_exit();

#endif