/* tailor.h -- Not copyrighted 1992 Mark Adler */

/* const's are inconsistently used across ANSI libraries--kill for all
   header files. */
#define const


/* Define MSDOS for Turbo C as well as Microsoft C */
#ifdef __POWERC                 /* For Power C too */
#  define __TURBOC__
#endif /* __POWERC */
#if (defined(__TURBOC__) && !defined(MSDOS))
#  define MSDOS
#endif

#ifdef ATARI_ST
#  undef MSDOS   /* avoid the MS-DOS specific includes */
#endif

/* Use prototypes and ANSI libraries if _STDC__, or Microsoft or Borland C,
 * or Silicon Graphics, or IBM C Set/2, or GNU gcc under emx.
 */
#if (defined(__STDC__) || defined(MSDOS) || defined(sgi))
#  ifndef PROTO
#    define PROTO
#  endif /* !PROTO */
#  define MODERN
#endif /* MSDOS */

#if (defined(__IBMC__) || defined(__EMX__) || defined(ATARI_ST))
#  ifndef PROTO
#    define PROTO
#  endif /* !PROTO */
#  define MODERN
#endif


#ifdef __IBMC__
#  define S_IFMT 0xF000
#endif /* __IBMC__ */

#ifdef __EMX__
#  define __32BIT__
#endif /* __EMX__ */

#ifdef __WATCOMC__
#  define __32BIT__
#endif

#if (defined(__OS2__) && !defined(OS2))
#  define OS2
#endif

#ifdef __convexc__
#	define CONVEX
#endif /* __convexc__ */

/* Turn off prototypes if requested */
#if (defined(NOPROTO) && defined(PROTO))
#  undef PROTO
#endif


/* Used to remove arguments in function prototypes for non-ANSI C */
#ifdef PROTO
#  define OF(a) a
#else /* !PROTO */
#  define OF(a) ()
#endif /* ?PROTO */


/* Memory allocation. */
#ifdef MACOS
#  define DYN_ALLOC
#endif
#if (defined(MSDOS) && !defined(__GO32__) && !defined(WIN32))
#  ifdef __TURBOC__
#    include <alloc.h>
#    define DYN_ALLOC
     /* Turbo C 2.0 does not accept far static allocations in small model */
     void far * fcalloc OF((unsigned items, unsigned size));
#  else /* !__TURBOC__ */
#    include <malloc.h>
#    define farmalloc _fmalloc
#    define farfree   _ffree
#    define fcalloc(nitems,itemsize) halloc((long)(nitems),(itemsize))
#  endif /* ?__TURBOC__ */
#else /* !MSDOS */
#  if defined(WIN32)
#    include <malloc.h>
#  endif
#  ifdef far
#    undef huge
#    undef far
#    undef near
#  endif
#  define huge
#  define far
#  define near
#  define farmalloc malloc
#  define farfree   free
#  define fcalloc(items,size) calloc((unsigned)(items), (unsigned)(size))
#  ifndef PROTO
     extern char *calloc(); /* essential for 16 bit systems (AT&T 6300) */
#  endif
#endif /* ?MSDOS */


#if (defined(OS2) && !defined(MSDOS))
/* MSDOS is defined anyway with MS C 16-bit. So the block above works.
 * For the 32-bit compilers, MSDOS must not be defined in the block above. */
#  define MSDOS
/* inherit MS-DOS file system etc. stuff */
#endif
    

/* Define MSVMS if either MSDOS or VMS defined */
#if defined(MSDOS) || defined(VMS)
#  define MSVMS
#endif

/* case mapping functions. case_map is used to ignore case in comparisons,
 * to_up is used to force upper case even on Unix (for dosify option).
 */
#if defined(OS2) || defined(MSDOS) || defined(VMS)
#  define case_map(c) upper[c]
#  define to_up(c)    upper[c]
#else
#  define case_map(c) (c)
#  define to_up(c)    ((c) >= 'a' && (c) <= 'z' ? (c)-'a'+'A' : (c))
#endif

/* Define void, voidp, and extent (size_t) */
#include <stdio.h>
#ifdef MODERN
#  if (!defined(M_XENIX) && !(defined(__GNUC__) && defined(sun)))
#    include <stddef.h>
#  endif /* !M_XENIX */
#  include <stdlib.h>
   typedef size_t extent;
   typedef void voidp;
#else /* !MODERN */
   typedef unsigned int extent;
#  define void int
   typedef char voidp;
#endif /* ?MODERN */

/* Get types and stat */
#ifdef VMS
#  include <types.h>
#  include <stat.h>
#else /* !VMS */
#  ifdef MACOS
#    include <types.h>
#    include <stddef.h>
#    include <Files.h>
#    include <StandardFile.h>
#    include <Think.h>
#    include <LoMem.h>
#    include <Pascal.h>
#    include "macstat.h"
#  else
#    ifdef ATARI_ST
#      include <ext.h>
#      include <tos.h>
#    else
#      include <sys/types.h>
#      include <sys/stat.h>
#    endif
#  endif
#endif /* ?VMS */

/* Some systems define S_IFLNK but do not support symbolic links */
#if defined (S_IFLNK) && (defined(NO_SYMLINK) || defined(MACOS))
#  undef S_IFLNK
#endif

/* Cheap fix for unlink on VMS */
#ifdef VMS
#  define unlink delete
#endif /* VMS */


/* For Pyramid */
#ifdef pyr
#  define strrchr rindex
#  define ZMEM
#endif /* pyr */


/* File operations--use "b" for binary if allowed or fixed length 512 on VMS */
#ifdef VMS
#  define FOPR  "r","ctx=stm"
#  define FOPM  "r+","ctx=stm","rfm=fix","mrs=512"
#  define FOPW  "w","ctx=stm","rfm=fix","mrs=512"
#else /* !VMS */
#  ifdef MODERN
#    define FOPR "rb"
#    define FOPM "r+b"
#    define FOPW "wb"
#  else /* !MODERN */
#    define FOPR "r"
#    define FOPM "r+"
#    define FOPW "w"
#  endif /* ?MODERN */
#endif /* VMS */

/* Open the old zip file in exclusive mode if possible (to avoid adding
 * zip file to itself).
 */
#ifdef OS2
#  define FOPR_EX FOPM
#else
#  define FOPR_EX FOPR
#endif

/* Define this symbol if your target allows access to unaligned data.
 * This is not mandatory, just a speed optimization. The compressed
 * output is strictly identical.
 */
#if defined(MSDOS) || defined(i386) || defined(mc68020) || defined(vax)
#   define UNALIGNED_OK
#endif


/* Under MSDOS we may run out of memory when processing a large number
 * of files. Compile with MEDIUM_MEM to reduce the memory requirements or
 * with SMALL_MEM to use as little memory as possible.
 */
#ifdef SMALL_MEM
#   define CBSZ 2048 /* buffer size for copying files */
#   define ZBSZ 2048 /* buffer size for temporary zip file */
#else
# ifdef MEDIUM_MEM
#   define CBSZ 8192
#   define ZBSZ 8192
# else
#  if defined(OS2) && !defined(M_I86SM)
#   define CBSZ 0xE000
#   define ZBSZ 0x7F00
    /* The C library of some compilers does not allow a buffer size > 32K */
#  else
#   define CBSZ 16384
#   define ZBSZ 16384
#  endif
# endif
#endif

#ifdef ATARI_ST
#  define MSDOS
#  define MSVMS
#  ifndef O_BINARY
#    define O_BINARY 0
#  endif
#  ifndef S_IFMT
#    define S_IFMT        (S_IFCHR|S_IFREG|S_IFDIR)
#  endif

/* a whole bunch of functions needs Tos '\\' filnames
 * instead of '/',  the translation functions are in fileio.c:
 */
#  define unlink    st_unlink
#  define chmod     st_chmod
#  define mktemp    st_mktemp
#  define fopen     st_fopen
#  define open      st_open
#  define SSTAT     st_stat
#  define findfirst st_findfirst
#  define link      st_rename
#  define rmdir     st_rmdir

  int st_unlink    OF((char *));
  int st_chmod     OF((char *, int));
  char *st_mktemp  OF((char *));
  FILE *st_fopen   OF((char *, char *));
  int st_open      OF((char *, int));
  int st_stat      OF((char *, struct stat *));
  int st_findfirst OF((char *, struct ffblk *, int));
  int st_rename    OF((char *, char *));
  int st_rmdir     OF((char *));
#else
#  define SSTAT      stat
#endif
   
/* end of tailor.h */
