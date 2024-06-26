/*
 * A debugger.
 * Intel 8086.
 */
#include <stdio.h>
#include <sys/const.h>
#include "trace.h"
#include "i8086.h"

/*
 * Disassemble an Intel 8086 instruction.
 */
char *
coninst(sp, s)
register char *sp;
{
	register INS *ip;
	register char *cp;
	register int r;
	register long a;
	unsigned char b[1];

	r = -1;
	for (;;) {
		if (getb(s, b, sizeof(b)) == 0)
			return (NULL);
		if ((b[0]&0xE7) == 0x26) {	/* Segment override */
			r = (b[0]>>3)&0x3;
			continue;
		}
		a = add;
		for (ip=insrtab; ip->i_name!=NULL; ip++) {
			if ((b[0]&ip->i_mask) != ip->i_code)
				continue;
			add = a;
			if ((cp=putifmt(sp, s, ip->i_name, b[0], r, 1))!=NULL)
				return (cp);
		}
		break;
	}
	sprintf(sp, ".byte\t0x%02x", b[0]);
	return (&sp[8]);
}

/*
 * Print out the operands to a Intel 8086 instruction according to a
 * format string.
 */
char *
putifmt(sp, s, fp, ins0, segm, t)
register char *sp;
register char *fp;
unsigned ins0;
{
	register char *cp;
	register int c;
	register int e;
	register int m;
	register int n;
	unsigned insn;
	int insk;
	char x[1];
	unsigned char b[1];
	register INS *ip;

	e = 0;
	m = 0;
	n = 0;
	insn = ins0;
	while ((c=*fp++) != '\0') {
		if (c != '%') {
			*sp++ = c;
			continue;
		}
	next:
		switch (c=*fp++) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
			n = c - '0';
			m = 1 << n;
			goto next;
		case ':':
			if (segm >= 0) {
				*sp++ = "ecsd"[segm];
				*sp++ = 's';
				*sp++ = ':';
			}
			goto next;
/*
		case 'a':
			v = (i>=-0x100 && i<=0x100) ? -1 : I;
			sp = conaddr(sp, 0, (long)(unsigned)i, v);
*/
		case 'b':
			if (getb(s, x, sizeof(x)) == 0)
				return (NULL);
			insk = x[0];
			goto next;
		case 'd':
			cp = (insn&m) ? "%n3r,%m" : "%nm,%3r";
			sp = putifmt(sp, s, cp, ins0, segm, t);
			continue;
		case 'e':
			e = insn&m;
			goto next;
		case 'f':
			if (((insn>>3)&0x7) != *fp++-'0')
				return (NULL);
			goto next;
		case 'g':
			if (insn != 10)
				sprintf(sp, "\t$0x%04x", insn);
			continue;
		case 'h':		/* 8087 escape */
			insk = ((ins0&0x7)<<3) | ((insn>>3)&0x7);
			sprintf(sp, "%x", insk);
			goto skip;
		case 'i':
			if (e==0 && t!=0) {
				if (getb(s, &insk, sizeof(insk)) == 0)
					return (NULL);
			} else {
				if (getb(s, b, sizeof(b)) == 0)
					return (NULL);
				if (e == 0)
					insk = b[0];
				else
					insk = x[0] = b[0];
			}
			goto next;
		case 'j':
			cp = jumptab[ins0&0xF];
			goto copy;
		case 'k':
			sp = conaddr(sp, 2, (long)(unsigned)insk,
				(unsigned)insk>0x100 ? 16 : -1);
			goto skip;
		case 'l':
			sprintf(sp, "0x%04x", insk);
			goto skip;
		case 'm':
			if ((insk=(insn>>6)&0x03) == 3) {
				n = 0;
				goto regn;
			}
			if (segm >= 0) {
				*sp++ = "ecsd"[segm];
				*sp++ = 's';
				*sp++ = ':';
			}
			switch (insk) {
			case 0:
				if ((insn&0x7) == 0x6) {
					sp = putifmt(sp, s, "%wp", ins0,
						segm, t);
					 continue;
				}
				insk = 0;
				break;
			case 1:
				if (getb(s, x, sizeof(x)) == 0)
					return (NULL);
				insk = x[0];
				break;
			case 2:
				if (getb(s, &insk, sizeof(insk)) == 0)
					return (NULL);
				break;
			case 3:
				n = 0;
				goto regn;
			}
			if (insk != 0) {
				if (insk>=-0x100 && insk<=0x100) {
					sprintf(sp, "%d", insk);
					while (*sp)
						sp++;
				} else {
					sp = conaddr(sp, 0,
						(long)(unsigned)insk, I);
				}
			}
			cp = indmtab[insn&0x7];
			goto copy;
		case 'n':
			if (getb(s, b, sizeof(b)) == 0)
				return (NULL);
			insn = b[0];
			goto next;
		case 'o':
			insk += add;
			goto next;
		case 'p':
			sp = conaddr(sp, segm==1?1:0, (long)(unsigned)insk, I);
			continue;
		case 'q':
			sp = conaddr(sp, 1, (long)(unsigned)insk, I);
			continue;
		case 'r':
		regn:
			cp = (t?regwtab:regbtab)[(insn>>n)&07];
			*sp++ = *cp++;
			*sp++ = *cp++;
			continue;
		case 's':
			*sp++ = "ecsd"[(insn>>n)&03];
			*sp++ = 's';
			continue;
		case 't':
			if ((t=insn&m) == 0)
				*sp++ = 'b';
			continue;
		case 'u':
			if (getb(s, b, sizeof(b)) == 0)
				return (NULL);
			insk = b[0];
			goto next;
		case 'v':
			if ((ins0&m) == 0) {
				*sp++ = '$';
				*sp++ = '1';
			} else {
				*sp++ = 'c';
				*sp++ = 'l';
			}
			continue;
		case 'w':
			if (getb(s, &insk, sizeof(insk)) == 0)
				return (NULL);
			goto next;
		case 'x':
			*sp++ = 'a';
			*sp++ = t ? 'x' : 'l';
			continue;
		case 'y':
			if ((insk&0x80) != 0
			&& (n=insk&0x7F) < NMICALL
			&& (cp=sysitab[n]) != NULL)
				sprintf(sp, "sys\t%s", cp);
			else
				sprintf(sp, "int\t0x%x", insk);
			goto skip;
		case 'z':
			if ((insn&m) != 0)
				return (NULL);
			goto next;

		case 'F':		/* 8087 codes */
			if (getb(s, b, sizeof(b)) == 0)
				return (NULL);
			insk = ((insn&0x7)<<8) | b[0];
			insn = b[0];
			for (ip = fpinstab; ip->i_name!=NULL; ip++) {
				if ((insk&ip->i_mask) != ip->i_code)
					continue;
				sp = putifmt(sp, s, ip->i_name, b[0], segm, t);
				goto skip;
			}
			return (NULL);

		case 'P':		/* Long pointers */
			if (getb(s, &insn, sizeof (insn)) == 0)
				return (NULL);
			sprintf(sp, "0x%04x:0x%04x", insn, insk);
			goto skip;
		}
	copy:
		while (*cp)
			*sp++ = *cp++;
		continue;
	skip:
		while (*sp)
			sp++;
		continue;
	}
	return (sp);
}

/*
 * Given a size character, `t1', and a type, `t2', return the appropriate
 * format string.
 */
char *
getform(t1, t2)
register int t1;
register int t2;
{
	register char *cp;
	register char *sp;

	if (t1=='f' || t1=='F')
		return ("%g");
	if (t1 == 'h')
		t1 = 'w';
	if ((cp=index(sp="bwlv", t1)) == NULL)
		return ("?");
	t1 = cp-sp;
	if ((cp=index(sp="duox", t2)) == NULL)
		return ("?");
	t2 = cp-sp;
	return (formtab[t1][t2]);
}
