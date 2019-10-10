/*
 * C preprocessor.
 * Machine and/or system specific
 * header file.
 */
#define	MACHINE	 "IAPX86"

#ifdef	UDI
#define	LOCATION	"SERIESIII"
#define	SYSTEM		"UDI"
#define	DEFDISK		""
#endif

#ifdef	COHERENT
#define	LOCATION	"MWC"
#define	SYSTEM		"COHERENT"
#ifdef	FLOPPY
#define	DEFDISK		"/lib/include"
#else
#define	DEFDISK		"/usr/include"
#endif
#endif

#ifdef	vax
#define	LOCATION	"VAX"
#define	SYSTEM		"UDI"
#define	DEFDISK		"CC86$INCLUDE:"
#endif

#ifdef	MSDOS
#define	LOCATION	"MWC86"
#define	SYSTEM		"MSDOS"
#define	DEFDISK		""
#endif
