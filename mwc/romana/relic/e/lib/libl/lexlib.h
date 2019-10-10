#include <stdio.h>
#include <sys/mdata.h>

#define	YYTSIZE		4096
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
	short		ll_qs[QSIZE+1];
} 			ll_tq;
extern	char		yytext[YYTSIZE];
extern	short		yy_lextab[];
extern	short		yy_lxctab[];
extern	short		yy_lxbtab[];
extern	short		yy_ctxtab[];
extern	short		*yy_clist[];
extern	short		*yy_nlist[];
extern	short		*yy_llist[];
extern	short		*ll_ctxt;
extern	short		ll_cc;
extern	short		ll_lc;
extern	short		ll_tf;
extern	short		ll_lf;
extern	short		ll_tokn;
extern	short		ll_tlen;
extern	short		ll_more;
extern	short		yyscon;
extern	short		yyleng;
extern	char		inpovf[];
extern	char		yytovf[];
extern	char		illchr[];
extern	short		_llic();
extern	short		_lloc();
