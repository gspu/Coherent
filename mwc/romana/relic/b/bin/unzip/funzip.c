/* funzip.c -- Not copyrighted 1992 by Mark Adler */

#define VERSION "1.3p1 of 10 January 1993"


/* You can do whatever you like with this source file, though I would
   prefer that if you modify it and redistribute it that you include
   comments to that effect with your name and the date.  Thank you.

   History:
   vers    date          who           what
   ----  ---------  --------------  ------------------------------------
    1.0  13 Aug 92  M. Adler        really simple unzip filter.
    1.1  13 Aug 92  M. Adler        cleaned up somewhat, give help if
                                    stdin not redirected, warn if more
                                    zip file entries after the first.
    1.2  15 Aug 92  M. Adler        added check of lengths for stored
                                    entries, added more help.
    1.3  16 Aug 92  M. Adler        removed redundant #define's, added
                                    decryption.
   1.3p1 10 Jan 93  G. Roelofs      incorporated fixes from 2.2 beta:
           [1.4]    G. Roelofs        added exit(0).
           [1.5]    K. U. Rommel      changed read/write modes for OS/2.
           [1.7]    G. Roelofs        changed to use DOS_OS2.
           [1.8]    M. Adler          improved inflation error msgs.
           [1.9]    G. Roelofs        changed ULONG/UWORD/byte to ulg/ush/uch;
                                      renamed inflate_entry() to inflate().
           [2.1]    J. Gailly         fixed crypt/store bug,
                    G. Roelofs        fixed decryption check (single byte).
 */


/*

   All funzip does is take a zip file from stdin and decompress the
   first entry to stdout.  The entry has to be either deflated or
   stored.  If the entry is encrypted, then the decryption password
   must be supplied on the command line as the first argument.

   funzip needs to be linked with inflate.o compiled from the unzip
   source.  If decryption is desired, then it needs to be compiled
   with -DCRYPT and linked also with crypt.o.

 */

#include "unzip.h"

/* enforce binary i/o if recognized */
#if defined(__STDC__) || defined(DOS_OS2)
#  define BINIO
#endif

#ifdef BINIO
#  define FOPR "rb"
#  define FOPW "wb"
#else
#  define FOPR "r"
#  define FOPW "w"
#endif

/* PKZIP header definitions */
#define LOCSIG 0x04034b50L      /* four byte lead-in (lsb first) */
#define LOCFLG 6                /* offset of bit flag */
#define  CRPFLG 1               /*  bit for encrypted entry */
#define  EXTFLG 8               /*  bit for extended local header */
#define LOCHOW 8                /* offset of compression method */
#define LOCTIM 10               /* file mod time (for decryption) */
#define LOCCRC 14               /* offset of crc */
#define LOCSIZ 18               /* offset of compressed size */
#define LOCLEN 22               /* offset of uncompressed length */
#define LOCFIL 26               /* offset of file name field length */
#define LOCEXT 28               /* offset of extra field length */
#define LOCHDR 30               /* size of local header, including sig */
#define EXTHDR 16               /* size of extended local header, inc sig */

#define RAND_HEAD_LEN 12        /* length of encryption header */

/* Macros for getting two byte and four byte header values */
#define SH(p) ((UWORD)(byte)((p)[0]) | ((UWORD)(byte)((p)[1]) << 8))
#define LG(p) ((ULONG)(SH(p)) | ((ULONG)(SH((p)+2)) << 16))

/* Function prototypes */
ULONG updcrc OF((byte *, int));
int inflate OF((void));
void err OF((int, char *));
void main OF((int, char **));

/* Globals */
FILE *in, *out;                 /* input and output files */
union work area;                /* inflate sliding window */
byte *outbuf;                   /* malloc'ed output buffer */
byte *outptr;                   /* points to next byte in output buffer */
int outcnt;                     /* bytes in output buffer */
ULONG outsiz;                   /* total bytes written to out */
int decrypt;                    /* flag to turn on decryption */
char *key;                      /* not used--needed to link crypt.c */

/* Masks for inflate.c */
UWORD mask_bits[] = {
    0x0000,
    0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
    0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};


/* Table of CRC-32's of all single byte values (made by makecrc.c) */
ULONG crc_32_tab[] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};


ULONG updcrc(s, n)
byte *s;                /* pointer to bytes to pump through */
int n;                  /* number of bytes in s[] */
/* Run a set of bytes through the crc shift register.  If s is a NULL
   pointer, then initialize the crc shift register contents instead.
   Return the current crc in either case. */
{
  register ULONG c;       /* temporary variable */

  static ULONG crc = 0xffffffffL; /* shift register contents */

  if (s == NULL)
    c = 0xffffffffL;
  else
  {
    c = crc;
    while (n--)
      c = crc_32_tab[((int)c ^ (*s++)) & 0xff] ^ (c >> 8);
  }
  crc = c;
  return c ^ 0xffffffffL;       /* (instead of ~c for 64-bit machines) */
}


void err(n, m)
int n;
char *m;
/* Exit on error with a message and a code */
{
  fprintf(stderr, "funzip error: %s\n", m);
  exit(n);
}


int ReadByte(b)
UWORD *b;
/* Used by inflate.c to get a byte (archaism from unzip) */
{
  register int c = getc(in);

  if (c == EOF)
    return 0;
#ifdef CRYPT
  if (decrypt)
    update_keys(c ^= decrypt_byte());
#endif /* CRYPT */
  *b = (UWORD)c;
  return 8;
}


int FlushOutput()
/* Empty output buffer */
{
  if (outcnt)
  {
    updcrc(outbuf, outcnt);
    if (fwrite((char *)outbuf, 1,outcnt,out) != outcnt)
      err(9, "out of space on stdout");
    outsiz += outcnt;
    outptr = outbuf;
    outcnt = 0;
  }
  return 0;
}


void main(argc, argv)
int argc;
char **argv;
/* Given a zip file on stdin, decompress the first entry to stdout. */
{
  byte h[LOCHDR];               /* first local header */

  /* if stdin not redirected, give the user help */
  if (isatty(0))
  {
    fprintf(stderr, "FUnZip (Filter UnZip), version %s\n%s", VERSION,
#ifdef CRYPT
      "usage: funzip [password] < infile.zip > outfile\n");
#else /* !CRYPT */
      "usage: funzip < infile.zip > outfile\n");
#endif /* ?CRYPT */
    fprintf(stderr,
      "       extracts to stdout the first zip entry of stdin.\n");
    exit(3);
  }

  /* prepare to be a binary filter */
  if ((outbuf = (byte *)malloc(OUTBUFSIZ)) == NULL)
    err(1, "out of memory");
#ifdef DOS_OS2
  setmode(0, O_BINARY); /* some buggy C libraries require BOTH :-( the  */
#endif                  /*  setmode() call AND the fdopen() in binary mode */
  if ((in = fdopen(0, FOPR)) == NULL)
    err(2, "cannot find stdin");
#ifdef DOS_OS2
  setmode(1, O_BINARY);
#endif
  if ((out = fdopen(1, FOPW)) == NULL)
    err(2, "cannot write to stdout");

  /* read local header, check validity, and skip name and extra fields */
  if (fread((char *)h, 1, LOCHDR, in) != LOCHDR || LG(h) != LOCSIG)
    err(3, "input not a zip file or empty");
  if (SH(h + LOCHOW) != STORED && SH(h + LOCHOW) != DEFLATED)
    err(3, "first entry not deflated or stored--can't funzip");
  fseek(in, (long)(SH(h + LOCFIL)) + (long)(SH(h + LOCEXT)), 1);

  /* if entry encrypted, decrypt and validate encryption header */
  if ((decrypt = h[LOCFLG] & CRPFLG) != 0)
#ifdef CRYPT
  {
    UWORD i, e;

    if (argc < 2)
      err(3, "need password on command line for encrypted entry");
    init_keys(argv[1]);
    for (i = 0; i < RAND_HEAD_LEN; i++)
      ReadByte(&e);
    if (e != (ush)(h[LOCFLG] & EXTFLG ? h[LOCTIM + 1] : h[LOCCRC + 3]))
      err(3, "incorrect password for first entry");
  }
#else /* !CRYPT */
    err(3, "cannot decrypt entry (need to recompile funzip with crypt.c)");
#endif /* ?CRYPT */

  /* prepare output buffer and crc */
  outptr = outbuf;
  outcnt = 0;
  outsiz = 0L;
  updcrc(NULL, 0);

  /* decompress */
  if (h[LOCHOW])
  {                             /* deflated entry */
    int r;
 
    if ((r = inflate()) != 0)
      if (r == 3)
        err(1, "out of memory");
      else
        err(4, "invalid compressed data--format violated");
  }
  else
  {                             /* stored entry */
    register ULONG n;

    n = LG(h + LOCLEN);
    if (n != LG(h + LOCSIZ) - (decrypt ? RAND_HEAD_LEN : 0)) {
      fprintf(stderr, "len %ld, siz %ld\n", n, LG(h + LOCSIZ));
      err(4, "invalid compressed data--length mismatch");
  }
    while (n--) {
      ush c = getc(in);
#ifdef CRYPT
      if (decrypt)
        update_keys(c ^= decrypt_byte());
#endif
      OUTB(c);
    }
  }
  FlushOutput();
  fflush(out);

  /* if extended header, get it */
  if ((h[LOCFLG] & EXTFLG) &&
      fread((char *)h + LOCCRC - 4, 1, EXTHDR, in) != EXTHDR)
    err(3, "zip file ended prematurely");

  /* validate decompression */
  if (LG(h + LOCCRC) != updcrc(outbuf, 0))
    err(4, "invalid compressed data--crc error");
  if (LG(h + LOCLEN) != outsiz)
    err(4, "invalid compressed data--length error");

  /* check if there are more entries */
  if (fread((char *)h, 1, 4, in) == 4 && LG(h) == LOCSIG)
    fprintf(stderr,
      "funzip warning: zip file has more than one entry--rest ignored\n");

  exit(0);
}
