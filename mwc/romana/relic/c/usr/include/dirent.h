/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * POSIX (IEEE 1003.1) compatible dirent.h header file.
 */

#ifndef DIRENT_H
#define DIRENT_H 1

/*
 * Rely on COHERENT's sys/dir.h to define type DIR and structure direct with
 * fields d_ino of type ino_t and d_name of type char [].
 */
#include <sys/dir.h>

/*
 * Implement dirent as a macro, making struct dirent equivalent to COHERENT's 
 * struct direct.
 */
#define dirent direct

#endif
