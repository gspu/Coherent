#ifndef	EHAND_H
#define	EHAND_H

/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		CONST
 *		EXTERN_C_BEGIN
 *		EXTERN_C_END
 *		PROTO ()
 *	<setjmp.h>
 *		jmp_buf
 *		setjmp ()
 */

#include <sys/compat.h>
#include <setjmp.h>

/*
 * Structure used for error control.
 */

typedef struct ehandler	ehand_t;

struct ehandler {
	ehand_t	      *	eh_next;
	jmp_buf		eh_buf;
};

extern ehand_t	      *	estack_base;

#define	PUSH_HANDLER(eh)	setjmp (((eh).eh_next = estack_base,\
					 estack_base = & (eh), (eh).eh_buf))
#define	POP_HANDLER(eh)		(estack_base != & (eh) ? \
				 error_error (__FILE__, __LINE__) : \
				 (void) (estack_base = (eh).eh_next))
#define	CHAIN_ERROR(eh)		(chain_error (& (eh)))


EXTERN_C_BEGIN

NO_RETURN void	throw_error	PROTO ((CONST char * _fmt, ...));
void		report_error	PROTO ((CONST char * _fmt, ...));
void		error_error	PROTO ((CONST char * _file, int _line));
void		chain_error	PROTO ((ehand_t * _ehand));

EXTERN_C_END


#endif	/* ! define (EHAND_H) */
