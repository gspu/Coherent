/*
 * Crypt subroutines - which use the
 * DES (Data Encryption Standard)
 * as published in National Bureau of Standards,
 * "Encryption Algorithm for Computer Data Encryption",
 * Vol. 40, No. 52, March 17, p.. 12134-12139.
 * (Reprinted in Cryptologia, Vol. I, No. 3, July 1977).
 */

#include <stdio.h>
#include <string.h>

#define	NBPC	8		/* Bits in a char */
#define	NIB	64		/* Input bits in a unit */
#define	NKB	64		/* Number of key bits */
#define	NUKB	56		/* Number of used key bits */
#define	NOKB	48		/* Number out from each key iteration */
#define	NITER	16		/* Number of iterations of S-functions */
#define	NSBOX	8		/* Number of S-boxes */
#define	NSCSET	64		/* Number in small character set */

/*
 * Table of Initial permutation of each 64 bit entity.
 */
static	char	IP[NIB] = {
	57, 49, 41, 33, 25, 17, 9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7,
	56, 48, 40, 32, 24, 16, 8, 0,
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6
};

/*
 * Inverted intial permutation ( IP -1)
 */
static	char	IP1[NIB] = {
	39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30,
	37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28,
	35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26,
	33, 1, 41, 9, 49, 17, 57, 25,
	32, 0, 40, 8, 48, 16, 56, 24
};

/*
 * E bit-selection table
 */
static	char	E[NOKB] = {
	31, 0, 1, 2, 3, 4,
	3, 4, 5, 6, 7, 8,
	7, 8, 9, 10, 11, 12,
	11, 12, 13, 14, 15, 16,
	15, 16, 17, 18, 19, 20,
	19, 20, 21, 22, 23, 24,
	23, 24, 25, 26, 27, 28,
	27, 28, 29, 30, 31, 0
};

/*
 * A saved copy of the E-table for
 * crypt to perturb.
 */
static	char	saveE[NOKB] = {
	31, 0, 1, 2, 3, 4,
	3, 4, 5, 6, 7, 8,
	7, 8, 9, 10, 11, 12,
	11, 12, 13, 14, 15, 16,
	15, 16, 17, 18, 19, 20,
	19, 20, 21, 22, 23, 24,
	23, 24, 25, 26, 27, 28,
	27, 28, 29, 30, 31, 0
};

/*
 * Permutation of 32-bits onto 32 bits
 * known as "P"
 */
static	char	P[NIB/2] = {
	15, 6, 19, 20,
	28, 11, 27, 16,
	0, 14, 22, 25,
	4, 17, 30, 9,
	1, 7, 23, 13,
	31, 26, 2, 8,
	18, 12, 29, 5,
	21, 10, 3, 24
};

/*
 * The following are the 8 S-box functions
 * (S1, S2, S3, S4, ..., S8)
 * Each turns a 6-bit quantity into a four bit number.
 */
static	char	Sboxes[NSBOX][4][16] = {
	/* S1 */
		14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
		0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
		4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
		15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13,
	/* S2 */
		15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
		3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
		0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
		13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9,
	/* S3 */
		10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
		13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
		13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
		1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12,
	/* S4 */
		7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
		13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
		10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
		3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14,
	/* S5 */
		2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
		14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
		4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
		11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3,
	/* S6 */
		12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
		10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
		9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
		4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13,
	/* S7 */
		4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
		13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
		1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
		6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12,
	/* S8 */
		13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
		1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
		7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
		2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11
};

/*
 * Permuted choice tables
 * PC-1 is used for first iteration of KS
 * and PC-2 is used therafter.
 */
static	char	PC1[NUKB] = {
	56, 48, 40, 32, 24, 16, 8,
	0, 57, 49, 41, 33, 25, 17,
	9, 1, 58, 50, 42, 34, 26,
	18, 10, 2, 59, 51, 43, 35,
	/* Division between Co and Do */
	62, 54, 46, 38, 30, 22, 14,
	6, 61, 53, 45, 37, 29, 21,
	13, 5, 60, 52, 44, 36, 28,
	20, 12, 4, 27, 19, 11, 3
};

static	char	PC2[NOKB] = {
	13, 16, 10, 23, 0, 4,
	2, 27, 14, 5, 20, 9,
	22, 18, 11, 3, 25, 7,
	15, 6, 26, 19, 12, 1,
	40, 51, 30, 36, 46, 54,
	29, 39, 50, 44, 32, 47,
	43, 48, 38, 55, 33, 52,
	45, 41, 49, 35, 28, 31
};

/*
 * Left shift table
 */
static	char	shifts[NITER] = {
	1, 1, 2, 2, 2, 2, 2, 2,
	1, 2, 2, 2, 2, 2, 2, 1
};

/*
 * Table to map a 6-bit integer onto the
 * smaller than ascii character set
 * ([a-zA-Z0-9./]).
 */
static	char	maptab[NSCSET] = {
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
	'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
	'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
	'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F',
	'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
	'W', 'X', 'Y', 'Z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '.', '/'
};

static	char	Ki[NITER][NOKB];

/*
 * Encrypt the block (one bit per byte)
 * using the (broken) DES algorithm.
 * The `edflag' is zero for encrypt,
 * non-zero for decrypt.
 */
encrypt(block, edflag)
char block[NIB];
{
	char block1[NIB], block2[NIB];

	permute(block, block1, IP, NIB);
	cypher(block1, block2, edflag);
	permute(block2, block, IP1, NIB);
}

/*
 * Setkey takes an array of NIB (64)
 * bytes called `key', each of which
 * contains one bit of the desired key.
 * This uses the DES Key Schedule (KS) function
 * to iteratively produce each of the NITER (16)
 * Ki's from the original key.
 */
setkey(key)
char key[NKB];
{
	char cd[NUKB];
	register ni;

	permute(key, cd, PC1, NUKB);
	for (ni=0; ni<NITER; ni++) {
		lrot(cd, shifts[ni], NUKB/2);
		lrot(&cd[NUKB/2], shifts[ni], NUKB/2);
		permute(cd, &Ki[ni][0], PC2, NOKB);
	}
}

/*
 * Crypt - the password encryption routine.
 * The key here, is the user's typed password
 * and the second is a 2-character salt in
 * the set "[a-zA-Z0-9./]".  The salt
 * changes the E-table in one of 4096 ways
 * to frustrate the use of hardware to crack
 * the DES code.
 */
char *
crypt(key, salt)
char *key, *salt;
{
	int sixbit;
	char stuff[NIB];
	char bkey[NKB];
	static char passwd[16];
	register char *cp, *bp;
	register i;

	perturb(salt);
	cp = key;
	bp = bkey;
	while (*cp!='\0' && bp<&bkey[NKB/NBPC])
		*bp++ = *cp++;
	while (bp < &bkey[NKB/NBPC])
		*bp++ = '\0';
	unpack(bkey, NKB);
	setkey(bkey);
	memcpy(stuff, "Password", NIB/NBPC);
	unpack(stuff, NIB);
	encrypt(stuff, 0);
	cp = passwd;
	*cp++ = salt[0];
	*cp++ = salt[1];
	bp = stuff;
	i = sixbit = 0;
	while (i < NIB) {
		sixbit = (sixbit<<1) + *bp++;
		if ((i++%6) == 6-1) {
			*cp++ = maptab[sixbit];
			sixbit = 0;
		}
	}
	*cp++ = maptab[sixbit];
	*cp = '\0';
	return (passwd);
}

/*
 * Perturb the E-table based on 2 6-bit characters
 * in the salt.  It produces a random permutation for
 * the E-table.
 */
static
perturb(salt)
char salt[2];
{
	register char *cp;
	register i1, i2;
	int salti;

	salti = 01<<8 | 01;
	i1 = salt[0];
	i2 = salt[1];
	for (cp = maptab; cp < &maptab[NSCSET]; cp++) {
		if (i1 == *cp)
			salti |= (cp-maptab)<<1;
		if (i2 == *cp)
			salti |= (cp-maptab)<<9;
	}
	srand(salti);
	/*
	 * Make new E-table, from the saved E-table
	 * with random junk added to it (mod NOKB).
	 */
	for (i1 = 0; i1 < NOKB; i1++)
		E[i1] = (saveE[i1] + (rand()>>6)) % (NIB/2);
}

/*
 * Permute incoming bits into outgoing
 * chunk with permutation table (`ptab').
 * `nbites' is the number of bits to
 * produce from the transformation.
 */
static
permute(ibits, obits, ptab, nbits)
register char *ibits;
register char *obits;
char *ptab;
{
	register bitno;

	for (bitno = 0; bitno<nbits; bitno++)
		*obits++ = ibits[ptab[bitno]];
}

/*
 * The intermediate cypher function
 * It does a chunk of NIB (64) bits
 * through the NITER (16) iterations
 * of the cypher function `f'.
 */
static
cypher(ibits, obits, edflag)
int edflag;
char ibits[NIB];
char obits[NIB];
{
	char t1bits[NOKB];
	char t2bits[NIB/2];
	char left[NIB/2], right[NIB/2];
	register int n;
	register int i;

	memcpy(left, ibits, NIB/2);
	memcpy(right, &ibits[NIB/2], NIB/2);
	for (n=0; n<NITER; n++) {
		/*
		 * L' = R
		 * R' = L + f(R,K)
		 */
		permute(right, t1bits, E, NOKB);
		i = edflag ? NITER-1-n : n;
		m2add(t1bits, &Ki[i][0], t1bits, NOKB);
		dosboxes(t1bits, t2bits);
		permute(t2bits, t1bits, P, NIB/2);
		m2add(t1bits, left, t2bits, NIB/2);
		memcpy(left, right, NIB/2);
		memcpy(right, t2bits, NIB/2);
	}
	memcpy(obits, right, NIB/2);
	memcpy(&obits[NIB/2], left, NIB/2);
}

/*
 * Rotate (what DES calls shift) left
 * `nbits' `bits' by `ns' places.
 * The shift count is assumed less than
 * NBPC (8).
 */
static
lrot(bits, ns, nbits)
char *bits;
{
	register i, nb;
	register char *bp;
	int byte;

	for (i=0; i<ns; i++) {
		bp = bits;
		byte = *bp;
		nb = nbits-1;
		do {
			bp[0] = bp[1];
			bp++;
		} while (--nb);
		*bp = byte;
	}
}

/*
 * Modulo 2 addition of bit vectors
 * `i1' and `i2' into `o'.
 * `nbits' is the number of bits in
 * each vector
 */
static
m2add(i1, i2, o, nbits)
register char *i1, *i2;
register nbits;
register char *o;
{
	do {
		*o++ = *i1++ ^ *i2++;
	} while (--nbits);
}

/*
 * Implement the "S-boxes"
 * Transforming from ibits (NOKB or 48 bits)
 * to obits (NIB/2 or 32 bits).
 * For each of these boxes, there is a
 * 6-bit chunk of the input with the row being
 * the first and last of these and the column
 * being the middle 4 bits.
 */
static
dosboxes(ibits, obits)
char ibits[NOKB];
char obits[NIB/2];
{
	register row, col;
	register boxno;
	int tempout;

	tempout = 0;
	for (boxno=0; boxno < NSBOX; boxno++) {
		row = col = 0;
		row = row<<1 | *ibits++;
		col = col<<1 | *ibits++;
		col = col<<1 | *ibits++;
		col = col<<1 | *ibits++;
		col = col<<1 | *ibits++;
		row = row<<1 | *ibits++;
		tempout = (tempout<<4) | Sboxes[boxno][row][col];
		for (col=0; col<4; col++) {
			*obits++ = (tempout>>3) & 01;
			tempout <<= 1;
		}
	}
}

/*
 * Unpack byte vector into bit vector
 */
static
unpack(bytes, nbits)
char *bytes;
unsigned nbits;
{
	register char *bitp;
	register char *bp;
	register i;
	int byte;

	bitp = &bytes[nbits];
	bp = &bytes[(nbits+NBPC-1)/NBPC];
	for (i=nbits; i>0; i--) {
		if ((i % NBPC) == 0)
			byte = *--bp;
		*--bitp = byte&01;
		byte >>= 1;
	}
}

/*
 * Pack bit vector into byte data.
 */
static
pack(bp, nbits)
register char *bp;
unsigned nbits;
{
	register i;
	register char *bitp;
	int byte;

	byte = 0;
	bitp = bp;
	for (i=0; i<nbits; i++) {
		byte = byte<<1 | *bitp++;
		if ((i%NBPC) == NBPC-1) {
			*bp++ = byte;
			byte = 0;
		}
	}
}
