#ifndef lint
static char *sccsid = "@(#)nptx.c	2.5 (smail) 9/15/87";
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include "defs.h"
#include <ctype.h>

char *malloc(), *index(), *fullname();
void nptx(), free(), dotspace();

enum edebug debug;	/* not used by nptx */
char *fnlist = NULL;	/* not used by nptx */

main()
{
	int i;
	char buf[SMLBUF], *p, *name, *last, *nick, *ctmp;

	while(gets(buf) != NULL) {
		/*  line should be in form
		**
		**  login	First Last
		** or
		**  login	First Last(Nickname)
		**
		*/
		if((p = index(buf, '\t')) == NULL) {
			(void) fprintf(stderr, "format error: %s\n", buf);
			continue;
		}

		*p++ = '\0';
		name = fullname(p);
		dotspace(name);

		if (last = rindex(name, '.')) {
			last++;
		} else {
			last = NULL;
		}

		if ((nick = index(p, '(')) != NULL) {
			nick++;
			if ((ctmp = index(nick, ')')) == NULL) {
				nick = NULL;
			} else {
				*ctmp = '\0';
			}
		}

		nptx(buf, name);

		if((last != NULL) && (nick != NULL)) {
			i=strlen(nick) + strlen(last) + 2;
			if((name = malloc(i)) != NULL) {
				(void) strcpy(name, nick);
				(void) strcat(name, ".");
				(void) strcat(name, last);
				dotspace(name);
				nptx(buf, name);
				free(name);
			}
		}
	}
	return(0);
}

void
dotspace(s)
char *s;
{
	register char *p, *t;

	/* turn whitespace to '.' */
	for(p = s; *p != '\0'; p++) {
		if((*p == ' ') || (*p == '\t')) {
			*p = '.';
		}
	}

	/* elide leading '.'s */
	for(p = s; *p == '.' ; p++)
		;

	/* elide mulitple '.'s and all "'"s */
	for(t = s; *p != '\0'; p++, t++) {
		*t = *p;

		if(*t == '\'') {
			t--;
			continue;
		}

		if(*p == '.') {
			while(*(++p) == '.')
				;
			p--;
		}
	}
	*t = '\0';

	/* elide trailing '.' */
	if((t > s) && (*(--t) == '.')) *t = '\0';
}

void
nptx(login, name)
char *login, *name;
{
	int i,j,k,N,lim,mask;
	int ii,ji,ki,Ni,limi,maski;
	char nl[11][100], il[11][100];
	char *pi, *p, *rindex();
	char buf[100];
	char bufi[100];

	if((name == NULL) || (*name == '\0')) {
		return;
	}

	for(i=0; i < 10; i++) {
		if((p = rindex(name, '.')) == NULL) break;
		(void) strcpy(nl[i], p+1);
		*p = NULL;
	}
	(void) strcpy(nl[i], name);

	while((strcmpic(nl[i], "Mr"  ) == 0)
	 ||   (strcmpic(nl[i], "Dr"  ) == 0)
	 ||   (strcmpic(nl[i], "Mrs" ) == 0)
	 ||   (strcmpic(nl[i], "Miss") == 0)
	 ||   (strcmpic(nl[i], "Ms"  ) == 0)) {
		i--;
	}

	while((strcmpic(nl[0], "Jr") == 0)
	 ||   (strcmpic(nl[0], "Sr") == 0)) {
		for(j=0; j < i; j++) {
			(void) strcpy(nl[j], nl[j+1]);
		}
		i--;
	}

	N = i;
	lim = 1 << (N+1);
	for(mask = 1 << N ; mask < lim ; mask++) {
		buf[0] = '\0';
		for(j = 1, k = N; j < lim; j <<=1, k--) {
			if(j & mask) {
				(void) strcat(buf, nl[k]);
				(void) strcat(buf, ".");
			}
		}
		if((p = rindex(buf, '.')) != NULL) {
			*p = '\0';
		}

		for(ii=0; ii < 10; ii++) {
			if((pi = rindex(buf, '.')) == NULL) break;
			(void) strcpy(il[ii], pi+1);
			*pi = NULL;
		}
		(void) strcpy(il[ii], buf);
		Ni = ii;
		limi = 1 << (Ni+1);
		for(maski = 1 << Ni /* 0 */ ; maski < limi ; maski++) {
			bufi[0] = '\0';
			for(ji = 1, ki = Ni; ji < limi; ji <<=1, ki--) {
				if(ji & maski) {
					(void) strcat(bufi, il[ki]);
				} else {
					char init[3];
					init[0] = il[ki][0];
					init[1] = '\0';
					(void) strcat(bufi, init);
				}
				(void) strcat(bufi, ".");
			}
			if((pi = rindex(bufi, '.')) != NULL) {
				*pi = '\0';
			}
#ifdef DOT_REQD
			if(index(bufi, '.') == NULL) {
				continue;
			}
#endif /* DOT_REQD */
			(void) printf("%s\t%s\n",bufi, login); /* */
		}
	}
}
