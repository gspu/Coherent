#include <stdio.h>
#include <sys/mdata.h>

#define	YYTSIZE		255
#define	QSIZE		MAXUCHAR
#define	qpt(q,c)	((++q.ll_qc),(q.ll_qs[q.ll_qi++&QSIZE]=(c)))
#define	qug(q,c)	((++q.ll_qc),(q.ll_qs[--q.ll_qo&QSIZE]=(c)))
#define	qgt(q)		((--q.ll_qc),(q.ll_qs[q.ll_qo++&QSIZE]))
#define	qlk(q,n)	(q.ll_qs[q.ll_qo+n&QSIZE])
#define	qct(q)		(q.ll_qc)

extern	struct ll_queue	{
	unsigned char	ll_qc;
	unsigned char	ll_qi;
	unsigned char	ll_qo;
	int		ll_qs[QSIZE+1];
} 			ll_tq;
extern	char		yytext[YYTSIZE];
extern	int		yy_lextab[];
extern	int		yy_lxctab[];
extern	int		yy_lxbtab[];
extern	int		yy_ctxtab[];
extern	int		*yy_clist[];
extern	int		*yy_nlist[];
extern	int		*yy_llist[];
extern	int		*ll_ctxt;
extern	int		ll_cc;
extern	int		ll_lc;
extern	int		ll_tf;
extern	int		ll_lf;
extern	int		ll_tokn;
extern	int		ll_tlen;
extern	int		ll_more;
extern	int		yyscon;
extern	int		yyleng;
extern	char		inpovf[];
extern	char		yytovf[];
extern	char		illchr[];
extern	int		_llic();
extern	int		_lloc();
