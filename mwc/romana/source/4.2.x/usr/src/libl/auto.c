/* (-lgl
 * 	COHERENT Version 3.1
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

#include "lexlib.h"
#include "lextype.h"

#include <common/_limits.h>
#include <common/tricks.h>

char	yytext [YYTSIZE];
struct	ll_queue ll_tq;
int * ll_ctxt = yy_lextab;
int	ll_cc;
int	ll_lc = '\n';
int	ll_tf;
int	ll_lf;
int	ll_tokn;
int	ll_tlen;
int	ll_more;
int	yyscon;
int	yyleng;
int	yyline;

char	inpovf [] = "input buffer overflow";
char	yytovf [] = "yytext overflow";

_lltk ()
{
	ll_tlen = 0;
	ll_tokn = INT_MAX;
	{
		register int ** clist, ** nlist, ** llist;
		register int ** tlist;
		register c;

		clist = yy_clist;
		nlist = yy_nlist;
		llist = yy_llist;
	loop:
		ll_cc = 0;
		* clist ++ = ll_ctxt;
		* clist-- = NULL;
		do {
			_llex (ll_cc, clist, nlist, llist);
			tlist = nlist;
			nlist = clist;
			clist = tlist;
			* nlist = NULL;
			if (* llist != NULL) {
				c = ll_cc;
				do {
					_llex (c, llist, nlist, llist);
					tlist = nlist;
					nlist = llist;
					llist = tlist;
					* nlist = NULL;
					++ c;
				} while (* llist != NULL);
			}
			++ ll_cc;
		} while (* clist != NULL);
		ll_tf = 0;
		if (ll_tokn == INT_MAX) {
			if ((c = (qct (ll_tq) ? qgt (ll_tq) : _llic ())) ==
			    EOF) {
				ll_lc = '\n';
				return 0;
			}
			ll_lc = c;
			_lloc (c);
			goto loop;
		}
	}
	{
		register char * pc;
		register i;

		pc = yytext;
		if (ll_more) {
			pc += yyleng;
			ll_more = 0;
		} else {
			yyleng = 0;
		}
		i = ll_tlen;
		if ((yyleng += i) > sizeof (yytext))
				error (yytovf);
		while (i --)
			* pc ++ = qgt (ll_tq);
		* pc = 0;
		if (pc-- > yytext)
			ll_lc = * pc;
	}
	return ll_tokn;
}

_llex (n, clist, nlist, llist)
int ** clist, ** nlist, ** llist;
{
	register int ** listp, * statep;
	register int c;
	register int ** clistp;

	c = UCHAR_MAX + 1;
	clistp = clist;
	while ((statep = * clistp ++) != NULL) {
		switch (* statep & LR_MASK) {
_list:
			while (* listp != statep)
				if (* listp ++ == NULL) {
					* listp-- = NULL;
					* listp = statep;
					break;
				}
			break;
_look:
			-- clistp;
			if (qct (ll_tq) <= n)
				if (n >= QSIZE)
					error (inpovf);
				else
					qpt (ll_tq, c = _llic ());
			else
				c = qlk (ll_tq, n);
			break;

		case LX_STOP:
			break;

		case LX_LINK:
			++ statep;
			listp = clist;
			while (* listp != statep)
				if (* listp ++ == NULL) {
					* listp -- = NULL;
					* listp = statep;
					break;
				}
			-- statep;
			/* FALL THROUGH */

		case LX_JUMP:
			statep += * statep >> LR_SHFT;
			listp = clist;
			goto _list;

		case LX_LOOK:
			++ statep;
			listp = llist;
			goto _list;

		case LX_ACPT:
			if ((* statep >> LR_SHFT)< ll_tokn || ll_cc > ll_tlen)
				if (ll_tf == 0 || ll_cc < ll_lf ||
				    ll_tf > * statep) {
					ll_tokn = (* statep >> LR_SHFT);
					ll_tlen = ll_cc;
				}
			break;

		case LX_CHAR:
			/*
			 * This is intended to be a relative comparison that
			 * will *not* match EOF. This fact was an undocumented
			 * consequence of the <sys/mdata.h> brain damage.
			 */
			if (c > (int) UCHAR_MAX)
				goto _look;
			if (c == (* statep ++ >> LR_SHFT)) {
				listp = nlist;
				goto _list;
			}
			break;

		case LX_CLAS:
			/*
			 * This is intended to be a relative comparison that
			 * will *not* match EOF. This fact was an undocumented
			 * consequence of the <sys/mdata.h> brain damage.
			 */

			if (c > (int) UCHAR_MAX)
				goto _look;
			/*
			 * This test, OTOH, *would* match an EOF.
			 */

			if (c & ~ UCHAR_MAX)
				break;

			if (yy_lxctab [(* statep >> LR_SHFT) *
				       ((UCHAR_MAX + 1) / __INT_BIT) +
				       (c / __INT_BIT)] &
			    yy_lxbtab [c & ((1 << MOST_BIT_UCHAR_CONST (__INT_BIT))
				       - 1)]) {
				++ statep;
				listp = nlist;
				goto _list;
			}
			break;

		case LX_BLIN:
			if (ll_lc == '\n') {
				++ statep;
				listp = clist;
				goto _list;
			}
			break;

		case LX_ELIN:
			/*
			 * This is intended to be a relative comparison that
			 * will *not* match EOF. This fact was an undocumented
			 * consequence of the <sys/mdata.h> brain damage.
			 */

			if (c > (int) UCHAR_MAX)
				goto _look;

			if (c == '\n' || c == EOF) {
				++ statep;
				listp = clist;
				goto _list;
			}
			break;

		case LX_ANYC:
			/*
			 * This is intended to be a relative comparison that
			 * will *not* match EOF. This fact was an undocumented
			 * consequence of the <sys/mdata.h> brain damage.
			 */

			if (c > (int) UCHAR_MAX)
				goto _look;

			if (c != '\n' && c != EOF) {
				++ statep;
				listp = nlist;
				goto _list;
			}
			break;

		case LX_SCON:
			if (yyscon == (* statep ++ >> LR_SHFT)) {
				listp = clist;
				goto _list;
			}
			break;
		}
	}
}
