/*
 * ptypes.h -- portable types.  Change the typedefs in this file
 * to match the local architecture.
 */
#ifndef PTYPES_H	/* Rest of file.  */
#define PTYPES_H
/* This file ought to be rewritten to adjust itself based on the contents
 * of the ANSI file limits.h.
 */

typedef signed char int8;
#define MAXINT8		((int8) 127)
typedef unsigned char uint8;
#define MAXUINT8	((uint8) 255)
typedef short int16;
#define MAXINT16	((int16) 32767)
typedef unsigned short uint16;
#define MAXUINT16	((uint16) 65535L)
typedef long int32;
#define MAXINT32	((int32) 2^31 - 1)
typedef unsigned long uint32;
#define MAXUINT32	((uint32) (((uint32) 2^32) - 1))

#endif /* ifdef PTYPES_H */
