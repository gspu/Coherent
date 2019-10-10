/*
 * Some programs request this header for accessing the mem... () functions
 * from the Standard C library. This behaviour is derived from BSD Unix
 * systems, and is not portable; we just redirect the request to <string.h>
 * so that such code can still compile.
 */

#include <string.h>
