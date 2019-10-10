/*
 * C compiler.
 * Local variable declarations.
 */
#ifdef   vax
#include "INC$LIB:cc0.h"
#else
#include "cc0.h"
#endif

/*
 * Read local variables.
 * The correct lexic level is in "llex".
 * The first token of the locals is in "s".
 */
locals()
{
	register SYM	*sp;
	SYM		*nsp;
	DIM		*dp, *ndp;
	INFO		*ip;
	int		seg;
	int		rf;
	int		c, dc, t;

	for (;;) {
		gcandt(&c, &t, &dp, &ip, &rf);
		if (c==C_NONE && t==T_NONE && rf==0)
			break;
		if (s == SEMI) {
			lex();
			continue;
		}
		if (c == C_NONE)
			c = C_AUTO;
		if (t == T_NONE)
			t = T_INT;
		for (;;) {
			if (gdecl(&nsp, &ndp, dp, c, SL_VAR)) {
				sp = nsp;
				sp = declare(sp, c, t, ndp, ip, rf);
				if ((dc=sp->s_class) == C_SIN) {
					seg = SPURE;
					if ((rf&S_RONLY) == 0) {
						if (s == ASSIGN)
							seg = SDATA;
						else
							seg = SBSS;
					}
					newseg(seg);
					align(sp);
					sp->s_value = here();
					sp->s_seg = seg;
					if (s == ASSIGN) {
						lex();
						putautos();
						init(sp, sp->s_dp, (sizeof_t)0, 1);
					} else {
						bput(BLOCK);
						zput(ssize(sp));
					}
					newseg(SCODE);
				}
				if (dc==C_AUTO || dc==C_REG) {
					bindlocal(sp);
					if (s == ASSIGN) {
						lex();
						putautos();
						init(sp, sp->s_dp, (sizeof_t)0, 0);
					}
				}
#ifdef KLUDGE
				if (dc == C_GREF) {
					bput(SGUESS);
					nput(sp->s_id);
					if (isfunc(sp))
						bput(SCODE);
					else
						bput(SDATA);
				}
#endif
			}
			dblocal(sp);
			if (s != COMMA)
				break;
			lex();
		}
		xdropinfo(t, ip);
		mustbe(SEMI);
	}
}
