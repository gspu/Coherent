#ifndef	__COMMON__STREAM_H__
#define	__COMMON__STREAM_H__

/*
 * This internal header file is intended as the sole point of declaration for
 * the STREAMS data types exported to drivers via the <sys/stream.h> header.
 * Several high-profile kernel data structures need to declare pointers to
 * these items, and so we export the following incomplete type declarations.
 */

typedef struct msgb	mblk_t;
typedef struct datab	dblk_t;
typedef struct qband	qband_t;
typedef struct queue	queue_t;


/*
 * The following type does not belong to the DDI/DKI STREAMS definition; it is
 * used internally.
 */

typedef struct __streams_schedule ssched_t;

#endif	/* ! defined (__COMMON__STREAM_H__) */
