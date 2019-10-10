//////////
/ n1/i386/tables/prefac.f
/ Code table preface.
/ Useful abbreviations for code tables.
/ Must be the first file in the tabgen input list.
/ i386.
////////

#define	BYTE	(FS8|FU8)			/* [u]char		*/
#define	WORD	(FS16|FU16)			/* [u]short		*/
#define	SHORT	(BYTE|WORD)			/* [u]char or [u]short	*/
#define	LONG	(FS32|FU32)			/* [u]int or [u]long	*/
#define	PTX	(FPTR|FPTB)			/* pointer		*/
#define	DWORD	(LONG|PTX)			/* 32-bit incl. pointer	*/
#define	SINT	(FS32|FS16|FS8)			/* signed integers	*/
#define	UINT	(FU32|FU16|FU8)			/* unsigned integers	*/
#define	FLD	(FFLD32|FFLD16|FFLD8)		/* bitfields		*/
#define	FLOAT	(FF32|FF64)			/* float or double	*/
#define	NFLT	(BYTE|WORD|DWORD)		/* integer or pointer	*/
#define	NDPARG	(FLOAT|FS16|FS32)		/* 80x87 args		*/
#define	PEREL	(PEQ|PNE)			/* equality relations	*/
#define	PSREL	(PEQ|PNE|PGT|PGE|PLT|PLE)	/* signed relations	*/
#define	PREL	(PEQ|PNE|PGT|PGE|PLT|PLE|PUGT|PUGE|PULT|PULE)	/* relations */
#define	PVALUE	(PRVALUE|PLVALUE)		/* value contexts	*/

//////////
/ end of n1/i386/tables/prefac.f
//////////
