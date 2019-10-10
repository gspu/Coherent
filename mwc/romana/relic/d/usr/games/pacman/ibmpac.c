/*
 * pacman
 * dal 1982
 * steve 1/24/90 added ndelay for reasonable slowness on AT
 * steve 10/4/90 kludged to make speed setting slightly more cogent
 * steve 10/9/90 made arrow keys work
 * steve 10/10/90 added -c option to work on video cards
 * steve 6/10/92 added #if _I386 conditionalization for COH386;
 *	cleaned up a few of the more offensive pointer/int fubars of dal.
 *	The i386 version must be setuid sys, so it can read /dev/mem.
 */

#include "term.c"
#include "io.c"

#if	_I386
#define	SCRBASE_M	0xB0000		/* monochrome screen base address */
#define	SCRBASE_C	0xB8000		/* color board screen base address */
#else
#define	SCRBASE_M	0xB000		/* monochrome screen base segment */
#define	SCRBASE_C	0xB800		/* color board screen base segment */
#endif

#define	rand()	((rn+=rn+((((rn<<0^rn<<1^rn<<8^rn<<9)>>15)^1)&1))&1)
int		rn;
#define	track()	(board>1)
#define	nprox()	(board>2? (board-3)/2+5 : 0)
#define	F_VERT	22
#define	F_HORZ	63
#define	F_HHID	2
#define	F_HOFF	((80-F_HORZ)/2)
#define	F_VOFF	((24-F_VERT)/2)
#define	S_BP	10000
#define	S_PN	3
#define	S_GH	200
#define	S_FD	10
#define	S_EN	50
/*
#define	S_FR	(board<5? (board-1)*200+100 : (board-4)*1000)
*/
#define	S_FR	((board-1)*200+100)
#define	F_GH	0
#define	F_PC	4
#define	F_FR	5
#define	F_BN	6
#define	F_FD	7
#define	F_EN	8
#define	F_DR	9
#define	F_TN	10
#define	F_WL	11
#define	F_BC	12
#define	F_BMSK	((1<<F_BC)-1)
#define	F_GMSK	(((1<<F_GH+4)-1)&~((1<<F_GH+0)-1))
#define	G_ETIM	24
#define	G_BTIM	8
#define	G_TMQN	8
#define	G_TOFF(n)	(5+(n)*G_TMQN)
enum	{G_ERAS, G_WRIT, G_MOVE, G_BLUE, G_INIT};
#define	E_TMQN	16
#define	E_TOFF	1
#define	P_TMQN	(9*(2-P_HALF))
#define	P_TOFF	0
enum	{P_ERAS, P_WRIT, P_MOVE, P_UPDI, P_INIT};
#define	axis(v)	((v)&1)
#define	incr(v)	(1-((v)&2))
#define	hm(v)	(axis(v)==0)
#define	vm(v)	(axis(v)!=0)
#define	om(v)	((v)^2)

int		ndelay = 10;
int		limit[2] = {F_HORZ-2, F_VERT-1};
int		edibles;
int		pacmen = S_PN+1;
int		stop;
int		e_s;
long		score;
long		bonus;
int		board = 1;
int		p_p[3];
int		p_h[2];
int		p_l;
int		p_m;
int		p_d;
int		p_i[3];
unsigned	p_n;
unsigned	e_n;
int		e_k;
int		energ[4][2];
int		r_s;
int		r_t[2];
int		r_d;
int		r_p[2];
int		g_g[2][2];
int		g_v[4];
struct	ghost	{
	int		g_p[3];
	int		g_q[3];
	int		g_h[3];
	int		g_s;
	int		g_d;
	int		g_e;
	int		g_f;
	int		g_r;
	int		g_o;
	unsigned 	g_n;
} ghost[4];

#define	D	(1<<F_DR)
#define	E	(1<<F_EN)
#define	F	(1<<F_FD)
#define	G	(1<<F_GH)
#define	P	(1<<F_PC)
#define	R	(1<<F_FR)
#define	T	(1<<F_TN)
#define	W	(1<<F_WL)
int	field[F_VERT][F_HORZ] = {
{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
{W,W,W,W,W,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,W,W},
{W,W,W,W,W,0,F,0,W,W,W,W,W,W,W,0,F,0,W,W,W,W,W,W,W,W,W,0,F,0,W,W},
{W,W,W,W,W,0,E,0,W,W,W,W,W,W,W,0,F,0,W,W,W,W,W,W,W,W,W,0,F,0,W,W},
{W,W,W,W,W,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0},
{W,W,W,W,W,0,F,0,W,W,W,W,W,W,W,0,F,0,W,W,W,0,F,0,W,W,W,W,W,W,W,W},
{W,W,W,W,W,0,F,0,F,0,F,0,F,0,F,0,F,0,W,W,W,0,F,0,F,0,F,0,F,0,W,W},
{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,0,F,0,W,W,W,W,W,W,W,W,W,0,0,0,W,W},
{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,0,F,0,W,W,W,0,0,0,0,0,0,0,0,0,0,G},
{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,0,F,0,W,W,W,0,0,0,W,W,W,W,W,W,D,D},
{T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,0,F,0,0,0,0,0,0,0,W,W,W,0,G,0,0,G},
{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,0,F,0,W,W,W,0,0,0,W,W,W,W,W,W,W,W},
{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,0,F,0,W,W,W,0,0,0,0,0,0,0,0,0,0,R},
{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,0,F,0,W,W,W,0,0,0,W,W,W,W,W,W,W,W},
{W,W,W,W,W,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,W,W},
{W,W,W,W,W,0,F,0,W,W,W,W,W,W,W,0,F,0,W,W,W,W,W,W,W,W,W,0,F,0,W,W},
{W,W,W,W,W,0,E,0,F,0,F,0,W,W,W,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,0,P},
{W,W,W,W,W,W,W,W,W,0,F,0,W,W,W,0,F,0,W,W,W,0,F,0,W,W,W,W,W,W,W,W},
{W,W,W,W,W,0,F,0,F,0,F,0,F,0,F,0,F,0,W,W,W,0,F,0,F,0,F,0,F,0,W,W},
{W,W,W,W,W,0,F,0,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,0,F,0,W,W},
{W,W,W,W,W,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0,F,0},
{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W}
};
#undef	D
#undef	E
#undef	F
#undef	G
#undef	P
#undef	R
#undef	T
#undef	W

int	cn[3];
unsigned	scrbase = SCRBASE_M;	/* monochrome screen by default */

nc(x, y, z)
{
	cn[0]=(x);
	cn[1]=(y);
	cn[2]=(z);
	mc(cn);
	return (field[cn[1]][cn[0]]);
}

mc(p)
register *p;
{
	register a, i;

	a = axis(p[2]);
	i = incr(p[2]);
	if ((unsigned)(p[a]+=i) > limit[a])
		p[a] -= i*(limit[a]+1);
}


main(argc, argv) int argc; char *argv[];
{
	char c;

	if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'c') {
		--argc;
		++argv;
		scrbase = SCRBASE_C;		/* color screen if -c */
	}
	if (argc > 1)
		ndelay = atoi(argv[1]);
	setup();
	map();
loop:
	cls();
	if (argc == 1) {
		atrb(s_a);
		curs(1, 0);
		bufstr("Speed [default ");
		dprint((long)ndelay);
		bufstr("]? ");
		bflush();
		inputecho(&c);
		if (c == '\n')
			;		/* take old ndelay, initially 10 */
		else if (c < '0' || c > '9')
			goto loop;	/* bogus nachos */
		else			/* input ndelay char by char, ugh */
			for (ndelay = 0; c >= '0' && c <= '9'; inputecho(&c)) {
				ndelay *= 10;
				ndelay += c - '0';
			}
		if (c != '\n')
			goto loop;	/* bogus terminator */
		curs(1, 0);
		bufstr("                         ");
		bflush();
	}
	score = 0;
	bonus = S_BP;
	curs(1, 0);
	bufstr("      ");
	while (play())
		++board;
	for (;;) {
		cls();
		atrb(s_a);
		curs(1, 0);
		bufstr("Your score was ");
		dprint(score);
		bufstr(" on level ");
		bufchr(board + 'A' - 1);
		bufstr(" at speed ");
		dprint((long)ndelay);
		bufchr('.');
		curs(2, 0);
		bufstr("Do you want to play again [y or n]? ");
		bflush();
		c = 0;
		inputecho(&c);
		switch (c) {
		case 'Y':
		case 'y':
		case '\n':
			board = 1;
			edibles = 0;
			pacmen = S_PN+1;
			goto loop;
		case 'Q':
		case 'q':
		case 'N':
		case 'n':
			reset();
			exit(0);
		}
	}
}

play()
{
	register i, j;
	register *f;
	register k;

	while (pacmen) {
		e_n = E_TOFF;
		e_k = 0;
		r_c[1] = board-1+'A';
		if (edibles == 0) {
			f = field;
			for (j=0; j<F_VERT; ++j)
				for (i=0; i<F_HORZ; ++i)
					if ((*f++&=~(1<<F_BN|1<<F_PC|F_GMSK))
					 &  (1<<F_FD|1<<F_EN))
						++edibles;
			plot(0, 0, F_HORZ, F_VERT);
			if (score == 0) {
				pleft();
				--pacmen;
				r_d = edibles/2;
				i = rand();
				r_t[0] = (1+0)*edibles/(2+i);
				i = rand();
				r_t[1] = (2+i)*edibles/(3+i);
				curs(1, 0);
				atrb(s_a);
				bufchr('0');
			}
		}
		pacman(P_INIT, 0);
		pacman(P_UPDI, 0);
		ghosts(G_INIT, 0, F_GMSK);
		ghosts(G_WRIT, 1, F_GMSK);
		curs(2, 0);
		atrb(r_c[0]);
		bufchr(r_c[1]);
		curs(p_h[1]+F_VOFF, p_h[0]-4+F_HOFF);
		atrb(s_a);
		bufstr("GET READY");
		delay(10);
		pleft();
		delay(5);
		plot(p_p[0]-4, p_p[1], 9, 1);
		pacman(P_WRIT, 1);
		animate();
		if (stop) {
			if (r_s) {
				r_s = 0;
				plot(r_p[0], r_p[1], 1, 1);
			}
			k = 0;
			while ((stop&1<<k+F_GH) == 0)
				++k;
			f = &ghost[k].g_q[2];
			ghosts(G_ERAS, 1, ~stop&F_GMSK);
			beep();
			for (i=0; i<12; ++i) {
				*f ^= 2;
				ghosts(G_WRIT, 1, stop);
				delay(1);
				ghosts(G_ERAS, 1, stop);
			}
			for (i=0; i<12; ++i) {
				p_p[2] += (p_d)? -1: 1;
				p_p[2] &= 3;
				pacman(P_WRIT, 1);
				delay(1);
				pacman(P_ERAS, 1);
			}
			--pacmen;
		} else
			return (1);
	}
	return (0);
}

map()
{
	register x, y, z;
	register e, g;
	register *f;

	e = 0;
	g = 0;
	f = field;
	for (y=0; y<F_VERT; ++y)
		for (x=0; x<=F_HORZ/2; ++x)
			field[y][F_HORZ-x-1] = field[y][x];
	for (y=0; y<F_VERT; ++y)
		for (x=0; x<F_HORZ; ++f, ++x) {
			if (*f & (1<<F_EN)) {
				energ[e][0] = x;
				energ[e][1] = y;
				++e;
			} else if (*f & (1<<F_GH)) {
				ghost[g].g_h[0] = x;
				ghost[g].g_h[1] = y;
				*f &= ~(1<<F_GH);
				if ((g++&1) == 0) {
					g_g[g/2][0] = x;
					g_g[g/2][1] = y;
				}
			} else if (*f & (1<<F_PC)) {
				p_h[0] = x;
				p_h[1] = y;
				*f &= ~(1<<F_PC);
			} else if (*f & (1<<F_FR)) {
				r_p[0] = x;
				r_p[1] = y;
			}
			for (z=0; z<4; ++z)
				if (nc(x, y, z) & 1<<F_WL)
					*f |= 1<<F_WL+z+1;
		}
}


animate()
{
	register i, c;
	register unsigned clock;
	register struct ghost *g;
	register *p;
	char key = '\0';
	long oscore;
	int gscore;

	oscore = score;
	e_s = 0;
	stop = 0;
	for (clock=0; !stop && edibles; ++clock) {
		i = 0;
		if (input(&key) || p_n==clock) switch (key) {
		case KEY_N:
			++i;
		case KEY_W:
			++i;
		case KEY_S:
			++i;
		case KEY_E:
			if (vm(i) && hm(p_p[2])) {
				c = nc(p_p[0], p_p[1], i);
				if (c & (1<<F_DR|1<<F_WL|1<<F_WL+p_p[2]+1))
					break;
				if (c & 1<<F_WL+om(p_p[2])+1)
					mc(p_p);
			} else if (hm(i) && vm(p_p[2])) {
				if (nc(p_p[0]+incr(i), p_p[1]+p_l, i) & 1<<F_WL)
					break;
				p_p[1] += p_l;
				p_l = 0;
			} else if (p_p[2] == i) {
				if (p_m) {
					key = '\0';
					break;
				}
			} else if (vm(i) && p_l) {
				p_p[1] += p_l;
				p_l = -p_l;
			}
			p_p[2] = i;
			p_m = 1;
			p_n = clock;
			key = '\0';
			break;
		case KEY_C:
			p_m = 0;
			key = '\0';
			break;
		}
		if (p_n == clock) {
			if (ndelay)
				delay(1);
			pacman(P_ERAS, 1);
			pacman(P_MOVE, 0);
			pacman(P_UPDI, 0);
			p_d ^= 1;
			pacman(P_WRIT, 1);
			p = &field[p_p[1]+p_l][p_p[0]];
			for (i=-1; i<2; ++i) {
				if (r_s && (p[i]&1<<F_FR)) {
					score += S_FR;
					r_s = 0;
					atrb(s_a);
					curs(r_p[1]+F_VOFF, r_p[0]+F_HOFF);
					c = dprint((long)S_FR);
					delay(5);
					plot(r_p[0], r_p[1], c, 1);
					continue;
				}
				if ((c=p[i]) & 1<<F_BN)
					continue;
				p[i] = c | 1<<F_BN;
				if (c & 1<<F_FD) {
					score += S_FD;
				} else if (c & 1<<F_EN) {
					ghosts(G_BLUE, 1, F_GMSK);
					gscore = S_GH;
					score += S_EN;
				} else
					continue;
				--edibles;
				if (edibles==r_t[0] || edibles==r_t[1]) {
					r_s = r_d;
					plot(r_p[0], r_p[1], 1, 1);
				}
			}
			if (r_s && --r_s==0)
				plot(r_p[0], r_p[1], 1, 1);
			if (e_s) for (g=ghost,i=1<<F_GH; i&F_GMSK; i<<=1,++g) {
				if (g->g_s!=1 || contact(g->g_p, 3)==0)
					continue;
				curs(g->g_p[1]+F_VOFF, g->g_p[0]-1+F_HOFF);
				beep();
				atrb(s_a);
				c = dprint((long)gscore);
				delay(5);
				atrb(0);
				curs(g->g_p[1]+F_VOFF, g->g_p[0]-1+F_HOFF);
				while (c--)
					bufchr(' ');
				g->g_s = 2;
				g->g_e = 0;
				plot(g->g_p[0]-1, g->g_p[1], 4, 1);
				score += gscore;
				gscore *= 2;
			}
		}
		for (g=ghost,i=1<<F_GH; i&F_GMSK; i<<=1,++g) {
			if (g->g_n != clock)
				continue;
			ghosts(G_ERAS, 1, i);
			ghosts(G_MOVE, 0, i);
			ghosts(G_WRIT, 1, i);
		}
		if (e_n == clock) {
			++e_k;
			p = energ[(e_k&=7)&3];
			plot(p[0], p[1], 1, 1);
			e_n += E_TMQN;
		}
		if (score != oscore) {
			curs(1, 0);
			atrb(s_a);
			dprint(score);
			if (score>=bonus && oscore<bonus) {
				bonus += S_BP;
				if (pacmen < S_PN+1) {
					beep();
					++pacmen;
					pleft();
				}
			}
			oscore = score;
		}
		bflush();
	}
}

dprint(l)
long l;
{
	register c, d = 1, e = 0;
	static long mag[] = {1, 10, 100, 1000, 10000, 100000};

	while (mag[d] <= l)
		++d;
	while (d--) {
		for (c=0; l>=mag[d]; ++c)
			l -= mag[d];
		bufchr(c+'0');
		++e;
	}
	return (e);
}

pleft()
{
	register i, j;
	register char *c;

	for (i=0; i<S_PN+1; ++i) {
		curs(22-2*i, 0);
		c = p_cc[pacmen>i+1? 0 : 1];
		for (j=0; j<3; ++j) {
			atrb(*c++);
			bufchr(*c++);
		}
	}
}

pacman(s, p)
{
	register i;
	register *f;

	switch (s) {
	case P_ERAS:
		for (i=0; i<p_i[2]; ++i) {
			f = &field[p_i[1]+i][p_i[0]];
			*f++ &= ~(1<<F_PC);
			*f++ &= ~(1<<F_PC);
			*f   &= ~(1<<F_PC);
		}
		plot(p_i[0], p_i[1], 3, p_i[2]);
		break;
	case P_WRIT:
		for (i=0; i<p_i[2]; ++i) {
			f = &field[p_i[1]+i][p_i[0]];
			*f++ |= (1<<F_PC);
			*f++ |= (1<<F_PC);
			*f   |= (1<<F_PC);
		}
		break;
	case P_MOVE:
		p_n += (P_TMQN*2-p_n%(P_TMQN*2)+P_TOFF);
		if (p_m) {
			i = nc(p_p[0], p_p[1], p_p[2]);
			if (vm(p_p[2]))
				if (p_l) {
					p_p[1] += p_l;
					p_l = 0;
				} else if (i & (1<<F_WL|1<<F_DR))
					p_m = 0;
				else if (P_HALF)
					p_l = incr(p_p[2]);
				else
					p_p[1] += incr(p_p[2]);
			else if (i & 1<<F_WL+p_p[2]+1)
				p_m = 0;
			else {
				mc(p_p);
				if (!P_HALF && (p_p[0]&1))
					mc(p_p);
			}
		}
		break;
	case P_UPDI:
		p_i[0] = p_p[0]-1;
		p_i[1] = p_p[1]+(p_l>>1);
		p_i[2] = 1+(p_l&1);
		break;
	case P_INIT:
		p_p[0] = p_h[0];
		p_p[1] = p_h[1];
		p_p[2] = 1;
		p_l = 0;
		p_m = 0;
		p_d = 0;
		p_n = P_TOFF;
		break;
	}
	if (p)
		plot(p_i[0], p_i[1], 3, p_i[2]);
}

ghosts(s, p, m)
{
	register struct ghost *g;
	register *f;
	register i;

	for (g=ghost,i=1<<F_GH; i&F_GMSK; i<<=1,++g) {
		if ((m&i) == 0)
			continue;
		switch (s) {
		case G_ERAS:
			f = &field[g->g_p[1]][g->g_p[0]];
			*f &= ~i;
			*(f-1) &= ~i;
			*(f+1) &= ~i;
			break;
		case G_WRIT:
			f = &field[g->g_p[1]][g->g_p[0]];
			*f |= i;
			*(f-1) |= i;
			*(f+1) |= i;
			break;
		case G_MOVE:
			g->g_p[0] = g->g_q[0];
			g->g_p[1] = g->g_q[1];
			g->g_p[2] = g->g_q[2];
			f = g_v;
			switch (g->g_s) {
			case 0:
				if (field[g->g_p[1]][g->g_p[0]] & 1<<F_TN)
					g->g_n += G_TMQN*8;
				else
					g->g_n += G_TMQN*4;
				if (g->g_r > 0)
					if (--g->g_r) {
						g->g_f ^= 2;
						break;
					} else {
						--g->g_r;
						g->g_f = 0;
					}
				if (g->g_r) {
					if (vector(g, g_g[0], 0))
						g->g_r = 0;
				} else if (contact(g->g_p, 3) && stop<i) {
					stop = i;
					break;
				} else if (track() && sight(g->g_p)) {
					vector(g, p_p, p_l);
				} else if (nprox() && prox(g->g_p)<=nprox()) {
					vector(g, p_p, p_l);
				} else {
					g_v[0] = g->g_p[2];
					g_v[1] = 3&(g->g_p[2]+
						 (((g-ghost)^g->g_p[2])&2)-1);
					g_v[2] = g_v[1]^2;
					g_v[3] = g_v[0]^2;
					f += (rand()&rand());
				}
				goto select;
			case 1:
				if (--g->g_e == 0) {
					g->g_s = 0;
					g->g_d = 0;
					--e_s;
				} else if (g->g_e <= G_BTIM)
					g->g_d ^= 1;
				g->g_n += G_TMQN*8;
				vector(g, p_p, p_l);
				f = g_v+2;
				goto select;
			case 2:
				if (g->g_r == 0) {
					g->g_r--;
					g->g_d = 0;
				}
				g->g_n += G_TMQN*2;
				if (vector(g, g_g[1], 0)) {
					g->g_q[2] ^= 2;
					while (g->g_n%(G_TMQN*4) != g->g_o)
						g->g_n += G_TMQN*2;
					g->g_s = 0;
					g->g_f = 0;
					--e_s;
					break;
				}
				f = g_v;
			select:
				if (vm(*f)
				&&  nc(g->g_p[0], g->g_p[1], *f)
				 &  (1<<F_WL|1<<F_WL+1|1<<F_WL+3
				 |  (!g->g_r)<<F_DR)
				   ) {
					if (++f-g_v > 3)
						f = g_v;
					goto select;
				}
				if (hm(*f)
				&&  nc(g->g_p[0]+2*incr(*f), g->g_p[1], *f)
				 &  1<<F_WL
				   ) {
					if (++f-g_v > 3)
						f = g_v;
					goto select;
				}
				if ((*f^g->g_p[2])==2 && g->g_e<G_ETIM-1) {
					if (++f-g_v > 3)
						f = g_v;
					goto select;
				}
				g->g_q[2] = *f;
				do {
					mc(g->g_q);
					if (vm(g->g_q[2])
					||  (g->g_q[0]&1) == 0
					   )
						break;
					if (g->g_r
					&& (g->g_q[0] == g_g[0][0]
					&&  g->g_q[1] == g_g[0][1]
					||  g->g_q[0] == g_g[1][0]
					&&  g->g_q[1] == g_g[1][1]
					   ))
						break;
					mc(g->g_q);
				} while (0);
				if (g->g_p[2] == g->g_q[2])
					g->g_f ^= 2;
				else if ((g->g_p[2]^g->g_q[2]) & 1)
					g->g_f = 0;
			}
			break;
		case G_BLUE:
			if (g->g_s < 2) {
				if (g->g_s == 0)
					if (g->g_r)
						break;
					else
						++e_s;
				g->g_s = 1;
				g->g_e = G_ETIM;
				g->g_d = 0;
			}
			break;
		case G_INIT:
			g->g_p[0] = g->g_q[0] = g->g_h[0];
			g->g_p[1] = g->g_q[1] = g->g_h[1];
			g->g_q[2] = g_i[g-ghost];
			g->g_s = 0;
			g->g_d = 0;
			g->g_e = 0;
			g->g_f = 0;
			g->g_r = 12*((g-ghost&1)*2+(g-ghost&2)/2);
			g->g_n = g->g_o = G_TOFF(g-ghost);
			break;
		}
		if (p)
			plot(g->g_p[0]-1, g->g_p[1], 3, 1);
	}
}

plot(x, y, nx, ny)
{
	register b, f;
	register struct ghost *g;
	register char *c;
	register int *p;
	register i;
	register z0, z1, z2, z3;

	if (x < 0) {
		nx -= x;
		x = 0;
	}
	if ((nx+=x) > F_HORZ-F_HHID)
		nx = F_HORZ-F_HHID;
	for (ny+=y; y<ny; ++y) {
		curs(y+F_VOFF, x+F_HOFF);
		p = &field[y][x];
		for (i=x; i<nx; ++i) {
			if ((f=*p++&F_BMSK)==0 || i<F_HHID)
				c = b_c;
			else {
				for (b=0; (f&1)==0; ++b)
					f >>= 1;
				switch (b) {
				case F_GH+0:
				case F_GH+1:
				case F_GH+2:
				case F_GH+3:
					g = ghost + b - F_GH;
/*
					c = g_c[b-F_GH]
					       [g->g_q[2]^(g->g_f&P_GFLP)]
					       [g->g_s-g->g_d]
					       [i-g->g_p[0]+1];
*/
					z0 = b-F_GH;
					z1 = g->g_q[2]^(g->g_f&P_GFLP);
					z2 = g->g_s-g->g_d;
					z3 = i-g->g_p[0]+1;
					c = g_c[z0][z1][z2][z3];
					break;
				case F_PC:
					c = p_c[y-p_i[1]+(p_l&1)][p_p[2]]
					       [p_d][i-p_i[0]];
					break;
				case F_FR:
					c = r_s? r_c : b_c;
					break;
				case F_BN:
					c = b_c;
					break;
				case F_FD:
					c = f_c;
					break;
				case F_EN:
					c = e_c[e_k>>2];
					break;
				case F_DR:
					c = d_c;
					break;
				case F_TN:
					c = b_c;
					break;
				case F_WL:
					c = w_c;
					break;
				}
			}
			atrb(*c++);
			bufchr(*c);
		}
	}
}

contact(p, n)
register *p;
{
	register dx, dy;

	dx = p_p[0] - p[0];
	dy = p_p[1]*2 + p_l - p[1]*2;
	if ((dy==0 && n>dx && dx>-n)
	||  (dx==0 && n>dy && dy>-n)
	   )
		return (1);
	return (0);
}

prox(p)
register *p;
{
	register dx, dy;

	if ((dx=p_p[0]-p[0]) < 0)
		dx = -dx;
	if ((dy=p_p[1]*2+p_l-p[1]*2) < 0)
		dy = -dy;
	if (dy > dx)
		return (dy);
	return (dx);
}

vector(g, t, o)
register struct ghost *g;
register *t;
{
	register i;
	int d[2], s[2] = {0, 0};

	if ((d[0]=t[0]-g->g_p[0]) < 0) {
			d[0] =- d[0];
			s[0] =  2;
	} else if (d[0] == 0)
		if (hm(g->g_p[2]))
			s[0] = g->g_p[2]&2^2;
		else
			s[0] = rand()*2;
	if ((d[1]=t[1]*2+o-g->g_p[1]*2) < 0) {
			d[1] =- d[1];
			s[1] =  2;
	} else if (d[1] == 0)
		if (vm(g->g_p[2]))
			s[1] = g->g_p[2]&2^2;
		else
			s[1] = rand()*2;
	if (d[0] > d[1])
		i = 0;
	else if (d[1] > d[0])
		i = 1;
	else
		i = axis(g->g_p[2]);
	t = g_v;
	*t++ = i | s[i];
	*t++ = (i^1) | s[i^1];
	*t++ = (i^1) | (s[i^1]^2);
	*t   = i | (s[i]^2);
	if (d[0] | d[1])
		return (0);
	return (1);
}

sight(p)
register *p;
{
	register v;
	int q[3];

	if (p[0] != p_p[0]
	&&  p[1] != p_p[1]
	   )
		return (0);
	for (v=p[2]-1; v<=p[2]+1; ++v) {
		q[0] = p[0];
		q[1] = p[1];
		q[2] = v&3;
		while ((field[q[1]][q[0]]&1<<F_WL) == 0) {
			if (q[0]==p_p[0] && q[1]==p_p[1])
				return (1);
			mc(q);
		}
	}
	return (0);
}
