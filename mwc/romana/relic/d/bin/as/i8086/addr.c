#include "asm.h"

/*
 * Address mode table.
 * This table is indexed by the number
 * that gets put in the r/m field,
 * and holds a word containing a bit for
 * each index register.
 */

int	amodes[] = {
	(1<<reg(BX)) + (1<<reg(SI)),
	(1<<reg(BX)) + (1<<reg(DI)),
	(1<<reg(BP)) + (1<<reg(SI)),
	(1<<reg(BP)) + (1<<reg(DI)),
	(1<<reg(SI)),
	(1<<reg(DI)),
	(1<<reg(BP)),
	(1<<reg(BX))
};

char	segovr[] = {
	0x26,
	0x2E,
	0x36,
	0x3E
};

/*
 * Read an address.
 * Return the addressing mode.
 */
addr(esp)
register struct expr *esp;
{
	register c;

	if ((c = getnb()) == '$') {
		expr(esp, 0);
		esp->e_mode = IM;
		return (IM);
	}
	if (c == '(') {
		addrx(esp);
		esp->e_type = E_ACON;
		esp->e_addr = 0;
		return (esp->e_mode & MMASK);
	}
	unget(c);
	expr(esp, 0);
	if (esp->e_type == E_AREG) {
		esp->e_mode = esp->e_addr;
		if ((esp->e_mode & MMASK) == SEGR)
			if ((c = getnb()) == ':') {	/* segment override */
				outab(segovr[reg(esp->e_addr)]);
				addr(esp);
			} else
				unget(c);
		return (esp->e_mode & MMASK);
	}
	if ((c = getnb()) == '(') {
		addrx(esp);
		if (esp->e_mode==IDX || esp->e_mode==ICL)
			aerr("invalid index register");
		return (esp->e_mode & MMASK);
	}
	unget(c);
	esp->e_mode = DIR;
	return (DIR);
}

/*
 * Read in index register field
 * of an address. The leading `(' has already
 * been read. Set the mode into the `e_mode'
 * field of the expression structure.
 */
addrx(esp)
struct expr *esp;
{
	register struct sym *sp;
	register c, i;
	int mask, mode;
	char id[NCPLN];

	mode = 0;
	do {
		getid(id, -1);
		if ((sp=lookup(id, 0))==NULL || sp->s_type!=E_AREG)
			aerr("index by non-register");
		else {
			mask = 01 << reg(sp->s_addr);
			if ((mode&mask) != 0)
				aerr("invalid index register");
			mode |= mask;
		}
	} while ((c = getnb()) == ',');
	if (c != ')')
		qerr("missing ')'");
	if (mode == (01<<reg(CL))) {
		esp->e_mode = ICL;
		return;
	}
	if (mode == (01<<reg(DX))) {
		esp->e_mode = IDX;
		return;
	}
	for (i=0; i<8; ++i) {
		if (mode == amodes[i]) {
			esp->e_mode = X | i;
			return;
		}
	}
	aerr("invalid index");
	esp->e_mode = D;
}
