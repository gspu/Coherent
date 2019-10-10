/*
 * Clear the background of a screen.
 */
#include <scn.h>

void
clearBak(back, f) register backGrnd *back; register loc *f;
{
	for (; NULL != back->data; back++)
		clearArea(back->row, back->col, strlen(back->data));
	for (; NULL != f->field; f++)
		clearArea(f->row, f->col, (f->len ? f->len : f->skipf));
}
