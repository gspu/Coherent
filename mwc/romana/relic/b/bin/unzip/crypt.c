/*
   This code is not copyrighted and is put in the public domain. It
   was originally written in Europe and can be freely distributed from
   any country except the U.S.A. If this code is imported in the U.S.A,
   it cannot be re-exported from the U.S.A to another country. (This
   restriction might seem curious but this is what US law requires.)
 */

/* This encryption code is a direct transcription of the algorithm from
   Roger Schlafly, described by Phil Katz in the file appnote.txt. This
   file is distributed with the PKZIP program (even in the version without
   encryption capabilities).
 */

#include "zip.h"

#ifndef SKIP_TIME_H   /* time.h already included in UnZip version */
#  include <time.h>
#endif /* !SKIP_TIME_H */

#if ((defined(MSDOS) || defined(__MSDOS__) || defined(WIN32)) && !defined(__GO32__))
#  include <process.h>
#else
   int  getpid OF((void));
#endif

#ifndef __GNUC__
void srand  OF((unsigned int));
#endif
int  rand   OF((void));

int  decrypt_byte OF((void));
void update_keys OF((int c));
void init_keys OF((char *passwd));
int  zencode OF((int c));
int  zdecode OF((int c));

local ulg keys[3]; /* keys defining the pseudo random sequence */

/***********************************************************************
 * Return the next byte in the pseudo-random sequence
 */
int decrypt_byte()
{
   ush temp;

   temp = (ush)keys[2] | 2;
   return (int)(((ush)(temp * (temp ^ 1)) >> 8) & 0xff);
}

/***********************************************************************
 * Update the encryption keys with the next byte of plain text
 */
void update_keys(c)
    int c;                  /* byte of plain text */
{
    keys[0] = crc32(keys[0], c);
    keys[1] += keys[0] & 0xff;
    keys[1] = keys[1] * 134775813L + 1;
    keys[2] = crc32(keys[2], (int)(keys[1] >> 24));
}


/***********************************************************************
 * Initialize the encryption keys and the random header according to
 * the given password.
 */
void init_keys(passwd)
    char *passwd;             /* password string to modify keys with */
{
    keys[0] = 305419896L;
    keys[1] = 591751049L;
    keys[2] = 878082192L;
    while (*passwd != '\0') {
        update_keys((int)*passwd);
        passwd++;
    }
}

/***********************************************************************
 * Return the encoded value of the byte c of the plain text
 */
int zencode(c)
    int c;
{
    int temp = decrypt_byte();

    update_keys(c);
    return temp ^ c;
}

/***********************************************************************
 * Return the decoded value of the byte c of the cypher text
 */
int zdecode(c)
    int c;
{
    int temp = c ^ decrypt_byte();

    update_keys(temp);
    return temp;
}


/***********************************************************************
 * Write encryption header to file zfile using the password passwd
 * and the cyclic redundancy check crc. Note: we use the rand() library
 * function, which is quite bad on many systems. But since we only
 * need 10 values, this is not important.
 */
void crypthead(passwd, crc, zfile)
    char *passwd;             /* password string */
    ulg crc;                  /* crc of file being encrypted */
    FILE *zfile;              /* where to write header to */
{
    int n = 10;               /* size of random header */

    init_keys(passwd);

    /* Encrypt random header (last two bytes is high word of crc) */

    srand((unsigned int)time(NULL) ^ getpid()); /* initialize generator */
    while (n--) {
        putc(zencode(rand() >> 7), zfile);
    }
    putc(zencode((int)(crc >> 16) & 0xff), zfile);
    putc(zencode((int)(crc >> 24)), zfile);
}


#ifdef UTIL

/***********************************************************************
 * Encrypt the zip entry described by z from file source to file dest
 * using the password passwd.  Return an error code in the ZE_ class.
 */
int zipcloak(z, source, dest, passwd)
    struct zlist far *z;    /* zip entry to encrypt */
    FILE *source, *dest;    /* source and destination files */
    char *passwd;           /* password string */
{
    int c;                  /* input byte */
    int res;                /* result code */
    ulg n;                  /* holds offset and counts size */
    ush flag;               /* previous flags */

    /* Set encrypted bit, clear extended local header bit and write local
       header to output file */
    if ((n = ftell(dest)) == -1L) return ZE_TEMP;
    z->off = n;
    flag = z->flg;
    z->flg |= 1,  z->flg &= ~8;
    z->lflg |= 1, z->lflg &= ~8;
    z->siz += 12;
    if ((res = putlocal(z, dest)) != ZE_OK) return res;

    /* Initialize keys with password and write random header */
    crypthead(passwd, z->crc, dest);

    /* Skip local header in input file */
    if (fseek(source, (long)(4 + LOCHEAD + (ulg)z->nam + (ulg)z->ext),
              SEEK_CUR)) {
        return ferror(source) ? ZE_READ : ZE_EOF;
    }

    /* Encrypt data */
    for (n = z->siz - 12; n; n--) {
        if ((c = getc(source)) == EOF) {
            return ferror(source) ? ZE_READ : ZE_EOF;
        }
        putc(zencode(c), dest);
    }
    /* Skip extended local header in input file if there is one */
    if ((flag & 8) != 0 && fseek(source, 16L, SEEK_CUR)) {
        return ferror(source) ? ZE_READ : ZE_EOF;
    }
    if (fflush(dest) == EOF) return ZE_TEMP;
    return ZE_OK;
}

/***********************************************************************
 * Decrypt the zip entry described by z from file source to file dest
 * using the password passwd.  Return an error code in the ZE_ class.
 */
int zipbare(z, source, dest, passwd)
    struct zlist far *z;  /* zip entry to encrypt */
    FILE *source, *dest;  /* source and destination files */
    char *passwd;         /* password string */
{
    int c0, c1;           /* last two input bytes */
    ulg offset;           /* used for file offsets */
    ulg size;             /* size of input data */
    int r;                /* size of encryption header */
    int res;              /* return code */
    ush flag;             /* previous flags */

    /* Save position and skip local header in input file */
    if ((offset = ftell(source)) == -1L ||
        fseek(source, (long)(4 + LOCHEAD + (ulg)z->nam + (ulg)z->ext),
              SEEK_CUR)) {
        return ferror(source) ? ZE_READ : ZE_EOF;
    }
    /* Initialize keys with password */
    init_keys(passwd);

    /* Decrypt encryption header, save last two bytes */
    c1 = 0;
    for (r = 12; r; r--) {
        c0 = c1;
        if ((c1 = zdecode(getc(source))) == EOF) {
            return ferror(source) ? ZE_READ : ZE_EOF;
        }
    }

    /* If last two bytes of header don't match crc (or file time in the
       case of an extended local header), back up and just copy */
    if ((ush)(c0 | (c1<<8)) !=
        (z->flg & 8 ? (ush) z->tim & 0xffff : (ush)(z->crc >> 16))) {
        if (fseek(source, offset, SEEK_SET)) {
            return ferror(source) ? ZE_READ : ZE_EOF;
        }
        if ((res = zipcopy(z, source, dest)) != ZE_OK) return res;
        return ZE_MISS;
    }

    /* Clear encrypted bit and local header bit, and write local header to
       output file */
    if ((offset = ftell(dest)) == -1L) return ZE_TEMP;
    z->off = offset;
    flag = z->flg;
    z->flg &= ~9;
    z->lflg &= ~9;
    z->siz -= 12;
    if ((res = putlocal(z, dest)) != ZE_OK) return res;

    /* Decrypt data */
    for (size = z->siz; size; size--) {
        if ((c1 = getc(source)) == EOF) {
            return ferror(source) ? ZE_READ : ZE_EOF;
        }
        putc(zdecode(c1), dest);
    }
    /* Skip extended local header in input file if there is one */
    if ((flag & 8) != 0 && fseek(source, 16L, SEEK_CUR)) {
        return ferror(source) ? ZE_READ : ZE_EOF;
    }
    if (fflush(dest) == EOF) return ZE_TEMP;

    return ZE_OK;
}


#else /* !UTIL */

/***********************************************************************
 * If requested, encrypt the data in buf, and in any case call fwrite()
 * with the arguments to zfwrite().  Return what fwrite() returns.
 */
unsigned zfwrite(buf, item_size, nb, f)
    voidp *buf;                /* data buffer */
    extent item_size;          /* size of each item in bytes */
    extent nb;                 /* number of items */
    FILE *f;                   /* file to write to */
{
    if (key != (char *)NULL) { /* key is the global password pointer */
        ulg size;              /* buffer size */
        char *p = (char*)buf;  /* steps through buffer */

        /* Encrypt data in buffer */
        for (size = item_size*(ulg)nb; size != 0; p++, size--) {
            *p = (char)zencode(*p);
        }
    }
    /* Write the buffer out */
    return fwrite(buf, item_size, nb, f);
}

/***********************************************************************
 * Encrypt the byte c and then do a putc().  The macro zputc defined in
 * crypt.h checks keys and calls zfputc if needed.  Return what putc()
 * returns. This function is only provided for compatibility with zip 1.0.
 */
int zfputc(c, f)
    int c;                  /* character to write */
    FILE *f;                /* file to write it to */
{
    return putc(zencode(c), f);
}

#endif /* ?UTIL */
