/********************************************************************
 * COPYRIGHT -- B&R Automation
 * V1.0 
 ********************************************************************/

#include <bur/plctypes.h>
#include <stdlib.h>
#include <SYS_Lib.h>
#include <string.h>
#include "exos_mem.h"

#ifdef __cplusplus
#error "C (only) compiler recommended"
#endif

/* New versions of memory allocation functions. */
/* ******************************************** */
void *malloc(size_t size) { return exos_mem_alloc(size); }

void free(void *ptr) { exos_mem_free(ptr); }

void *realloc(void *ptr, size_t size) { return exos_mem_realloc(ptr, size); }

void *calloc(size_t items, size_t size) { return exos_mem_calloc(items, size); }

/* Recursive versions. Mainly used by builtin functions. */
/* ***************************************************** */
struct _reent;
#define REENT struct _reent *_r __attribute__((__unused__))

void *_malloc_r(REENT, size_t size)
{
	return exos_mem_alloc(size);
}

void _free_r(REENT, void *ptr) { exos_mem_free(ptr); }

void *_realloc_r(REENT, void *ptr, size_t size) { return exos_mem_realloc(ptr, size); }

void *_calloc_r(REENT, size_t items, size_t size) { return exos_mem_calloc(items, size); }

/* Make sure one of the new symbols always is used even if it's
   not used directly in the code. This is required to prevent
   the linker from skipping our new versions of alloc/free. */
int const forceHeapMemory = (int)malloc;

/* Default version of the old heap size specifier. Required to prevent
   a linker error. The attribues are used to allow a second version to
   be specified without causing a linker error. */
__attribute__((weak, visibility("hidden"))) unsigned long bur_heap_size = 0x0;

/* Redirect assertion. */
void __assert(const char *file, int line, const char *failedexpr)
{
	ERRxwarning(9109, line, (STRING *)file);
	ERRxfatal(9109, 0, (STRING *)failedexpr);
}

void __assert_func(const char *file, int line,
				   const char *func, const char *failedexpr)
{
	ERRxwarning(9109, line, (STRING *)file);
	ERRxwarning(9109, 0, (STRING *)func);
	ERRxfatal(9109, 0, (STRING *)failedexpr);
}

/* Redirect terminate handler. */
void __verbose_terminate_handler()
{
	ERRxfatal(9130, 0, "Terminate Handler Called");
}
