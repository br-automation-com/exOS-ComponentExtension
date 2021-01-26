#ifndef _EXOS_MEM_H_
#define _EXOS_MEM_H_

#ifndef _SIZE_T_DEFINED
#include <sys/types.h>
#endif

/**
 * \page exos_mem
 * 
 * The exos_mem library contains thread safe TLSF allocator functions with an automatically
 * growing heap. The logic of function invocation is the same as for common allocator
 * functions; malloc, calloc, free and realloc.
 * 
 * When using C/C++, it may be useful to redirect the standard alloc/free functions (that
 * are also invoked by new/delete) to the exos_mem_alloc/free functions so that you get the
 * deterministic allocation behaviour that TLSF provides. An exception to the deterministic
 * behaviour is when the heap needs to grow. Here the TMP_alloc is used for fetching a new
 * memory area. The size of this area i precalculated by an internal algorithm.
 * 
 * To redirect the standard alloc/free functions to exos_mem_alloc/free functions in a
 * program/library you should link the \c ExHeapRedirect.c from the ExBase library into
 * your task or dynamic library (a static library has no heap of its own).
 * 
 * You do this via selecting Add New Object - Referenced File, then look for the 
 * \c ExHeapRedirect.c in the `\Logical\Libraries\ExBase\SG4` folder. The benefit of
 * using a referenced file is that you can keep up with the latest version of the
 * redirection sourcefile from ExBase in case it changes in future versions.
 * 
 * In case you add the \c ExHeapRedirect.c to your task or dynamic library, you dont
 * need to define a \c bur_heap_size, as the standard heap is not used anymore.
 * For C applications, you merely need to attach an empty dummy.cpp file to activate 
 * the enhanced compiler set that lets you use sprintf and similar functions.
 * 
 * This is how the file looks like:
 * 
 * \include exos_mem/ar/ExHeapRedirect.c
 * 
*/

void *exos_mem_alloc(size_t size);
void exos_mem_free(void *ptr);
void *exos_mem_realloc(void *ptr, size_t size);
void *exos_mem_calloc(size_t nelem, size_t elem_size);
size_t exos_mem_get_used_size(void);
size_t exos_mem_get_max_size(void);

#endif //_EXOS_MEM_H_
