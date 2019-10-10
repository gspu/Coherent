#include "mprec.h"

/*
 *	Itom returns a pointer to a mint which has a value equal to
 *	the int "n".  It allocates the space both for the mint and
 *	for the value.  Note that itom can be used to initialize
 *	pointer to mint's.
 */

mint *
itom(n)
int n;
{
	register mint	*result;

	result = (mint *)mpalc(sizeof *result);
	minit(result);
	mitom(n, result);
	return (result);
}
