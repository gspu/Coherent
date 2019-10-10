/*

 Copyright (C) 1990-1992 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included
 unmodified, that it is not sold for profit, and that this copyright notice
 is retained.

*/

/*
 *  zipup.c by Mark Adler. Includes modifications by Jean-loup Gailly.
 */

#define NOCPYRT         /* this is not a main module */
#include <ctype.h>
#include "zip.h"
#include "revision.h"
#ifdef OS2
#  include "os2zip.h"
#endif

/* Use the raw functions for MSDOS and Unix to save on buffer space.
   They're not used for VMS since it doesn't work (raw is weird on VMS).
   (This sort of stuff belongs in fileio.c, but oh well.) */
#ifdef VMS
#  define fhow "r"
#  define fbad NULL
   typedef void *ftype;
#  define zopen(n,p)   (vms_native?vms_open(n)    :(ftype)fopen((n),(p)))
#  define zread(f,b,n) (vms_native?vms_read(f,b,n):fread((b),1,(n),(FILE*)(f)))
#  define zclose(f)    (vms_native?vms_close(f)   :fclose((FILE*)(f)))
#  define zerr(f)      (vms_native?vms_error(f)   :ferror((FILE*)(f)))
#  define zstdin stdin
   ftype vms_open OF((char *));
   int vms_read OF((ftype, char *, int));
   int vms_close OF((ftype));
   int vms_error OF((ftype));
#else /* !VMS */
#  if defined(MSDOS) && !defined(ATARI_ST)
#    include <io.h>
#    include <fcntl.h>
#    define fhow (O_RDONLY|O_BINARY)
#  else /* !MSDOS */
     int open OF((char *, int));
     int read OF((int, char *, int));
     int close OF((int));
     int lseek OF((int, long, int));
#    define fhow 0
#  endif /* ?MSDOS */
   typedef int ftype;
#  define fbad (-1)
#  define zopen(n,p) open(n,p)
#  define zread(f,b,n) read(f,b,n)
#  define zclose(f) close(f)
#  define zerr(f) (k==(extent)(-1L))
#  define zstdin 0
#endif /* ?VMS */


/* Local data */

#ifndef UTIL
local ulg crc;       /* crc on uncompressed file data */
local ftype ifile;   /* file to compress */
#endif
ulg isize;           /* input file size. global only for debugging */    

/* Local functions */
#if defined(PROTO) && !defined(UTIL)
   local int suffixes(char *, char *);
#endif


/* Note: a zip "entry" includes a local header (which includes the file
   name), an encryption header if encrypting, the compressed data
   and possibly an extended local header. */

int zipcopy(z, x, y)
struct zlist far *z;    /* zip entry to copy */
FILE *x, *y;            /* source and destination files */
/* Copy the zip entry described by *z from file *x to file *y.  Return an
   error code in the ZE_ class.  Also update tempzn by the number of bytes
   copied. */
{
  ulg n;                /* holds local header offset */

  if (fseek(x, z->off, SEEK_SET))
    return ferror(x) ? ZE_READ : ZE_EOF;
  z->off = tempzn;
  n = 4 + LOCHEAD + (long)z->nam + (long)z->ext + z->siz;
  /* copy the extended local header if there is one */
  if (z->lflg & 8) n += 16;
  tempzn += n;
  return fcopy(x, y, n);
}


#ifndef UTIL

int percent(n, m)
long n, m;               /* n is the original size, m is the new size */
/* Return the percentage compression from n to m using only integer
   operations */
{
  if (n > 0xffffffL)            /* If n >= 16M */
  {                             /*  then divide n and m by 256 */
    n += 0x80;  n >>= 8;
    m += 0x80;  m >>= 8;
  }
  return n ? (int)(1 + (200 * (n - m)/n)) / 2 : 0;
}

local int suffixes(a, s)
char *a;                /* name to check suffix of */
char *s;                /* list of suffixes separated by : or ; */
/* Return true if a ends in any of the suffixes in the list s. */
{
  int m;                /* true if suffix matches so far */
  char *p;              /* pointer into special */
  char *q;              /* pointer into name a */

  m = 1;
#ifdef VMS
  if( (q = strrchr(a,';')) != NULL )    /* Cut out VMS file version */
    --q;
  else
    q = a + strlen(a) - 1;
#else
  q = a + strlen(a) - 1;
#endif
  for (p = s + strlen(s) - 1; p >= s; p--)
    if (*p == ':' || *p == ';')
      if (m)
        return 1;
      else
      {
        m = 1;
#ifdef VMS
        if( (q = strrchr(a,';')) != NULL )      /* Cut out VMS file version */
          --q;
        else
          q = a + strlen(a) - 1;
#else
        q = a + strlen(a) - 1;
#endif
      }
    else
    {
      m = m && q >= a && case_map(*p) == case_map(*q);
      q--;
    }
  return m;
}

int zipup(z, y)
struct zlist far *z;    /* zip entry to compress */
FILE *y;                /* output file */
/* Compress the file z->name into the zip entry described by *z and write
   it to the file *y. Encrypt if requested.  Return an error code in the
   ZE_ class.  Also, update tempzn by the number of bytes written. */
{
  ulg a = 0L;           /* attributes returned by filetime() */
  char *b;              /* malloc'ed file buffer */
  extent k = 0;         /* result of zread */
  int l = 0;            /* true if this file is a symbolic link */
  int m;                /* method for this entry */
  ulg o, p;             /* offsets in zip file */
  long q = -2L;         /* size returned by filetime */
  int r;                /* temporary variable */
  ulg s = 0L;           /* size of compressed data */
  int isdir;            /* set for a directory name */

  isdir = z->name[strlen(z->name)-1] == '/';

  if ((z->tim = filetime(z->name, &a, &q)) == 0 || q < -1L)
    return ZE_OPEN;
  /* q is set to -1 if the input file is a device */

  z->nam = strlen(z->zname);

  /* Select method based on the suffix and the global method */
  m = special != NULL && suffixes(z->name, special) ? STORE : method;

  /* Open file to zip up unless it is stdin */
  if (strcmp(z->name, "-") == 0)
  {
    ifile = (ftype)zstdin;
#ifdef MSDOS
    setmode(zstdin, O_BINARY);
#endif
  }
  else
  {
#ifdef VMS
   if (vms_native)
     get_vms_attributes(z);
#endif
#ifdef OS2
    GetEAs(z->name, &z->extra, &z->ext, &z->cextra, &z->cext);
    /* store data in local header, and size only in central headers */
#endif
    l = issymlnk(a);
    if (l)
      ifile = fbad;
    else if (isdir) { /* directory */
      ifile = fbad;
      m = STORE;
    }
    else if ((ifile = zopen(z->name, fhow)) == fbad)
      return ZE_OPEN;
  }

  if (l || q == 0)
    m = STORE;
  if (m == BEST)
    m = DEFLATE;

  /* Do not create STORED files with extended local headers if the
   * input size is not known, because such files could not be extracted.
   * So if the zip file is not seekable and the input file is not
   * on disk, obey the -0 option by forcing deflation with stored block.
   * Note however that using "zip -0" as filter is not very useful...
   * ??? to be done.
   */

  /* Fill in header information and write local header to zip file.
   * This header will later be re-written since compressed length and
   * crc are not yet known.
   */

  /* (Assume ext, cext, com, and zname already filled in.) */
#ifdef OS2
  z->vem = z->dosflag ? 20 :            /* Made under MSDOS by PKZIP 2.0 */
  /* We for a FAT file system, we must cheat and pretend that the
   * file was not made on OS2 but under DOS. unzip is confused otherwise.
   */
#else
  z->vem = dosify ? 20 :                /* Made under MSDOS by PKZIP 2.0 */
#endif
#ifdef VMS
                    0x200 + REVISION;   /* Made under VMS by this Zip */
#else /* !VMS */
# ifdef OS2
                    0x600 + REVISION;   /* Made under OS/2 by this Zip */
# else /* !OS2 */
#  ifdef MSDOS
                    0     + REVISION;   /* Made under MSDOS by this Zip */
#  else
                    0x300 + REVISION;   /* Made under Unix by this Zip */
#  endif /* MSDOS */
# endif /* ?OS2 */
#endif /* ?VMS */

  z->ver = 20;                          /* Need PKUNZIP 2.0 */
  z->crc = 0;  /* to be updated later */
  /* Assume first that we will need an extended local header: */
  z->flg = 8;  /* to be updated later */
#ifdef CRYPT
  if (key != NULL) {
    z->flg |= 1;
    /* Since we do not yet know the crc here, we pretend that the crc
     * is the modification time:
     */
    z->crc = z->tim << 16;
  }
#endif
  z->lflg = z->flg;
  z->how = m;                             /* may be changed later  */
  z->siz = m == STORE && q >= 0 ? q : 0;  /* will be changed later  */
  z->len = q >= 0 ? q : 0;                /* may be changed later  */
  z->dsk = 0;
  z->att = BINARY;                        /* may be changed later */
  z->atx = z->dosflag ? a & 0xff : a;      /* Attributes from filetime() */
  z->off = tempzn;
  if ((r = putlocal(z, y)) != ZE_OK)
    return r;
  tempzn += 4 + LOCHEAD + z->nam + z->ext;

#ifdef CRYPT
  if (key != NULL) {
    crypthead(key, z->crc, y);
    z->siz += 12;  /* to be updated later */
    tempzn += 12;
  }
#endif
  o = ftell(y); /* for debugging only */

  /* Write stored or deflated file to zip file */
  isize = 0L;
  crc = updcrc((char *)NULL, 0);

  if (m == DEFLATE) {
     bi_init(y);
     z->att = (ush)UNKNOWN;
     ct_init(&z->att, &m);
     lm_init(level, &z->flg);
     s = deflate();
  }
  else
  {
    if ((b = malloc(CBSZ)) == NULL)
       return ZE_MEM;

    if (!isdir) /* no read for directories */
    while ((k = l ? rdsymlnk(z->name, b, CBSZ) : zread(ifile, b, CBSZ)) > 0)
    {
      isize += k;
      crc = updcrc(b, k);
      if (zfwrite(b, 1, k, y) != k)
      {
        free((voidp *)b);
        return ZE_TEMP;
      }
#ifdef MINIX
      if (l)
        q = k;
#endif /* MINIX */
      if (l)
        break;
    }
    free((voidp *)b);
    s = isize;
  }
  if (ifile != fbad && zerr(ifile))
    return ZE_READ;
  if (ifile != fbad)
    zclose(ifile);

  tempzn += s;
  p = tempzn; /* save for future fseek() */

#ifndef VMS
  /* Check input size (but not in VMS--variable record lengths mess it up) */
  if (q >= 0 && isize != (ulg)q && !translate_eol)
  {
    fprintf(mesg, " i=%ld, q=%ld ", isize, q);
    error("incorrect input size");
  }
#endif /* !VMS */

  /* Try to rewrite the local header with correct information */
  z->crc = crc;
  z->siz = s;
#ifdef CRYPT
  if (key != NULL)
    z->siz += 12;
#endif
  z->len = isize;
  if (fseek(y, z->off, SEEK_SET)) {
    if (z->how != (ush) m)
       error("can't rewrite method");
    if (m == STORE && q < 0)
       error("zip -0 not allowed for input/output from/to pipe or device");
    if ((r = putextended(z, y)) != ZE_OK)
      return r;
    tempzn += 16L;
    z->flg = z->lflg; /* if flg modified by inflate */
  } else {
     /* seek ok, ftell() should work, check compressed size */
#ifndef VMS
    if (p - o != s) {
      fprintf(mesg, " s=%ld, actual=%ld ", s, p-o);
      error("incorrect compressed size");
    }
#endif
    z->how = m;
    if ((z->flg & 1) == 0)
      z->flg &= ~8; /* clear the extended local header flag */
    z->lflg = z->flg;
    /* rewrite the local header: */
    if ((r = putlocal(z, y)) != ZE_OK)
      return r;
    if (fseek(y, p, SEEK_SET))
      return ZE_READ;
    if ((z->flg & 1) != 0) {
      /* encrypted file, extended header still required */
      if ((r = putextended(z, y)) != ZE_OK)
        return r;
      tempzn += 16L;
    }
  }

  /* Display statistics */
  if (noisy)
  {
    if (verbose)
      fprintf(mesg, " (in=%lu) (out=%lu)", isize, s);
    if (m == DEFLATE)
      fprintf(mesg, " (deflated %d%%)\n", percent(isize, s));
    else
      fprintf(mesg, " (stored 0%%)\n");
    fflush(mesg);
  }
  return ZE_OK;
}


int file_read(buf, size)
  char *buf;
  unsigned size;
/* Read a new buffer from the current input file, and update the crc and
 * input file size.
 * IN assertion: size >= 2 (for end-of-line translation)
 */
{
  unsigned len;
  char far *b;
  if (translate_eol) {
    size >>= 1;
    b = buf+size;
    size = len = zread(ifile, b, size);
    if (len == (unsigned)EOF || len == 0) return len;
    do {
       if ((*buf++ = *b++) == '\n') *(buf-1) = '\r', *buf++ = '\n', len++;
    } while (--size != 0);
    buf -= len;
  } else {
    len = zread(ifile, buf, size);
    if (len == (unsigned)EOF || len == 0) return len;
  }
  crc = updcrc(buf, len);
  isize += (ulg)len;
  return len;
}
#endif /* !UTIL */
