/*

 Copyright (C) 1990-1992 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included
 unmodified, that it is not sold for profit, and that this copyright notice
 is retained.

*/

/*
 *  fileio.c by Mark Adler.
 */

#include "zip.h"

#include <time.h>

#ifdef WIN32
#  include <sys/utime.h>
#  include <windows.h> /* for findfirst/findnext */
#endif

#ifdef MACOS
#  define EXDEV 1
#endif

#ifdef OSF
#  define EXDEV 18    /* avoid a bug in the DEC OSF/1 header files. */
#else
#  include <errno.h>
#endif

#ifdef MINIX
#  ifdef S_IWRITE
#    undef S_IWRITE
#  endif /* S_IWRITE */
#  define S_IWRITE S_IWUSR
#endif /* S_IWUSR */

#ifdef ATARI_ST
#  undef MSDOS
#endif

#ifdef MSDOS
#  include <io.h>
#  if (defined(__TURBOC__) || defined(__GO32__))
#    include <dir.h>
#  else /* !__TURBOC__ */
#    if !defined(__EMX__) && !defined(__WATCOMC__)
#      include <direct.h>
#    endif
#  endif /* ?__TURBOC__ */
#  define link rename
#  ifdef OS2
#    define MATCH shmatch
#  else /* !OS2 */
#    define MATCH dosmatch
#  endif /* ?OS2 */
#else /* !MSDOS */
   extern int errno;    /* error number from system functions */
#  ifdef VMS
#    define RMDIR
#    define link rename
#    include "VMSmunch.h"
#  endif /* VMS */
#  ifdef MACOS
#    define link rename
#    define mktemp tmpnam
#  endif
#  define MATCH shmatch
#endif /* ?MSDOS */

#ifdef ATARI_ST
#  define MSDOS 1
#endif

#ifdef UTS
#  define RMDIR
#endif /* UTS */


/* Extra malloc() space in names for cutpath() */
#ifdef VMS
#  define PAD 3         /* may have to change .FOO] to ]FOO.DIR */
#else /* !VMS */
#  define PAD 0
#endif /* ?VMS */


/* For now, assume DIRENT implies System V implies TERMIO */
#if defined(DIRENT) && !defined(MINIX) && !defined(TERMIO)
#  define TERMIO
#endif


#ifdef CRYPT
#  ifdef MSVMS
#    ifdef MSDOS
#      ifdef __EMX__
#        define getch() _read_kbd(0, 1, 0)
#      else
#        ifdef __GO32__
#          include <pc.h>
#          define getch() getkey()
#        else
#          include <conio.h>
#        endif
#      endif
#    else /* !MSDOS */
#      define getch() getc(stderr)
#      define echoff(f) echo(0)   /* for echo control */
#      define echon()   echo(1)
#      include <iodef.h>
#      include <ttdef.h>
#      if !defined(SS$_NORMAL)
#        define SS$_NORMAL 1   /* only thing we need from <ssdef.h> */
#      endif
#    endif /* ?MSDOS */
#  else /* !MSVMS */
#    ifdef TERMIO       /* Amdahl, Cray, all SysV? */
#      ifdef CONVEX
#        include <sys/termios.h>
#        include <sgtty.h>
#        define O_BINARY 0
#      else /* !CONVEX */
#        ifdef LINUX 
#          include <termios.h>
#        else /* !LINUX */
#          include <termio.h>
#        endif /* ?LINUX */
#        define sgttyb termio
#        define sg_flags c_lflag
         int ioctl OF((int, int, voidp *));
#      endif /* ?CONVEX */
#      define GTTY(f,s) ioctl(f,TCGETA,s)
#      define STTY(f,s) ioctl(f,TCSETAW,s)
#    else /* !TERMIO */
#      ifndef MINIX
#        include <sys/ioctl.h>
#      endif /* !MINIX */
#      include <sgtty.h>
       int gtty OF((int, struct sgttyb *));
       int stty OF((int, struct sgttyb *));
#      define GTTY gtty
#      define STTY stty
#    endif /* ?TERMIO */
     int isatty OF((int));
     char *ttyname OF((int));
     int open OF((char *, int, ...));
     int close OF((int));
     int read OF((int, voidp *, int));
#  endif /* ?MSVMS */
#endif /* ?CRYPT */

#ifdef VMS
#  include <descrip.h>
#  include <rms.h>
#endif

/* For directory access. (This is getting rather messy. Cleanup
 * scheduled for version 17.9.)
 */
#ifndef UTIL

#ifdef SYSV                     /* use readdir()  */
#  include <dirent.h>
#  define dstrm DIR
#  define direct dirent
#else

#ifdef DIRENT                   /* use getdents() */
#  if defined(MINIX) || defined(OSF)
#    include <dirent.h>
#  else /* !MINIX */
#    include <sys/dirent.h>
#  endif /* ?MINIX */
#  define direct dirent
#  ifdef MINIX
     int getdents OF((int, char *, unsigned));
#  else /* !MINIX */
     int getdents OF((int, char *, int));
#  endif /* ?MINIX */
#  define DBSZ 4096     /* This has to be bigger than a directory block */
   typedef struct {     /* directory stream buffer */
     int f;             /* file descriptor for the directory "file" */
     char *p;           /* pointer to next entry in buffer */
     char *q;           /* pointer after end of buffer contents */
     char b[DBSZ];              /* buffer */
   } dstrm;

#else /* !DIRENT */             /* use opendir(), etc. */
#  if defined(CONVEX) || defined(ultrix)
#    include <dirent.h>
#    ifdef direct
#      undef direct /* ultrix 4.2, at least if !__POSIX */
#    endif
#    define direct dirent
#  endif /* CONVEX || ultrix */
#  ifdef NDIR
#    include "ndir.h"           /* for HPUX */
#  else /* !NDIR */
#    ifdef MSDOS
#     ifdef OS2
#      include "os2zip.h"
#     else /* !OS2 */
#      ifndef ATARI_ST
#        include <dos.h>
#      endif
#      if (defined(__TURBOC__) || defined(__GO32__))
#        define FATTR           FA_HIDDEN+FA_SYSTEM+FA_DIREC
#        define FFIRST(n,d)     findfirst(n,(struct ffblk *)d,FATTR)
#        define FNEXT(d)        findnext((struct ffblk *)d)
#      else /* !__TURBOC__ */
#        define FATTR           _A_HIDDEN+_A_SYSTEM+_A_SUBDIR
#        define FFIRST(n,d)     _dos_findfirst(n,FATTR,(struct find_t *)d)
#        define FNEXT(d)        _dos_findnext((struct find_t *)d)
#      endif /* ?__TURBOC__ */
       typedef struct direct {
         char d_reserved[30];
         char d_name[13];
	 int d_first;
#ifdef WIN32
	 HANDLE d_hFindFile;
#endif
       } DIR;
#     endif /* ?OS2 */
#    else /* !MSDOS */
#      ifdef VMS
#        include <ssdef.h>
         typedef struct direct {
             int d_wild;                /* flag for wildcard vs. non-wild */
             struct FAB fab;
             struct NAM nam;
             char d_qualwildname[NAM$C_MAXRSS + 1];
             char d_name[NAM$C_MAXRSS + 1];
         } DIR;
#      else /* !VMS */
#        ifdef MACOS
           typedef struct direct {
             int d_wild;                /* flag for wildcard vs. non-wild */
             char *d_name;
          } DIR;
#        endif
#        ifdef M_XENIX
#          include <sys/ndir.h>
#        else /* !M_XENIX */
#          include <sys/dir.h>
#        endif /* ?M_XENIX */
#        ifdef NODIR                    /* for AT&T 3B1 */
#          define dirent direct
           typedef FILE DIR;
#          define dstrm DIR
#        endif /* NODIR */
#      endif /* ?VMS */
#    endif /* ?MSDOS */
#  endif /* ?NDIR */
#  define dstrm DIR
#  ifndef NODIR
     DIR *opendir OF((char *));
#  endif /* !NODIR */
#  ifndef CONVEX
     struct direct *readdir OF((DIR *));
#  endif /* !CONVEX */
#endif /* ?DIRENT */
#endif /* ?SYSV */
#endif /* !UTIL */


/* Library functions not in (most) header files */

#if defined(__IBMC__) || defined(__WATCOMC__)
#  define NO_MKTEMP
#endif
char *mktemp OF((char *));

#ifdef __GO32__
  char *strlwr OF((char *));
#else
  int link OF((char *, char *));
  int unlink OF((char *));
# if defined(MSDOS)
   int chmod OF((char *, int));
   /* For many targets, chmod is already defined by sys/stat.h, and second
    * parameter is an unsigned long.
    */
# endif
#endif


#ifndef UTIL    /* the companion #endif is a bit of ways down ... */

#ifndef __TURBOC__
   int utime OF((char *, time_t *));
#endif /* !__TURBOC__ */
#ifndef MSDOS
   int open OF((char *, int, ...));
   int close OF((int));
#  ifndef RMDIR
     int rmdir OF((char *));
#  endif /* !RMDIR */
#endif /* !MSDOS */


/* Local globals (kinda like "military intelligence" or "broadcast quality") */
local int exflag = 0;           /* Exclude flag */

#ifdef VMS
  typedef int statime;
#else /* !VMS */
  typedef time_t statime;
#endif /* ?VMS */

/* Local functions */
#ifdef PROTO
#  ifdef VMS
     local void vms_wild(char *, dstrm *);
#  endif /* VMS */
#  ifdef DIRENT
     local dstrm *opend(char *);
     local void closed(dstrm *);
#  endif /* DIRENT */
   local char *readd(dstrm *);
   local int fqcmp(voidp *, voidp *);
   local int fqcmpz(voidp *, voidp *);
   local char *last(char *);
   local char *msname(char *);
#  ifdef VMS
     local char *strlower(char *);
     local char *strupper(char *);
#  endif /* VMS */
   local char *ex2in(char *, int *);
   local int newname(char *);
   local void inctime(struct tm *);
   local ulg unix2dostime(statime *);
#  if !defined(__TURBOC__) && !defined(OS2) && !defined(__GO32__)
     local int cmptime(struct tm *, struct tm *);
     local time_t invlocal(struct tm *);
#  endif /* !__TURBOC__ */
#endif /* PROTO */


#if defined(MSDOS) && !defined(OS2)
dstrm *opendir(n)
char *n;                /* directory to open */
/* Start searching for files in the MSDOS directory n */
{
  dstrm *d;             /* malloc'd return value */
  char *p;              /* malloc'd temporary string */

  if ((d = (dstrm *)malloc(sizeof(dstrm))) == NULL ||
      (p = malloc(strlen(n) + 5)) == NULL)
    return NULL;
  strcat(strcpy(p, n), "/*.*");
#ifdef WIN32
  {
  WIN32_FIND_DATA fd;
  DWORD dwAttr;
  BOOL bAttr;

  if ((HANDLE)0xFFFFFFFF == (d->d_hFindFile = FindFirstFile(p, &fd)))
    {
    free((voidp *)p);
    return NULL;
    }
  else
    strcpy(d->d_name, fd.cFileName);
  if (-1 != (dwAttr = GetFileAttributes(fd.cFileName)))
    {
    bAttr = FALSE;
    if (FILE_ATTRIBUTE_SYSTEM == (dwAttr & FILE_ATTRIBUTE_SYSTEM))
	bAttr = TRUE;
    if (FILE_ATTRIBUTE_HIDDEN == (dwAttr & FILE_ATTRIBUTE_HIDDEN))
	bAttr = TRUE;
    if (FILE_ATTRIBUTE_DIRECTORY == (dwAttr & FILE_ATTRIBUTE_DIRECTORY))
	bAttr = TRUE;
    if (!bAttr)
	{
	free ((voidp *)p);
	free ((void *) d);
	return NULL;
	}
    }

  }
#else
  if (FFIRST(p, d))
  {
    free((voidp *)p);
    return NULL;
  }
  free((voidp *)p);
#endif
  d->d_first = 1;
  return d;
}

struct direct *readdir(d)
dstrm *d;               /* directory stream to read from */
/* Return pointer to first or next directory entry, or NULL if end. */
{
  if (d->d_first)
    d->d_first = 0;
  else
#ifdef WIN32
    {
    WIN32_FIND_DATA fd;

    if (!FindNextFile(d->d_hFindFile, &fd))
	return NULL;
    else
	strcpy(d->d_name, fd.cFileName);
    }
#else /* !WIN32 */
    if (FNEXT(d))
      return NULL;
#endif
  return (struct direct *)d;
}
#  define closedir free

#endif /* MSDOS && !OS2 */


#ifdef VMS

/*---------------------------------------------------------------------------

    _vms_findfirst() and _vms_findnext(), based on public-domain DECUS C
    fwild() and fnext() routines (originally written by Martin Minow, poss-
    ibly modified by Jerry Leichter for bintnxvms.c), were written by Greg
    Roelofs and are still in the public domain.  Routines approximate the
    behavior of MS-DOS (MSC and Turbo C) findfirst and findnext functions.

  ---------------------------------------------------------------------------*/
local void vms_wild(p, d)
char *p;
dstrm *d;
{
  /*
   * Do wildcard setup
   */
  /* set up the FAB and NAM blocks. */
  d->fab = cc$rms_fab;             /* initialize fab */
  d->nam = cc$rms_nam;             /* initialize nam */

  d->fab.fab$l_nam = &d->nam;           /* fab -> nam */
  d->fab.fab$l_fna = p;                 /* argument wild name */
  d->fab.fab$b_fns = strlen(p);         /* length */

  d->nam.nam$l_esa = d->d_qualwildname; /* qualified wild name */
  d->nam.nam$b_ess = NAM$C_MAXRSS;      /* max length */
  d->nam.nam$l_rsa = d->d_name;         /* matching file name */
  d->nam.nam$b_rss = NAM$C_MAXRSS;      /* max length */

  /* parse the file name */
  if (sys$parse(&d->fab) != RMS$_NORMAL)
    return;
  /* Does this replace d->fab.fab$l_fna with a new string in its own space?
     I sure hope so, since p is free'ed before this routine returns. */

  /* have qualified wild name (i.e., disk:[dir.subdir]*.*); null-terminate
   * and set wild-flag */
  d->d_qualwildname[d->nam.nam$b_esl] = '\0';
  d->d_wild = (d->nam.nam$l_fnb & NAM$M_WILDCARD)? 1 : 0;   /* not used... */
#ifdef DEBUG
  printf("  incoming wildname:  %s\n", p);
  printf("  qualified wildname:  %s\n", d->d_qualwildname);
#endif /* DEBUG */
}

dstrm *opendir(n)
char *n;                /* directory to open */
/* Start searching for files in the VMS directory n */
{
  char *c;              /* scans VMS path */
  dstrm *d;             /* malloc'd return value */
  int m;                /* length of name */
  char *p;              /* malloc'd temporary string */

  if ((d = (dstrm *)malloc(sizeof(dstrm))) == NULL ||
      (p = malloc((m = strlen(n)) + 4)) == NULL)
    return NULL;
  /* Directory may be in form "[DIR.SUB1.SUB2]" or "[DIR.SUB1]SUB2.DIR;1".
     If latter, convert to former. */
  if (m > 0  &&  *(c = strcpy(p,n)+m-1) != ']')
  {
    while (--c > p  &&  *c != ';')
      ;
    if (c-p < 5  ||  strncmp(c-4, ".DIR", 4))
    {
      free((voidp *)d);  free((voidp *)p);
      return NULL;
    }
    c -= 3;
    *c-- = '\0';        /* terminate at "DIR;#" */
    *c = ']';           /* "." --> "]" */
    while (c > p  &&  *--c != ']')
      ;
    *c = '.';           /* "]" --> "." */
  }
  strcat(p, "*.*");
  vms_wild(p, d);       /* set up wildcard */
  free((voidp *)p);
  return d;
}

struct direct *readdir(d)
dstrm *d;               /* directory stream to read from */
/* Return pointer to first or next directory entry, or NULL if end. */
{
  int r;                /* return code */

  do {
    d->fab.fab$w_ifi = 0;       /* internal file index:  what does this do? */

    /* get next match to possible wildcard */
    if ((r = sys$search(&d->fab)) == RMS$_NORMAL)
    {
        d->d_name[d->nam.nam$b_rsl] = '\0';   /* null terminate */
        return (struct direct *)d;   /* OK */
    }
  } while (r == RMS$_PRV);
  return NULL;
}
#  define closedir free
#endif /* VMS */


#ifdef NODIR                    /* for AT&T 3B1 */
/*
**  Apparently originally by Rich Salz.
**  Cleaned up and modified by James W. Birdsall.
*/

#  define opendir(path) fopen(path, "r")
 
struct direct *readdir(dirp)
DIR *dirp;
{
  static struct direct entry;

  if (dirp == NULL) 
    return NULL;
  for (;;)
    if (fread (&entry, sizeof (struct direct), 1, dirp) == 0) 
      return NULL;
    else if (entry.d_ino) 
      return (&entry);
} /* end of readdir() */

#  define closedir(dirp) fclose(dirp)
#endif /* NODIR */


#ifdef DIRENT
local dstrm *opend(n)
char *n;                /* directory name to open */
/* Open the directory *n, returning a pointer to an allocated dstrm, or
   NULL if error. */
{
  dstrm *d;             /* pointer to malloc'ed directory stream */

  if ((d = (dstrm *)malloc(sizeof(dstrm))) == NULL)
    return NULL;
  if ((d->f = open(n, 0, 0)) < 0)               /* open directory */
    return NULL;
  d->p = d->q = d->b;                           /* buffer is empty */
  return d;
}
#else /* !DIRENT */
#  define opend opendir                         /* just use opendir() */
#endif /* ?DIRENT */


local char *readd(d)
dstrm *d;               /* directory stream to read from */
/* Return a pointer to the next name in the directory stream d, or NULL if
   no more entries or an error occurs. */
{
  struct direct *e;     /* directory entry read */

#ifdef DIRENT
  int n;                /* number of entries read by getdents */

  if (d->p >= d->q)                             /* if empty, fill buffer */
    if ((n = getdents(d->f, d->b, DBSZ)) <= 0)
      return NULL;
    else
      d->q = n + (d->p = d->b);
  e = (struct direct *)(d->p);                  /* point to entry */
  d->p += ((struct direct *)(d->p))->d_reclen;  /* advance */
  return e->d_name;                             /* return name */
#else /* !DIRENT */
  return (e = readdir(d)) == NULL ? (char *)NULL : e->d_name;
#endif /* ?DIRENT */
}


#ifdef DIRENT
local void closed(d)
dstrm *d;               /* directory stream to close */
/* Close the directory stream */
{
  close(d->f);
  free((voidp *)d);
}
#else /* !DIRENT */
#  define closed closedir
#endif /* ?DIRENT */


#ifdef MSDOS

int wild(w)
char *w;                /* path/pattern to match */
/* If not in exclude mode, expand the pattern based on the contents of the
   file system.  Return an error code in the ZE_ class. */
{
  char *a;              /* alloc'ed space for name */
  dstrm *d;             /* stream for reading directory */
  char *e;              /* name found in directory */
  int f;                /* true if there was a match */
  char *n;              /* constructed name from directory */
  char *p;              /* path */
  char *q;              /* name */
  int r;                /* temporary variable */
  char v[5];            /* space for device current directory */

  /* Allocate and copy pattern */
  if ((p = a = malloc(strlen(w) + 1)) == NULL)
    return ZE_MEM;
  strcpy(p, w);

  /* Normalize pattern to upper case, path delimiter as '/'. */
#if defined(FORCE_UPPER)
#ifndef OS2
  strupr(p);                            /* convert to upper case */
#else /* OS2 */
  if (IsFileSystemFAT(p)) strupr(p);
#endif /* !OS2 */
#endif
  for (q = p; *q; q++)                  /* use / consistently */
    if (*q == '\\')
      *q = '/';

  /* If excluding, don't bother with file system */
  if (exflag)
  {
    r = procname(p);
    free((voidp *)a);
    return r;
  }

  /* Only name can have special matching characters */
  if ((q = isshexp(p)) != NULL &&
      (strrchr(q, '/') != NULL || strrchr(q, ':') != NULL))
  {
    free((voidp *)a);
    return ZE_PARMS;
  }

  /* Separate path and name into p and q */
  if ((q = strrchr(p, '/')) != NULL && (q == p || q[-1] != ':'))
  {
    *q++ = 0;                           /* path/name -> path, name */
    if (*p == 0)                        /* path is just / */
      p = strcpy(v, "/.");
  }
  else if ((q = strrchr(p, ':')) != NULL)
  {                                     /* has device and no or root path */
    *q++ = 0;
    p = strcat(strcpy(v, p), ":");      /* copy device as path */
    if (*q == '/')                      /* -> device:/., name */
    {
      strcat(p, "/");
      q++;
    }
    strcat(p, ".");
  }
  else                                  /* no path or device */
  {
    q = p;
    p = strcpy(v, ".");
  }

  /* Search that level for matching names */
  if ((d = opend(p)) == NULL)
  {
    free((voidp *)a);
    return ZE_MISS;
  }
  if ((r = strlen(p)) > 1 &&
      (strcmp(p + r - 2, ":.") == 0 || strcmp(p + r - 2, "/.") == 0))
    *(p + r - 1) = 0;
  f = 0;
  while ((e = readd(d)) != NULL)
    if (strcmp(e, ".") && strcmp(e, "..") && MATCH(q, e))
    {
      f = 1;
      if (strcmp(p, ".") == 0)                  /* path is . */
        procname(e);                            /* name is name */
      else
      {
        if ((n = malloc(strlen(p) + strlen(e) + 2)) == NULL)
        {
          free((voidp *)a);
          return ZE_MEM;
        }
        n = strcpy(n, p);
        if (n[r = strlen(n) - 1] != '/' && n[r] != ':')
          strcat(n, "/");
        r = procname(strcat(n, e));             /* name is path/name */
        free((voidp *)n);
        if (r)
        {
          free((voidp *)a);
          return r;
        }
      }
    }
  closed(d);

  /* Done */
  free((voidp *)a);
  return f ? ZE_OK : ZE_MISS;
}

#endif /* MSDOS */


#ifdef VMS
int wild(p)
char *p;                /* path/pattern to match */
/* Expand the pattern based on the contents of the file system.  Return an
   error code in the ZE_ class. */
{
  dstrm *d;             /* stream for reading directory */
  char *e;              /* name found in directory */
  int f;                /* true if there was a match */

  /* Search given pattern for matching names */
  if ((d = (dstrm *)malloc(sizeof(dstrm))) == NULL)
    return ZE_MEM;
  vms_wild(p, d);       /* pattern may be more than just directory name */
  f = 0;
  while ((e = readd(d)) != NULL)        /* "dosmatch" is already built in */
    if (procname(e) == ZE_OK)
      f = 1;
  closed(d);

  /* Done */
  return f ? ZE_OK : ZE_MISS;
}
#endif /* VMS */


char *getnam(n)
char *n;                /* where to put name (must have >=FNMAX+1 bytes) */
/* Read a space, \n, \r, or \t delimited name from stdin into n, and return
   n.  If EOF, then return NULL.  Also, if the name read is too big, return
   NULL. */
{
  int c;                /* last character read */
  char *p;              /* pointer into name area */

  p = n;
  while ((c = getchar()) == ' ' || c == '\n' || c == '\r' || c == '\t')
    ;
  if (c == EOF)
    return NULL;
  do {
    if (p - n >= FNMAX)
      return NULL;
    *p++ = (char)c;
    c = getchar();
  } while (c != EOF && c != ' ' && c != '\n' && c != '\r' && c != '\t');
  *p = 0;
  return n;
}


struct flist far *fexpel(f)
struct flist far *f;    /* entry to delete */
/* Delete the entry *f in the doubly-linked found list.  Return pointer to
   next entry to allow stepping through list. */
{
  struct flist far *t;  /* temporary variable */

  t = f->nxt;
  *(f->lst) = t;                        /* point last to next, */
  if (t != NULL)
    t->lst = f->lst;                    /* and next to last */
  if (f->name != NULL)                  /* free memory used */
    free((voidp *)(f->name));
  if (f->zname != NULL)
    free((voidp *)(f->zname));
  farfree((voidp far *)f);
  fcount--;                             /* decrement count */
  return t;                             /* return pointer to next */
}


local int fqcmp(a, b)
voidp *a, *b;           /* pointers to pointers to found entries */
/* Used by qsort() to compare entries in the found list by name. */
{
  return strcmp((*(struct flist far **)a)->name,
                (*(struct flist far **)b)->name);
}


local int fqcmpz(a, b)
voidp *a, *b;           /* pointers to pointers to found entries */
/* Used by qsort() to compare entries in the found list by zname. */
{
  return strcmp((*(struct flist far **)a)->zname,
                (*(struct flist far **)b)->zname);
}


local char *last(p)
char *p;                /* sequence of / delimited path components */
/* Return a pointer to the start of the last path component. */
{
  char *t;              /* temporary variable */

#ifdef VMS
  if ((t = strrchr(p, ']')) != NULL)
#else /* !VMS */
  if ((t = strrchr(p, '/')) != NULL)
#endif /* ?VMS */
    return t + 1;
  else
    return p;
}


local char *msname(n)
char *n;
/* Reduce all path components to MSDOS upper case 8.3 style names.  Probably
   should also check for invalid characters, but I don't know which ones
   those are. */
{
  int c;                /* current character */
  int f;                /* characters in current component */
  char *p;              /* source pointer */
  char *q;              /* destination pointer */

  p = q = n;
  f = 0;
  while ((c = *p++) != 0)
    if (c == '/')
    {
      *q++ = (char)c;
      f = 0;                            /* new component */
    }
    else if (c == '.')
      if (f < 9)
      {
        *q++ = (char)c;
        f = 9;                          /* now in file type */
      }
      else
        f = 12;                         /* now just excess characters */
    else
      if (f < 12 && f != 8)
      {
        *q++ = (char)(to_up(c));
        f++;                            /* do until end of name or type */
      }
  *q = 0;
  return n;
}


#ifdef VMS
local char *strlower(s)
char *s;                /* string to convert */
/* Convert all uppercase letters to lowercase in string s */
{
  char *p;              /* scans string */

  for (p = s; *p; p++)
    if (*p >= 'A' && *p <= 'Z')
      *p += 'a' - 'A';
  return s;
}

local char *strupper(s)
char *s;                /* string to convert */
/* Convert all lowercase letters to uppercase in string s */
{
  char *p;              /* scans string */

  for (p = s; *p; p++)
    if (*p >= 'a' && *p <= 'z')
      *p -= 'a' - 'A';
  return s;
}
#endif /* VMS */

local char *ex2in(x, pdosflag)
char *x;                /* external file name */
int *pdosflag;          /* output: force MSDOS file attributes? */
/* Convert the external file name to a zip file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  char *n;              /* internal file name (malloc'ed) */
  char *t;              /* shortened name */
  int dosflag;

#ifdef OS2
  dosflag = dosify || IsFileSystemFAT(x);
  if ( !dosify && use_longname_ea && (t = GetLongPathEA(x)) != NULL )
  {
    x = t;
    dosflag = 0;
  }
#else
# ifdef MSDOS
  dosflag = 1;
# else /* !MSDOS */
  dosflag = dosify; /* default for non-DOS and non-OS/2 */
# endif /* MSDOS */
#endif /* OS2 */

  /* Find starting point in name before doing malloc */
#ifdef MSDOS                            /* msdos */
  t = *x && *(x + 1) == ':' ? x + 2 : x;
  while (*t == '/' || *t == '\\')
    t++;
#else /* !MSDOS */
#  ifdef VMS                            /* vms */
  t = x;
  if ((n = strrchr(t, ':')) != NULL)
    t = n + 1;
  if (*t == '[' && (n = strrchr(t, ']')) != NULL)
    if ((x = strchr(t, '.')) != NULL && x < n)
      t = x + 1;
    else
      t = n + 1;
#  else /* !VMS */                      /* unix */
  for (t = x; *t == '/'; t++)
    ;
#  endif /* ?VMS */
#endif /* ?MSDOS */

  /* Make changes, if any, to the copied name (leave original intact) */
#ifdef MSDOS
  for (n = t; *n; n++)
    if (*n == '\\')
      *n = '/';
#endif /* MSDOS */

  if (!pathput)
    t = last(t);

  /* Malloc space for internal name and copy it */
  if ((n = malloc(strlen(t) + 1)) == NULL)
    return NULL;
  strcpy(n, t);

#ifdef VMS
  if ((t = strrchr(n, ']')) != NULL)
  {
    *t = '/';
    while (--t > n)
      if (*t == '.')
        *t = '/';
  }

  /* Fix from Greg Roelofs: */
  /* Get current working directory and strip from n (t now = n) */
  {
    char cwd[256], *p, *q;
    int c;

    if (getcwd(cwd, 256) && ((p = strchr(cwd, '.')) != NULL))
    {
      ++p;
      if ((q = strrchr(p, ']')) != NULL)
      {
        *q = '/';
        while (--q > p)
          if (*q == '.')
            *q = '/';
        /* strip bogus path parts from n */
        if (strncmp(n, p, (c=strlen(p))) == 0)
        {
          q = n + c;
          while (*t++ = *q++)
            ;
        }
      }
    }
  }
  strlower(n);
  if (!vmsver)
    if ((t = strrchr(n, ';')) != NULL)
      *t = 0;

  if( (t = strrchr(n, '.')) != NULL )
  {
    if( t[1] == 0 )               /* "filename." -> "filename" */
      *t = 0;
    else if( t[1] == ';' )        /* "filename.;vvv" -> "filename;vvv" */
    {
      char *f;
      for( f=t+1; *t++ = *f++; )
        ;
    }
  }
#endif /* VMS */
  if (dosify)
    msname(n);
#if defined(MSDOS) && !defined(OS2) && !defined(FORCE_UPPER)
  else
    strlwr(n);
#endif
  /* Returned malloc'ed name */
  if (pdosflag) 
    *pdosflag = dosflag;
  return n;
}


char *in2ex(n)
char *n;                /* internal file name */
/* Convert the zip file name to an external file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  char *x;              /* external file name */
#ifdef VMS
  char *t;              /* scans name */

  if ((t = strrchr(n, '/')) == NULL)
#endif /* VMS */
  {
    if ((x = malloc(strlen(n) + 1 + PAD)) == NULL)
      return NULL;
    strcpy(x, n);
  }
#ifdef VMS
  else
  {
    if ((x = malloc(strlen(n) + 3 + PAD)) == NULL)
      return NULL;
    strcpy(x, "[.");
    strcpy(x + 2, n);
    *(t = x + 2 + (t - n)) = ']';
    while (--t > x)
      if (*t == '/')
        *t = '.';
  }
  strupper(x);
#endif /* VMS */
#ifdef OS2
  if ( !IsFileNameValid(x) )
    ChangeNameForFAT(x);
#endif /* !OS2 */
#if defined(FORCE_UPPER) && defined(MSDOS)
  /* Don't convert to upper case, causes wrong warnings. Keep the
   * name as it was before in the old zip file.
   */
  strupr(x);
#endif
  return x;
}


int exclude()
/* Change from including to excluding names when procname() called.  Return
   an error code in the ZE_ class. */
{
  struct flist far *f;          /* steps through found linked list */
  extent j;                     /* index for s */
  struct flist far **s;         /* sorted table */

  /* sort found list, remove duplicates */
  if (fcount)
  {
    if ((s = (struct flist far **)malloc(
         fcount * sizeof(struct flist far *))) == NULL)
      return ZE_MEM;
    for (j = 0, f = found; f != NULL; f = f->nxt)
      s[j++] = f;
    qsort((char *)s, fcount, sizeof(struct flist far *), fqcmp);
    for (j = fcount - 1; j > 0; j--)
      if (strcmp(s[j - 1]->name, s[j]->name) == 0)
        fexpel(s[j]);           /* fexpel() changes fcount */
    qsort((char *)s, fcount, sizeof(struct flist far *), fqcmpz);
    for (j = 1; j < fcount; j++)
      if (strcmp(s[j - 1]->zname, s[j]->zname) == 0)
      {
        warn("name in zip file repeated: ", s[j]->zname);
        warn("  first full name: ", s[j - 1]->name);
        warn(" second full name: ", s[j]->name);
        return ZE_PARMS;
      }
    free((voidp *)s);
  }
  exflag = 1;
  return ZE_OK;
}


local int newname(n)
char *n;                /* name to add (or exclude) */
/* Add (or exclude) a name that is not in the zip file.  Return an error
   code in the ZE_ class. */
{
  char *m;
  struct flist far *f;  /* where in found, or new found entry */
  struct zlist far *z;  /* where in zfiles (if found) */
  int dosflag;

  /* Search for name in zip file.  If there, mark it, else add to
     list of new names to do (or remove from that list). */
  if ((m = ex2in(n, &dosflag)) == NULL)
    return ZE_MEM;
  if ((z = zsearch(m)) != NULL)
    if (exflag)
    {
      z->mark = 0;
      free((voidp *)m);
      if (verbose)
        printf("zip diagnostic: excluding %s\n", z->name);
    }
    else
    {
      free((voidp *)(z->name));
      free((voidp *)(z->zname));
      if ((z->name = malloc(strlen(n) + 1 + PAD)) == NULL)
        return ZE_MEM;
      strcpy(z->name, n);
      z->zname = m;
      z->mark = 1;
      z->dosflag = dosflag;
      if (verbose)
        printf("zip diagnostic: including %s\n", z->name);
    }
  else
    if (exflag)
    {
      /* search list for name--if there, remove it */
      for (f = found; f != NULL; f = f->nxt)
        if (namecmp(n, f->name) == 0)
        {
          fexpel(f);
          break;
        }
      free((voidp *)m);
    }
    else
    {
      /* allocate space and add to list */
      if ((f = (struct flist far *)farmalloc(sizeof(struct flist))) == NULL ||
          (f->name = malloc(strlen(n) + 1 + PAD)) == NULL)
      {
        if (f != NULL)
          farfree((voidp far *)f);
        return ZE_MEM;
      }
      strcpy(f->name, n);
      f->zname = m;
      f->dosflag = dosflag;
      *fnxt = f;
      f->lst = fnxt;
      f->nxt = NULL;
      fnxt = &f->nxt;
      fcount++;
    }
  return ZE_OK;
}


int procname(n)
char *n;                /* name to process */
/* Process a name or sh expression to operate on (or exclude).  Return
   an error code in the ZE_ class. */
{
  char *a;              /* path and name for recursion */
  dstrm *d;             /* directory stream from opend() */
  char *e;              /* pointer to name from readd() */
  struct flist far *f;  /* steps through found list */
  int m;                /* matched flag */
  char *p;              /* path for recursion */
  struct stat s;        /* result of stat() */
  struct zlist far *z;  /* steps through zfiles list */

  if (strcmp(n, "-") == 0)   /* if compressing stdin */
    return newname(n);
  else if (
#ifdef S_IFLNK          /* if symbolic links exist ... */
      linkput ? lstat(n, &s) :
#endif /* S_IFLNK */
      SSTAT(n, &s)
#if defined(__TURBOC__) || defined(VMS)
       /* Borland and VMS C bug: stat() succeeds on wild card names! */
      || isshexp(n)
#endif
     )
  {
    /* Not a file or directory--search for shell expression in zip file */
    p = ex2in(n, NULL);         /* shouldn't affect matching chars */
    m = 1;
    for (z = zfiles; z != NULL; z = z->nxt)
      if (MATCH(p, z->zname))
      {
        z->mark = !exflag;
        if (verbose)
          printf("zip diagnostic: %scluding %s\n",
                 exflag ? "ex" : "in", z->name);
        m = 0;
      }
    /* If excluding, also search for expression in found list */
    if (exflag)
    {
      for (f = found; f != NULL;)
        if (MATCH(p, f->zname))
        {
          f = fexpel(f);
          m = 0;
        }
        else
          f = f->nxt;
    }
    free((voidp *)p);
    if (m)
      return ZE_MISS;           /* no match */
  }
  else
  {
    /* Live name--use if file, recurse if directory */
#if defined(FORCE_UPPER) && defined(MSDOS)
# ifndef OS2
    strupr(n);                  /* convert to upper case */
# else /* OS2 */
    if (IsFileSystemFAT(n)) strupr(n);
# endif /* !OS2 */
#endif

#ifdef MSDOS
    for (p = n; *p; p++)          /* use / consistently */
      if (*p == '\\')
        *p = '/';
#endif /* MSDOS */
    if ((s.st_mode & S_IFDIR) == 0)
    {
      /* add or remove name of file */
        if ((m = newname(n)) != ZE_OK)
          return m;
    } else {
        /* recurse into directory */
        if (recurse && (d = opend(n)) != NULL)
        {
#ifdef VMS
          while ((e = readd(d)) != NULL)
            if ((m = procname(e)) != ZE_OK)     /* recurse on name */
            {
              /* want to just set warning error and continue */
              closed(d);
              return m;
            }
#else /* !VMS */
          if ((p = malloc(strlen(n)+2)) == NULL)
            return ZE_MEM;
          if (strcmp(n, ".") == 0)
            *p = 0; /* avoid "./" prefix and do not create zip entry */
          else
          {
            strcpy(p, n);
            a = p + strlen(p);
            if (a[-1] != '/')
              strcpy(a, "/");
            if ((m = newname(p)) != ZE_OK)
              return m;
          }
          while ((e = readd(d)) != NULL)
            if (strcmp(e, ".") && strcmp(e, ".."))
            {
              if ((a = malloc(strlen(p) + strlen(e) + 1)) == NULL)
              {
                free((voidp *)p);
                closed(d);
                return ZE_MEM;
              }
              strcat(strcpy(a, p), e);
              if ((m = procname(a)) != ZE_OK)   /* recurse on name */
              {
                free((voidp *)a);  free((voidp *)p);
                closed(d);
                return m;
              }
              free((voidp *)a);
            }
          free((voidp *)p);
#endif /* ?VMS */
          closed(d);
        }
      }
    }
  return ZE_OK;
}


#if !defined(CRAY) && !defined(__TURBOC__) && !defined(OS2) /* and ... */
#if !defined( __GO32__)

local int cmptime(p, q)
struct tm *p, *q;       /* times to compare */
/* Return negative if time p is before time q, positive if after, and
   zero if the same */
{
  int r;                /* temporary variable */

  if (p == NULL)
    return -1;
  else if ((r = p->tm_year - q->tm_year) != 0)
    return r;
  else if ((r = p->tm_mon - q->tm_mon) != 0)
    return r;
  else if ((r = p->tm_mday - q->tm_mday) != 0)
    return r;
  else if ((r = p->tm_hour - q->tm_hour) != 0)
    return r;
  else if ((r = p->tm_min - q->tm_min) != 0)
    return r;
  else
    return p->tm_sec - q->tm_sec;
}


local time_t invlocal(t)
struct tm *t;           /* time to convert */
/* Find inverse of localtime() using bisection.  This routine assumes that
   time_t is an integer type, either signed or unsigned.  The expectation
   is that sometime before the year 2038, time_t will be made a 64-bit
   integer, and this routine will still work. */
{
  time_t i;             /* midpoint of current root range */
  time_t l;             /* lower end of root range */
  time_t u;             /* upper end of root range */

  /* Bracket the root [0,largest time_t].  Note: if time_t is a 32-bit signed
     integer, then the upper bound is GMT 1/19/2038 03:14:07, after which all
     the Unix systems in the world come to a grinding halt.  Either that, or
     all those systems will suddenly find themselves transported to December
     of 1901 ... */
  l = 0;
  u = 1;
  while (u < (u << 1))
    u = (u << 1) + 1;

  /* Find the root */
  while (u - l > 1)
  {
    i = l + ((u - l) >> 1);
    if (cmptime(localtime(&i), t) <= 0)
      l = i;
    else
      u = i;
  }
  return l;
}
#endif
#endif


void stamp(f, d)
char *f;                /* name of file to change */
ulg d;                  /* dos-style time to change it to */
/* Set last updated and accessed time of file f to the DOS time d. */
{
#if defined(MACOS)
  warn("timestamp not implemented yet", "");
#else
#ifdef __TURBOC__
  int h;                /* file handle */

  if ((h = open(f, 0)) != -1)
  {
#ifdef ATARI_ST
    d = ( d >> 16 ) | ( d << 16 );
#endif
    setftime(h, (struct ftime *)&d);
    close(h);
  }
#else /* !__TURBOC__ */
#ifdef VMS
  int tm_sec, tm_min, tm_hour, tm_mday, tm_mon, tm_year;
  char timbuf[24];
  static char *month[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                          "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
  struct VMStimbuf {
      char *actime;           /* VMS revision date, ASCII format */
      char *modtime;          /* VMS creation date, ASCII format */
  } ascii_times = {timbuf, timbuf};

  /* Convert DOS time to ASCII format for VMSmunch */
  tm_sec = (int)(d << 1) & 0x3e;
  tm_min = (int)(d >> 5) & 0x3f;
  tm_hour = (int)(d >> 11) & 0x1f;
  tm_mday = (int)(d >> 16) & 0x1f;
  tm_mon = ((int)(d >> 21) & 0xf) - 1;
  tm_year = ((int)(d >> 25) & 0x7f) + 1980;
  sprintf(timbuf, "%02d-%3s-%04d %02d:%02d:%02d.00", tm_mday, month[tm_mon],
    tm_year, tm_hour, tm_min, tm_sec);

  /* Set updated and accessed times of f */
  if (VMSmunch(f, SET_TIMES, &ascii_times) != RMS$_NMF)
    warn("can't set zipfile time: ", f);

#else /* !VMS */
#ifdef OS2
  SetFileTime(f, d);
#else /* !OS2 */
  struct tm t;          /* argument for mktime() or invlocal() */
  time_t u[2];          /* argument for utime() */
#ifndef __GO32__
  extern time_t mktime OF((struct tm *));
#endif

  /* Convert DOS time to time_t format in u[0] and u[1] */
  t.tm_sec = (int)(d << 1) & 0x3e;
  t.tm_min = (int)(d >> 5) & 0x3f;
  t.tm_hour = (int)(d >> 11) & 0x1f;
  t.tm_mday = (int)(d >> 16) & 0x1f;
  t.tm_mon = ((int)(d >> 21) & 0xf) - 1;
  t.tm_year = ((int)(d >> 25) & 0x7f) + 80;
#if defined(MSDOS) || defined(OS2) || defined(CRAY)
  /* mktime() is more reliable than invlocal() because the time range is
   * wider on MSDOS than on Unix; required for Cray because invlocal assumes
   * 32-bit ints
   */
  u[0] = u[1] = mktime(&t);
#else
  u[0] = u[1] = invlocal(&t);
#endif

  /* Set updated and accessed times of f */
  utime(f, u);
#endif /* ?OS2 */
#endif /* ?VMS */
#endif /* ?__TURBOC__ */
#endif /* ?MACOS */
}


local void inctime(s)
struct tm *s;           /* time to increment in place */
/* Increment the time structure *s by one second, return the result in
   place. */
{
  int y;                /* temporary variable */

  /* days in each month, except for February */
  static int days[] = {31,0,31,30,31,30,31,31,30,31,30,31};

  /* Set days in February from year (1900 is a leap year, 2000 is not) */
  y = s->tm_year + 1900;
  days[1] = y % 4 == 0 && (y % 100 != 0 || y % 400 == 0) ? 29 : 28;

  /* Increment time with carry */
  if (s->tm_sec != 59)
    s->tm_sec++;
  else if (s->tm_sec = 0, s->tm_min != 59)
    s->tm_min++;
  else if (s->tm_min = 0, s->tm_hour != 23)
    s->tm_hour++;
  else if (s->tm_hour = 0, s->tm_mday != days[s->tm_mon])
    s->tm_mday++;
  else if (s->tm_mday = 1, s->tm_mon != 11)
    s->tm_mon++;
  else
  {
    s->tm_mon = 0;
    s->tm_year++;
  }
}


ulg dostime(y, n, d, h, m, s)
int y;                  /* year */
int n;                  /* month */
int d;                  /* day */
int h;                  /* hour */
int m;                  /* minute */
int s;                  /* second */
/* Convert the date y/n/d and time h:m:s to a four byte DOS date and
   time (date in high two bytes, time in low two bytes allowing magnitude
   comparison). */
{
  return y < 1980 ? dostime(1980, 1, 1, 0, 0, 0) :
        (((ulg)y - 1980) << 25) | ((ulg)n << 21) | ((ulg)d << 16) |
        ((ulg)h << 11) | ((ulg)m << 5) | ((ulg)s >> 1);
}


local ulg unix2dostime(t)
statime *t;             /* unix time to convert */
/* Return the Unix time t in DOS format, rounded up to the next two
   second boundary. */
{
  struct tm *s;         /* result of localtime() */

  s = localtime(t);             /* Use local time since MSDOS does */
  inctime(s);                   /* Add one second to round up */
  return dostime(s->tm_year + 1900, s->tm_mon + 1, s->tm_mday,
                 s->tm_hour, s->tm_min, s->tm_sec);
}


ulg filetime(f, a, n)
char *f;                /* name of file to get info on */
ulg *a;                 /* return value: file attributes */
long *n;                /* return value: file size */
/* If file *f does not exist, return 0.  Else, return the file's last
   modified date and time as an MSDOS date and time.  The date and
   time is returned in a long with the date most significant to allow
   unsigned integer comparison of absolute times.  Also, if a is not
   a NULL pointer, store the file attributes there, with the high two
   bytes being the Unix attributes, and the low byte being a mapping
   of that to DOS attributes.  If n is not NULL, store the file size
   there.
   If f is "-", use standard input as the file. If f is a device, return
   a file size of -1 */
{
  struct stat s;        /* results of stat() */
  char name[FNMAX];
  int len = strlen(f);

  strcpy(name, f);
  if (name[len - 1] == '/')
    name[len - 1] = 0; 
  /* not all systems allow stat'ing a file with / appended */

  if (strcmp(f, "-") == 0) {
    if (fstat(fileno(stdin), &s) != 0)
      error("fstat(stdin)");
  } else if ((
#ifdef S_IFLNK
             linkput ? lstat(name, &s) :
#endif
             SSTAT(name, &s)) != 0)
             /* Accept about any file kind including directories
              * (stored with trailing / with -r option)
              */
    return 0;

  if (a != NULL)
#ifdef OS2
    *a = (s.st_mode << 16) | GetFileMode(name);
#else
    *a = (s.st_mode << 16) | !(s.st_mode & S_IWRITE);
#endif
  if (n != NULL)
    *n = (s.st_mode & S_IFREG) == 0 ? -1L : s.st_size;

#ifdef OS2
  return GetFileTime(name);
#else /* !OS2 */
#  ifdef VMS
     return unix2dostime(&s.st_ctime);   /* Use creation time in VMS */
#  else /* !VMS */
#    ifdef ATARI_ST
       return s.st_mtime; /* Turbo C doesn't use UNIX times */
#    else
       return unix2dostime(&s.st_mtime);
#    endif
#  endif /* ?VMS */
#endif /* ?OS2 */
}


int issymlnk(a)
ulg a;                  /* Attributes returned by filetime() */
/* Return true if the attributes are those of a symbolic link */
{
#ifdef S_IFLNK
  return ((a >> 16) & S_IFMT) == S_IFLNK;
#else /* !S_IFLNK */
  return (int)a & 0;    /* avoid warning on unused parameter */
#endif /* ?S_IFLNK */
}


int deletedir(d)
char *d;                /* directory to delete */
/* Delete the directory *d if it is empty, do nothing otherwise.
   Return the result of rmdir(), delete(), or system().
 */
{
#ifdef MACOS
  warn("deletedir not implemented yet", "");
  return 127;
#else
#ifdef RMDIR
  /* code from Greg Roelofs, who horked it from Mark Edwards (unzip) */
  int r, len;
  char *s;              /* malloc'd string for system command */

  len = strlen(d);
  if ((s = malloc(len + 34)) == NULL)
    return 127;

#ifdef VMS
  {
    char *c;            /* pointer into VMS path */
    /* convert "DEV:[DIR.SUB1.SUB2]" form to "DEV:[DIR.SUB1]SUB2.DIR;0" */
    strcat(strcpy(s, "set prot=(o:rwed) "), d);   /* d starts at s+18 */
    if (*(c = s+17+len) != ']')
    {
      free(s);
      return 127;
    }
    strcpy(c, ".DIR;0");        /* 0 translates to highest version */
    while (--c > s+18  &&  *c != '.'  &&  *c != '[') ;
    if (c == s+18)
    {
      free(s);
      return 127;
    }
    if (*c == '.')
      *c = ']';
    else if (*--c == ']')  /* presumably of form "DEV:[DIR.SUB1.][SUB2]" */
    {                      /* (possible to have "DEV:[DIR.SUB1.][][SUB2]"?) */
      char *b = c + 2;
      c[-1] = ']';
      while (*c++ = *b++) ;
    }
    else        /* must have reached device name:  can't delete top level */
    {
      free(s);
      return 127;
    }
  }
  /* unprotect directory and delete it as a file.  May fail if exists 
     normal file "foo.dir" on top of directory "foo.dir" */
  system(s);
  r = delete(s+18);
#else /* !VMS */
  sprintf(s, "IFS=\" \t\n\" /bin/rmdir %s 2>/dev/null", d);
  r = system(s);
#endif /* ?VMS */
  free(s);
  return r;
#else /* !RMDIR */
  return rmdir(d);
#endif /* ?RMDIR */
#endif /* ?MACOS */
}


#endif /* !UTIL */

int destroy(f)
char *f;                /* file to delete */
/* Delete the file *f, returning non-zero on failure. */
{
  return unlink(f);
}


int replace(d, s)
char *d, *s;            /* destination and source file names */
/* Replace file *d by file *s, removing the old *s.  Return an error code
   in the ZE_ class. */
{
  struct stat t;        /* results of stat() */

  if (SSTAT(d, &t) == 0 && unlink(d))
    return ZE_CREAT;                    /* Can't erase zip file--give up */
  if (link(s, d))                       /* Just move s on top of d */
#if !defined(VMS) && !defined(ATARI_ST)
    /* For VMS & ATARI assume failure is EXDEV */
    if (errno != EXDEV
#  ifdef ENOTSAM
       && errno != ENOTSAM /* Used at least on Turbo C */
#  endif
        ) return ZE_CREAT;
    else
#endif
    {
      FILE *f, *g;      /* source and destination files */
      int r;            /* temporary variable */

      if ((f = fopen(s, FOPR)) == NULL) {
        fprintf(stderr," replace: can't open %s\n", s);
        return ZE_TEMP;
      }
      if ((g = fopen(d, FOPW)) == NULL)
      {
        fclose(f);
        return ZE_CREAT;
      }
      r = fcopy(f, g, (ulg)-1L);
      fclose(f);
      if (fclose(g) || r != ZE_OK)
      {
        unlink(d);
        return r ? (r == ZE_TEMP ? ZE_WRITE : r) : ZE_WRITE;
      }
#ifdef VMS /* only delete if rename failed:  previous version may exist */
      unlink(s);
    }
#else /* !VMS */
    }
  unlink(s);
#endif /* !VMS */
  return ZE_OK;
}


int getfileattr(f)
char *f;                /* file path */
/* Return the file attributes for file f or 0 if failure */
{
  struct stat s;

  return SSTAT(f, &s) == 0 ? s.st_mode : 0;
}


int setfileattr(f, a)
char *f;                /* file path */
int a;                  /* attributes returned by getfileattr() */
/* Give the file f the attributes a, return non-zero on failure */
{
#if defined (VMS) || defined(MACOS)
  return 0;
#else /* !VMS */
  return chmod(f, a);
#endif /* ?VMS */
}


#ifdef NO_MKTEMP

char *tempname(zip)
  char *zip;              /* path name of zip file to generate temp name for */

/* Return a temporary file name in its own malloc'ed space.
 * This function might accidentally destroy an existing file
 * with extension .$z$ . Use mktemp below if you have it on your system.
 */
{
  char *p;              /* temporary pointer */
  char *t;              /* malloc'ed space for name */

  if ((t = malloc(strlen(zip)+5)) == NULL)
    return NULL;
  strcpy(t, zip);
  if ((p = strrchr(t, '.')) != NULL &&
      (!strncmp(p, ".zip", 4) || !strncmp(p, ".ZIP", 4)))
      /* strncmp to avoid problems with VMS ';' */
    strcpy(p, ".$z$");
  else
    strcat(t, ".$z$");

  return t;
}
#else /* !NO_MKTEMP */

char *tempname(zip)
  char *zip;              /* path name of zip file to generate temp name for */

/* Return a temporary file name in its own malloc'ed space, using tempath. */
{
  char *t = zip;   /* malloc'ed space for name (use zip to avoid warning) */

  if (tempath != NULL)
  {
    if ((t = malloc(strlen(tempath)+10)) == NULL)
      return NULL;
    strcpy(t, tempath);
#ifndef VMS
    if (t[strlen(t)-1] != '/')
      strcat(t, "/");
#endif
  }
  else
  {
    if ((t = malloc(9)) == NULL)
      return NULL;
    *t = 0;
  }
  strcat(t, "_ZXXXXXX");
  return mktemp(t);
}

#endif /* NO_MKTEMP */


int fcopy(f, g, n)
FILE *f, *g;            /* source and destination files */
ulg n;                  /* number of bytes to copy or -1 for all */
/* Copy n bytes from file *f to file *g, or until EOF if n == -1.  Return
   an error code in the ZE_ class. */
{
  char *b;              /* malloc'ed buffer for copying */
  extent k;             /* result of fread() */
  ulg m;                /* bytes copied so far */

  if ((b = malloc(CBSZ)) == NULL)
    return ZE_MEM;
  m = 0;
  while (n == -1L || m < n)
  {
    if ((k = fread(b, 1, n == -1 ?
                   CBSZ : (n - m < CBSZ ? (extent)(n - m) : CBSZ), f)) == 0)
      if (ferror(f))
      {
        free((voidp *)b);
        return ZE_READ;
      }
      else
        break;
    if (fwrite(b, 1, k, g) != k)
    {
      free((voidp *)b);
      fprintf(stderr," fcopy: write error\n");
      return ZE_TEMP;
    }
    m += k;
  }
  free((voidp *)b);
  return ZE_OK;
}


#ifdef CRYPT

#ifndef MSDOS

#ifdef VMS

int echo(opt)
    int opt;
{
/*---------------------------------------------------------------------------
    Based on VMSmunch.c, which in turn was based on Joe Meadows' file.c code.
  ---------------------------------------------------------------------------
     * For VMS v5.x:
     *   IO$_SENSEMODE/SETMODE info:  Programming, Vol. 7A, System Programming,
     *     I/O User's: Part I, sec. 8.4.1.1, 8.4.3, 8.4.5, 8.6
     *   sys$assign(), sys$qio() info:  Programming, Vol. 4B, System Services,
     *     System Services Reference Manual, pp. sys-23, sys-379
     *   fixed-length descriptor info:  Programming, Vol. 3, System Services,
     *     Intro to System Routines, sec. 2.9.2
     * GRR, 15 Aug 91
  ---------------------------------------------------------------------------*/
    static struct dsc$descriptor_s DevDesc =
        {9, DSC$K_DTYPE_T, DSC$K_CLASS_S, "SYS$INPUT"};
     /* {dsc$w_length, dsc$b_dtype, dsc$b_class, dsc$a_pointer}; */
    static short           DevChan, iosb[4];
    static long            i, status;
    static unsigned long   oldmode[2], newmode[2];   /* each = 8 bytes */
  

/*---------------------------------------------------------------------------
    Assign a channel to standard input.
  ---------------------------------------------------------------------------*/

    status = sys$assign(&DevDesc, &DevChan, 0, 0);
    if (!(status & 1))
        return status;

/*---------------------------------------------------------------------------
    Use sys$qio and the IO$_SENSEMODE function to determine the current tty
    status (for password reading, could use IO$_READVBLK function instead,
    but echo on/off will be more general).
  ---------------------------------------------------------------------------*/

    status = sys$qio(0, DevChan, IO$_SENSEMODE, &iosb, 0, 0,
                     oldmode, 8, 0, 0, 0, 0);
    if (!(status & 1))
        return status;
    status = iosb[0];
    if (!(status & 1))
        return status;

/*---------------------------------------------------------------------------
    Copy old mode into new-mode buffer, then modify to be either NOECHO or
    ECHO (depending on function argument opt).
  ---------------------------------------------------------------------------*/

    newmode[0] = oldmode[0];
    newmode[1] = oldmode[1];
    if (opt == 0)
        newmode[1] |= TT$M_NOECHO;                      /* set NOECHO bit */
    else
        newmode[1] &= ~((unsigned long) TT$M_NOECHO);   /* clear NOECHO bit */

/*---------------------------------------------------------------------------
    Use the IO$_SETMODE function to change the tty status.
  ---------------------------------------------------------------------------*/

    status = sys$qio(0, DevChan, IO$_SETMODE, &iosb, 0, 0,
                     newmode, 8, 0, 0, 0, 0);
    if (!(status & 1))
        return status;
    status = iosb[0];
    if (!(status & 1))
        return status;

/*---------------------------------------------------------------------------
    Deassign the sys$input channel by way of clean-up, then exit happily.
  ---------------------------------------------------------------------------*/

    status = sys$dassgn(DevChan);
    if (!(status & 1))
        return status;

    return SS$_NORMAL;   /* we be happy */

} /* end function echo() */


#else /* !VMS */

local int echofd = -1;  /* file descriptor whose echo is off */

void echoff(f)
int f;                  /* file descriptor to turn echo off on */
/* Turn echo off for file descriptor f.  Assumes that f is a tty device. */
{
  struct sgttyb sg;     /* tty device structure */

  echofd = f;
  GTTY(f, &sg);                                 /* get settings */
  sg.sg_flags &= ~ECHO;                         /* turn echo off */
  STTY(f, &sg);
}

void echon()
/* Turn echo back on for file descriptor echofd. */
{
  struct sgttyb sg;     /* tty device structure */

  if (echofd != -1)
  {
    GTTY(echofd, &sg);                          /* get settings */
    sg.sg_flags |= ECHO;                        /* turn echo on */
    STTY(echofd, &sg);
    echofd = -1;
  }
}

#endif /* ?VMS */

#endif /* !MSDOS */


char *getp(m, p, n)
char *m;                /* prompt for password */
char *p;                /* return value: line input */
int n;                  /* bytes available in p[] */
/* Get a password of length n-1 or less into *p using the prompt *m.
   The entered password is not echoed.  Return p on success, NULL on
   failure (can't get controlling tty). */
{
  char c;               /* one-byte buffer for read() to use */
  int i;                /* number of characters input */
  char *w;              /* warning on retry */

#ifndef MSDOS
#ifndef VMS
  int f;                /* file decsriptor for tty device */

  /* Turn off echo on tty */
  if (!isatty(2))
    return NULL;                                /* error if not tty */
  if ((f = open(ttyname(2), 0, 0)) == -1)
    return NULL;
#endif /* !VMS */
  echoff(f);                                    /* turn echo off */
#endif /* !MSDOS */

  /* Get password */
  w = "";
  do {
#ifdef VMS   /* bug:  VMS adds '\n' to NULL fputs (apparently) */
    if (*w)
#endif /* VMS */
    fputs(w, stderr);                           /* warning if back again */
    fputs(m, stderr);                           /* prompt */
    fflush(stderr);
    i = 0;
    do {                                        /* read line, keeping n */
#ifdef MSVMS
      if ((c = (char)getch()) == '\r')
        c = '\n';
#else /* !MSVMS */
      read(f, &c, 1);
#endif /* ?MSVMS */
      if (i < n)
        p[i++] = c;
    } while (c != '\n');
    putc('\n', stderr);  fflush(stderr);
    w = "(line too long--try again)\n";
  } while (p[i-1] != '\n');
  p[i-1] = 0;                                   /* terminate at newline */

#ifndef MSDOS
  echon();                                      /* turn echo back on */
#ifndef VMS
  close(f);
#endif /* !VMS */
#endif /* !MSDOS */

  /* Return pointer to password */
  return p;
}

#endif /* ?CRYPT */


#ifdef ZMEM

/************************/
/*  Function memset()  */
/************************/

/*
 * memset - for systems without it
 *  bill davidsen - March 1990
 */

char *
memset(buf, init, len)
register char *buf;     /* buffer loc */
register int init;      /* initializer */
register unsigned int len;   /* length of the buffer */
{
    char *start;

    start = buf;
    while (len--) *(buf++) = init;
    return(start);
}


/************************/
/*  Function memcpy()  */
/************************/

char *
memcpy(dst,src,len)           /* v2.0f */
register char *dst, *src;
register unsigned int len;
{
    char *start;

    start = dst;
    while (len--)
        *dst++ = *src++;
    return(start);
}


/************************/
/*  Function memcmp()  */
/************************/

int
memcmp(b1,b2,len)                     /* jpd@usl.edu -- 11/16/90 */
register char *b1, *b2;
register unsigned int len;
{

    if (len) do {             /* examine each byte (if any) */
      if (*b1++ != *b2++)
        return (*((uch *)b1-1) - *((uch *)b2-1));  /* exit when miscompare */
       } while (--len);

    return(0);        /* no miscompares, yield 0 result */
}

#endif  /* ZMEM */

#ifdef __TURBOC__

/************************/
/*  Function fcalloc()  */
/************************/

/* Turbo C malloc() does not allow dynamic allocation of 64K bytes
 * and farmalloc(64K) returns a pointer with an offset of 8, so we
 * must fix the pointer. Warning: the pointer must be put back to its
 * original form in order to free it.
 * For MSC, use halloc instead of this function (see tailor.h).
 */
void far * fcalloc(items, size)
    unsigned items; /* number of items */
    unsigned size;  /* item size */
{
    void far * buf = farmalloc((ulg)items*size + 16L);
    /* Normalize the pointer to seg:0 */
    *((int*)&buf+1) += ((unsigned)((uch*)buf-0) + 15) >> 4;
    *(int*)&buf = 0;
    return buf; /* buf stays NULL if alloc failed */
}

#endif /* __TURBOC__ */
