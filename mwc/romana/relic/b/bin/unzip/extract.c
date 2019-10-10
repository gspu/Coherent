/*---------------------------------------------------------------------------

  extract.c

  This file contains the high-level routines ("driver routines") for extrac-
  ting and testing zipfile members.  It calls the low-level routines in files
  explode.c, inflate.c, unreduce.c and unshrink.c.

  ---------------------------------------------------------------------------*/


#include "unzip.h"
#ifdef  MSWIN
#  include "wizunzip.h"
#  include "replace.h"
#endif /* MSWIN */


/************************************/
/*  Extract Local Prototypes, etc.  */
/************************************/

static int store_info __((void));
static int extract_or_test_member __((void));
#ifdef CRYPT
   static int decrypt_member __((void));
   static int testp __((byte *hdr));
#endif

static byte *mem_i_buffer;
static byte *mem_o_buffer;
static ULONG mem_i_size, mem_i_offset;
static ULONG mem_o_size, mem_o_offset;

static char *VersionMsg =
  " skipping: %-22s  need %s compat. v%u.%u (can do v%u.%u)\n";
static char *ComprMsg =
  " skipping: %-22s  compression method %d\n";
static char *FilNamMsg =
  "%s:  bad filename length (%s)\n";
static char *ExtFieldMsg =
  "%s:  bad extra field length (%s)\n";
static char *OffsetMsg =
  "file #%d:  bad zipfile offset (%s)\n";





/**************************************/
/*  Function extract_or_test_files()  */
/**************************************/

int extract_or_test_files()    /* return PK-type error code */
{
    char **fnamev;
    byte *cd_inptr;
    int cd_incnt, error, error_in_archive=0;
    int renamed, query, len, filnum=(-1), blknum=0;
#ifdef OS2
    extern int longname;  /* from os2unzip.c */
#endif
    UWORD i, j, members_remaining, num_skipped=0, num_bad_pwd=0;
    longint cd_bufstart, bufstart, inbuf_offset, request;
    min_info info[DIR_BLKSIZ];


/*---------------------------------------------------------------------------
    The basic idea of this function is as follows.  Since the central di-
    rectory lies at the end of the zipfile and the member files lie at the
    beginning or middle or wherever, it is not very desirable to simply
    read a central directory entry, jump to the member and extract it, and
    then jump back to the central directory.  In the case of a large zipfile
    this would lead to a whole lot of disk-grinding, especially if each mem-
    ber file is small.  Instead, we read from the central directory the per-
    tinent information for a block of files, then go extract/test the whole
    block.  Thus this routine contains two small(er) loops within a very
    large outer loop:  the first of the small ones reads a block of files
    from the central directory; the second extracts or tests each file; and
    the outer one loops over blocks.  There's some file-pointer positioning
    stuff in between, but that's about it.  Btw, it's because of this jump-
    ing around that we can afford to be lenient if an error occurs in one of
    the member files:  we should still be able to go find the other members,
    since we know the offset of each from the beginning of the zipfile.

    Begin main loop over blocks of member files.  We know the entire central
    directory is on this disk:  we would not have any of this information un-
    less the end-of-central-directory record was on this disk, and we would
    not have gotten to this routine unless this is also the disk on which
    the central directory starts.  In practice, this had better be the ONLY
    disk in the archive, but maybe someday we'll add multi-disk support.
  ---------------------------------------------------------------------------*/

    pInfo = info;
    members_remaining = ecrec.total_entries_central_dir;

    while (members_remaining) {
        j = 0;

        /*
         * Loop through files in central directory, storing offsets, file
         * attributes, and case-conversion flags until block size is reached.
         */

        while (members_remaining && (j < DIR_BLKSIZ)) {
            --members_remaining;
            pInfo = &info[j];

            if (readbuf(sig, 4) <= 0) {
                error_in_archive = 51;  /* 51:  unexpected EOF */
                members_remaining = 0;  /* ...so no more left to do */
                break;
            }
            if (strncmp(sig, central_hdr_sig, 4)) {  /* just to make sure */
                fprintf(stderr, CentSigMsg, j);  /* sig not found */
                fprintf(stderr, ReportMsg);   /* check binary transfers */
                error_in_archive = 3;   /* 3:  error in zipfile */
                members_remaining = 0;  /* ...so no more left to do */
                break;
            }
            /* process_cdir_file_hdr() sets pInfo->hostnum, pInfo->lcflag */
            if ((error = process_cdir_file_hdr()) != 0) {
                error_in_archive = error;   /* only 51 (EOF) defined */
                members_remaining = 0;  /* ...so no more left to do */
                break;
            }
            if ((error = do_string(crec.filename_length, FILENAME)) != 0) {
                if (error > error_in_archive)
                    error_in_archive = error;
                if (error > 1) {  /* fatal:  no more left to do */
                    fprintf(stderr, FilNamMsg, filename, "central");
                    members_remaining = 0;
                    break;
                }
            }
            if ((error = do_string(crec.extra_field_length, EXTRA_FIELD)) != 0)
            {
                if (error > error_in_archive)
                    error_in_archive = error;
                if (error > 1) {  /* fatal */
                    fprintf(stderr, ExtFieldMsg, filename, "central");
                    members_remaining = 0;
                    break;
                }
            }
            if ((error = do_string(crec.file_comment_length, SKIP)) != 0) {
                if (error > error_in_archive)
                    error_in_archive = error;
                if (error > 1) {  /* fatal */
                    fprintf(stderr, "\n%s:  bad file comment length\n",
                            filename);
                    members_remaining = 0;
                    break;
                }
            }
            if (process_all_files) {
                if (store_info())
                    ++num_skipped;
                else
                    ++j;  /* file is OK: save info[] and continue with next */
            } else {
                fnamev = fnv;   /* don't destroy permanent filename pointer */
                for (--fnamev; *++fnamev;)
                    if (match(filename, *fnamev)) {
                        if (store_info())
                            ++num_skipped;
                        else
                            ++j;   /* file is OK */
                        break;  /* found match for filename, so stop looping */
                    } /* end if (match), for-loop (fnamev) */
            } /* end if (process_all_files) */

        } /* end while-loop (adding files to current block) */

        /* save position in central directory so can come back later */
        cd_bufstart = cur_zipfile_bufstart;
        cd_inptr = inptr;
        cd_incnt = incnt;

    /*-----------------------------------------------------------------------
        Second loop:  process files in current block, extracting or testing
        each one.
      -----------------------------------------------------------------------*/

        for (i = 0; i < j; ++i) {
            filnum = i + blknum*DIR_BLKSIZ;
            pInfo = &info[i];
            /*
             * if the target position is not within the current input buffer
             * (either haven't yet read far enough, or (maybe) skipping back-
             * ward) skip to the target position and reset readbuf().
             */
            /* LSEEK(pInfo->offset):  */
            request = pInfo->offset + extra_bytes;
            inbuf_offset = request % INBUFSIZ;
            bufstart = request - inbuf_offset;

            if (request < 0) {
                fprintf(stderr, SeekMsg, ReportMsg);
                error_in_archive = 3;       /* 3:  severe error in zipfile, */
                continue;                   /*  but can still go on */
            } else if (bufstart != cur_zipfile_bufstart) {
                cur_zipfile_bufstart = lseek(zipfd, bufstart, SEEK_SET);
                if ((incnt = read(zipfd,(char *)inbuf,INBUFSIZ)) <= 0) {
                    fprintf(stderr, OffsetMsg, filnum, "lseek");
                    error_in_archive = 3;   /* 3:  error in zipfile, but */
                    continue;               /*  can still do next file   */
                }
                inptr = inbuf + (int)inbuf_offset;
                incnt -= (int)inbuf_offset;
            } else {
                incnt += (inptr-inbuf) - (int)inbuf_offset;
                inptr = inbuf + (int)inbuf_offset;
            }

            /* should be in proper position now, so check for sig */
            if (readbuf(sig, 4) <= 0) {  /* bad offset */
                fprintf(stderr, OffsetMsg, filnum, "EOF");
                error_in_archive = 3;    /* 3:  error in zipfile */
                continue;       /* but can still try next one */
            }
            if (strncmp(sig, local_hdr_sig, 4)) {
                fprintf(stderr, OffsetMsg, filnum,
                        "can't find local header sig");   /* bad offset */
                error_in_archive = 3;
                continue;
            }
            if ((error = process_local_file_hdr()) != 0) {
                fprintf(stderr, "\nfile #%d:  bad local header\n", filnum);
                error_in_archive = error;       /* only 51 (EOF) defined */
                continue;       /* can still try next one */
            }
            if ((error = do_string(lrec.filename_length, FILENAME)) != 0) {
                if (error > error_in_archive)
                    error_in_archive = error;
                if (error > 1) {
                    fprintf(stderr, FilNamMsg, filename, "local");
                    continue;   /* go on to next one */
                }
            }
            if (extra_field != (byte *)NULL)
                free(extra_field);
            extra_field = (byte *)NULL;
            if ((error = do_string(lrec.extra_field_length, EXTRA_FIELD)) != 0)
            {
                if (error > error_in_archive)
                    error_in_archive = error;
                if (error > 1) {
                    fprintf(stderr, ExtFieldMsg, filename, "local");
                    continue;   /* go on */
                }
            }

            /*
             * just about to extract file:  if extracting to disk, check if
             * already exists, and if so, take appropriate action according to
             * fflag/uflag/overwrite_all/etc. (we couldn't do this in upper
             * loop because we don't store the possibly renamed filename[] in
             * info[])
             */
            if (!tflag && !cflag) {
                renamed = FALSE;   /* user hasn't renamed output file yet */
#ifdef OS2
                longname = FALSE;  /* no long name has yet been stored */
#endif

startover:
                query = FALSE;
#ifdef MACOS
                macflag = (pInfo->hostnum == MAC_);
#endif
                /* mapname can create dirs if not freshening or if renamed */
                if ((error = mapname(!fflag || renamed)) > 1) {    /* skip */
                    if ((error > 2) && (error_in_archive < 2))
                        error_in_archive = 2;   /* (weak) error in zipfile */
                    continue;   /* go on to next file */
                }

                switch (check_for_newer(filename)) {
                    case DOES_NOT_EXIST:
                        if (fflag && !renamed)  /* don't skip if just renamed */
                            continue;   /* freshen (no new files):  skip */
                        break;
                    case EXISTS_AND_OLDER:
                        if (overwrite_none)
                            continue;   /* never overwrite:  skip file */
                        if (!overwrite_all && !force_flag)
                            query = TRUE;
                        break;
                    case EXISTS_AND_NEWER:             /* (or equal) */
                        if (overwrite_none || (uflag && !renamed))
                            continue;  /* skip if update/freshen & orig name */
                        if (!overwrite_all && !force_flag)
                            query = TRUE;
                        break;
                }
/*#ifndef VMS*/ /* VMS creates higher version number instead of overwriting
                 * (will have to modify for VMS-style names with specific
                 *  version numbers:  just check V_flag?  don't use stat?) */
                if (query) {
#ifdef MSWIN
                    FARPROC lpfnprocReplace;
                    int ReplaceDlgRetVal;   /* replace dialog return value */

                    ShowCursor(FALSE);      /* turn off cursor */
                    SetCursor(hSaveCursor); /* restore the cursor */
                    lpfnprocReplace = MakeProcInstance(ReplaceProc, hInst);
                    ReplaceDlgRetVal = DialogBoxParam(hInst, "Replace",
                      hWndMain, lpfnprocReplace, (DWORD)(LPSTR)filename);
                    FreeProcInstance(lpfnprocReplace);
                    hSaveCursor = SetCursor(hHourGlass);
                    ShowCursor(TRUE);
                    switch (ReplaceDlgRetVal) {
                        case IDM_REPLACE_RENAME:
                            renamed = TRUE;
                            goto startover;   /* sorry for a goto */
                        case IDM_REPLACE_YES:
                            break;
                        case IDM_REPLACE_ALL:
                            overwrite_all = TRUE;
                            overwrite_none = FALSE;  /* just to make sure */
                            break;
                        case IDM_REPLACE_NONE:
                            overwrite_none = TRUE;
                            overwrite_all = FALSE;  /* make sure */
                            force_flag = FALSE;     /* ditto */
                            /* FALL THROUGH, skip */
                        case IDM_REPLACE_NO:
                            continue;
                    }
#else /* !MSWIN */
reprompt:
                    fprintf(stderr,
                      "replace %s? [y]es, [n]o, [A]ll, [N]one, [r]ename: ",
                      filename);
                    FFLUSH   /* for Amiga and Mac MPW */
                    fgets(answerbuf, 9, stdin);
                    switch (*answerbuf) {
                        case 'A':   /* dangerous option:  force caps */
                            overwrite_all = TRUE;
                            overwrite_none = FALSE;  /* just to make sure */
                            break;
                        case 'r':
                        case 'R':
                            do {
                                fprintf(stderr, "new name: ");
                                FFLUSH   /* for AMIGA and Mac MPW */
                                fgets(filename, FILNAMSIZ, stdin);
                                /* usually get \n here:  better check for it */
                                len = strlen(filename);
                                if (filename[len-1] == '\n')
                                    filename[--len] = 0;
                            } while (len == 0);
                            renamed = TRUE;
                            goto startover;   /* sorry for a goto */
                        case 'y':
                        case 'Y':
                            break;
                        case 'N':
                            overwrite_none = TRUE;
                            overwrite_all = FALSE;  /* make sure */
                            force_flag = FALSE;     /* ditto */
                            /* FALL THROUGH, skip */
                        case 'n':
                            continue;   /* skip file */
                        default:
                            fprintf(stderr, "error:  invalid response [%c]\n",
                              *answerbuf);   /* warn the user */
                            goto reprompt;   /* why not another goto? */
                    } /* end switch (*answerbuf) */
#endif /* ?MSWIN */
                } /* end if (query) */
/*#endif*/ /* !VMS */
            } /* end if (extracting to disk) */

#ifdef CRYPT
            if (pInfo->encrypted && ((error = decrypt_member()) != 0)) {
                if (error == 10) {
                    if (error > error_in_archive)
                        error_in_archive = error;
                    fprintf(stderr,
                      " skipping: %-22s  unable to get password\n", filename);
                } else {  /* (error == 1) */
                    fprintf(stderr,
                      " skipping: %-22s  incorrect password\n", filename);
                    ++num_bad_pwd;
                }
                continue;   /* go on to next file */
            }
#endif /* CRYPT */
            disk_full = 0;
            if ((error = extract_or_test_member()) != 0) {
                if (error > error_in_archive)
                    error_in_archive = error;       /* ...and keep going */
                if (disk_full > 1)
                    return error_in_archive;        /* (unless disk full) */
            }
        } /* end for-loop (i:  files in current block) */


        /*
         * Jump back to where we were in the central directory, then go and do
         * the next batch of files.
         */

        cur_zipfile_bufstart = lseek(zipfd, cd_bufstart, SEEK_SET);
        read(zipfd, (char *)inbuf, INBUFSIZ);  /* were there b4 ==> no error */
        inptr = cd_inptr;
        incnt = cd_incnt;
        ++blknum;

#ifdef TEST
        printf("\ncd_bufstart = %ld (%.8lXh)\n", cd_bufstart, cd_bufstart);
        printf("cur_zipfile_bufstart = %ld (%.8lXh)\n", cur_zipfile_bufstart,
          cur_zipfile_bufstart);
        printf("inptr-inbuf = %d\n", inptr-inbuf);
        printf("incnt = %d\n\n", incnt);
#endif

    } /* end while-loop (blocks of files in central directory) */

/*---------------------------------------------------------------------------
    Double-check that we're back at the end-of-central-directory record, and
    print quick summary of results, if we were just testing the archive.  We
    send the summary to stdout so that people doing the testing in the back-
    ground and redirecting to a file can just do a "tail" on the output file.
  ---------------------------------------------------------------------------*/

    readbuf(sig, 4);
    if (strncmp(sig, end_central_sig, 4)) {     /* just to make sure again */
        fprintf(stderr, EndSigMsg);  /* didn't find end-of-central-dir sig */
        fprintf(stderr, ReportMsg);  /* check binary transfers */
        if (!error_in_archive)       /* don't overwrite stronger error */
            error_in_archive = 1;    /* 1:  warning error */
    }
    if (tflag && (quietflg == 1)) {
        int num=filnum+1 - num_bad_pwd;

        if (error_in_archive)
            printf("At least one error was detected in %s.\n", zipfn);
        else if (num == 0)
            printf("Caution:  zero files tested in %s.\n", zipfn);
        else if (process_all_files && (num_skipped+num_bad_pwd == 0))
            printf("No errors detected in %s.\n", zipfn);
        else
            printf("No errors detected in %s for the %d file%s tested.\n",
              zipfn, num, (num==1)? "":"s");
        if (num_skipped > 0)
            printf("%d file%s skipped because of unsupported compression or\
 encoding.\n",
              num_skipped, (num_skipped==1)? "":"s");
#ifdef CRYPT
        if (num_bad_pwd > 0)
            printf("%d file%s skipped because of incorrect password.\n",
              num_bad_pwd, (num_bad_pwd==1)? "":"s");
#endif /* CRYPT */
    }
    if ((num_skipped > 0) && !error_in_archive)   /* files not tested or  */
        error_in_archive = 1;                     /*  extracted:  warning */
#ifdef CRYPT
    if ((num_bad_pwd > 0) && !error_in_archive)   /* files not tested or  */
        error_in_archive = 1;                     /*  extracted:  warning */
#endif /* CRYPT */

    return (error_in_archive);

} /* end function extract_or_test_files() */





/***************************/
/*  Function store_info()  */
/***************************/

static int store_info()   /* return 1 if skipping, 0 if OK */
{
    ULONG tmp;

#define UNKN_COMPR \
   (crec.compression_method>IMPLODED && crec.compression_method!=DEFLATED)
#if 0  /* old */
#  define UNKN_COMPR   (crec.compression_method>IMPLODED)
#endif


/*---------------------------------------------------------------------------
    Check central directory info for version/compatibility requirements.
  ---------------------------------------------------------------------------*/

    pInfo->encrypted = crec.general_purpose_bit_flag & 1;    /* bit field */
    pInfo->ExtLocHdr = (crec.general_purpose_bit_flag & 8) == 8;  /* bit */
    pInfo->text = crec.internal_file_attributes & 1;         /* bit field */
    pInfo->crc = crec.crc32;
    pInfo->compr_size = crec.compressed_size;

    if (crec.version_needed_to_extract[1] == VMS_) {
        if (crec.version_needed_to_extract[0] > VMS_VERSION) {
            fprintf(stderr, VersionMsg, filename, "VMS",
              crec.version_needed_to_extract[0] / 10,
              crec.version_needed_to_extract[0] % 10,
              VMS_VERSION / 10, VMS_VERSION % 10);
            return 1;
        }
#ifndef VMS   /* won't be able to use extra field, but still have data */
        else if (!tflag && !force_flag) {  /* if forcing, extract regardless */
            fprintf(stderr,
              "\n%s:  stored in VMS format.  Extract anyway? (y/n) ",
              filename);
            FFLUSH   /* for Amiga and Mac MPW */
            fgets(answerbuf, 9, stdin);
            if ((*answerbuf != 'y') && (*answerbuf != 'Y'))
                return 1;
        }
#endif /* !VMS */
    /* usual file type:  don't need VMS to extract */
    } else if (crec.version_needed_to_extract[0] > UNZIP_VERSION) {
        fprintf(stderr, VersionMsg, filename, "PK",
          crec.version_needed_to_extract[0] / 10,
          crec.version_needed_to_extract[0] % 10,
          UNZIP_VERSION / 10, UNZIP_VERSION % 10);
        return 1;
    }

    if UNKN_COMPR {
        fprintf(stderr, ComprMsg, filename, crec.compression_method);
        return 1;
    }
#ifndef CRYPT
    if (pInfo->encrypted) {
        fprintf(stderr, " skipping: %-22s  encrypted (not supported)\n",
          filename);
        return 1;
    }
#endif /* !CRYPT */

/*---------------------------------------------------------------------------
    Store some central-directory information (encryption, file attributes,
    offsets) for later use.
  ---------------------------------------------------------------------------*/

    tmp = crec.external_file_attributes;

    pInfo->dos_attr = 32;   /* set archive bit:  file is not backed up */
    switch (pInfo->hostnum) {
        case UNIX_:
        case VMS_:
            pInfo->unix_attr = (unsigned) (tmp >> 16);
            break;
        case DOS_OS2_FAT_:
        case OS2_HPFS_:
            pInfo->dos_attr = (unsigned) tmp;
            tmp = (!(tmp & 1)) << 1;   /* read-only bit */
            pInfo->unix_attr = (unsigned) (0444 | (tmp<<6) | (tmp<<3) | tmp);
#ifdef UNIX
            umask( (int)(tmp=umask(0)) );
            pInfo->unix_attr &= ~tmp;
#endif
            break;
        case MAC_:
            pInfo->unix_attr = (unsigned) (tmp & 1);   /* read-only bit */
            break;
        default:
            pInfo->unix_attr = 0666;
            break;
    } /* end switch (host-OS-created-by) */

    pInfo->offset = (longint) crec.relative_offset_local_header;
    return 0;

} /* end function store_info() */





/***************************************/
/*  Function extract_or_test_member()  */
/***************************************/

static int extract_or_test_member()    /* return PK-type error code */
{
#ifdef S_IFLNK
    int symlnk=FALSE;
#endif /* S_IFLNK */
    int r, error=0;
    UWORD b;



/*---------------------------------------------------------------------------
    Initialize variables, buffers, etc.
  ---------------------------------------------------------------------------*/

    bits_left = 0;
    bitbuf = 0L;
    outpos = 0L;
    outcnt = 0;
    outptr = outbuf;
    zipeof = 0;
    crc32val = 0xFFFFFFFFL;

#ifdef S_IFLNK
    if ((pInfo->unix_attr & S_IFMT) == S_IFLNK  &&  (pInfo->hostnum == UNIX_)
        && !tflag && !cflag)
        symlnk = TRUE;
#endif /* S_IFLNK */

    memset(outbuf, 0xaa, OUTBUFSIZ);
#if (!defined(DOS_OS2) || defined(MSWIN))
    if (aflag)                  /* if we have a scratchpad, clear it out */
#ifdef MSWIN
        _fmemset(outout, 0xaa, OUTBUFSIZ);
#else /* !MSWIN */
        memset(outout, 0xaa, OUTBUFSIZ);
#endif /* ?MSWIN */
#endif /* !DOS_OS2 || MSWIN */

    if (tflag) {
        if (!quietflg) {
            fprintf(stdout, "  Testing: %-22s ", filename);
            fflush(stdout);
        }
    } else {
        if (cflag) {            /* output to stdout (copy of it) */
#if (defined(MACOS) || defined(AMIGA))
            outfd = 1;
#else /* !(MACOS || AMIGA) */
            outfd = dup(1);     /* GRR: change this to #define for Mac/Amiga */
#endif /* ?(MACOS || AMIGA) */
#ifdef DOS_OS2
            if (!aflag)
                setmode(outfd, O_BINARY);
#endif /* DOS_OS2 */
#ifdef VMS
            if (create_output_file())   /* VMS version required for stdout! */
                return 50;      /* 50:  disk full (?) */
#endif
        } else
#ifdef S_IFLNK
        if (!symlnk)    /* symlink() takes care of file creation */
#endif /* !S_IFLNK */
        {
            if (create_output_file())
                return 50;      /* 50:  disk full (?) */
        }
    } /* endif (!tflag) */

/*---------------------------------------------------------------------------
    Unpack the file.
  ---------------------------------------------------------------------------*/

    switch (lrec.compression_method) {

    case STORED:
        if (!tflag && (quietflg < 2)) {
            fprintf(stdout, " Extracting: %-22s ", filename);
            if (cflag)
                fprintf(stdout, "\n");
            fflush(stdout);
        }
#ifdef S_IFLNK
        /*
         * If file came from Unix and is a symbolic link and we are extracting
         * to disk, allocate a storage area, put the data in it, and create the
         * link.  Since we know it's a symbolic link to start with, shouldn't
         * have to worry about overflowing unsigned ints with unsigned longs.
         * (This doesn't do anything for compressed symlinks, but that can be
         * added later...it also doesn't set the time or permissions of the
         * link, but does anyone really care?)
         */
        if (symlnk) {
#if (defined(MTS) || defined(MACOS))
            fprintf(stdout, "\n  warning:  symbolic link ignored\n");
            error = 1;          /* 1:  warning error */
#else /* !(MTS || MACOS) */
            char *orig = (char *)malloc((unsigned)lrec.uncompressed_size+1);
            char *p = orig;

            while (ReadByte(&b))
                *p++ = b;
            *p = 0;   /* terminate string */
            UpdateCRC((unsigned char *)orig, p-orig);
            if (symlink(orig, filename))
                if ((errno == EEXIST) && overwrite_all) {  /* OK to overwrite */
                    unlink(filename);
                    if (symlink(orig, filename))
                        perror("symlink error");
                } else
                    perror("symlink error");
            free(orig);
#endif /* ?(MTS || MACOS) */
        } else
#endif /* S_IFLNK */
        while (ReadByte(&b) && !disk_full)
            OUTB(b)
        break;

    case SHRUNK:
        if (!tflag && (quietflg < 2)) {
            fprintf(stdout, "UnShrinking: %-22s ", filename);
            if (cflag)
                fprintf(stdout, "\n");
            fflush(stdout);
        }
#ifdef S_IFLNK   /* !!! This code needs to be added to unShrink, etc. !!! */
        if (symlnk) {
            fprintf(stdout, "\n  warning:  symbolic link ignored\n");
            error = 1;          /* 1:  warning error */
        }
#endif /* S_IFLNK */
        unShrink();
        break;

    case REDUCED1:
    case REDUCED2:
    case REDUCED3:
    case REDUCED4:
        if (!tflag && (quietflg < 2)) {
            fprintf(stdout, "  Expanding: %-22s ", filename);
            if (cflag)
                fprintf(stdout, "\n");
            fflush(stdout);
        }
#ifdef S_IFLNK   /* !!! This code needs to be added to unShrink, etc. !!! */
        if (symlnk) {
            fprintf(stdout, "\n  warning:  symbolic link ignored\n");
            error = 1;          /* 1:  warning error */
        }
#endif /* S_IFLNK */
        unReduce();
        break;

    case IMPLODED:
        if (!tflag && (quietflg < 2)) {
            fprintf(stdout, "  Exploding: %-22s ", filename);
            if (cflag)
                fprintf(stdout, "\n");
            fflush(stdout);
        }
#ifdef S_IFLNK   /* !!! This code needs to be added to unShrink, etc. !!! */
        if (symlnk) {
            fprintf(stdout, "\n  warning:  symbolic link ignored\n");
            error = 1;          /* 1:  warning error */
        }
#endif /* S_IFLNK */
        if (((r = explode()) != 0) && (r != 5)) {   /* ignore 5 if seekable */
            if ((tflag && quietflg) || (!tflag && (quietflg > 1)))
                fprintf(stderr, "  error:  %s%s\n", r == 3?
                  "not enough memory to explode " :
                  "invalid compressed (imploded) data for ", filename);
            else
                fprintf(stderr, "\n  error:  %s\n", r == 3?
                  "not enough memory for explode operation" :
                  "invalid compressed data for explode format");
            error = (r == 3)? 5 : 2;
        }
        break;

    case DEFLATED:
        if (!tflag && (quietflg < 2)) {
            fprintf(stdout, "  Inflating: %-22s ", filename);
            if (cflag)
                fprintf(stdout, "\n");
            fflush(stdout);
        }
#ifdef S_IFLNK   /* !!! This code needs to be added to unShrink, etc. !!! */
        if (symlnk) {
            fprintf(stdout, "\n  warning:  symbolic link ignored\n");
            error = 1;          /* 1:  warning error */
        }
#endif /* S_IFLNK */
        if ((r = inflate()) != 0) {
            if ((tflag && quietflg) || (!tflag && (quietflg > 1)))
                fprintf(stderr, "  error:  %s%s\n", r == 3?
                  "not enough memory to inflate " :
                  "invalid compressed (deflated) data for ", filename);
            else
                fprintf(stderr, "\n  error:  %s\n", r == 3?
                  "not enough memory for inflate operation" :
                  "invalid compressed data for inflate format");
            error = (r == 3)? 5 : 2;
        }
        break;

    default:   /* should never get to this point */
        fprintf(stderr, "%s:  unknown compression method\n", filename);
        /* close and delete file before return? */
        return 1;               /* 1:  warning error */

    } /* end switch (compression method) */

    if (disk_full) {            /* set by FlushOutput()/OUTB() macro */
        if (disk_full > 1)
            return 50;          /* 50:  disk full */
        error = 1;              /* 1:  warning error */
    }

/*---------------------------------------------------------------------------
    Write the last partial buffer, if any; set the file date and time; and
    close the file (not necessarily in that order).  Then make sure CRC came
    out OK and print result.
  ---------------------------------------------------------------------------*/

#ifdef S_IFLNK
    if (!symlnk) {
#endif /* S_IFLNK */
    if (!disk_full && FlushOutput())
        if (disk_full > 1)
            return 50;          /* 50:  disk full */
        else {                  /* disk_full == 1 */
            fprintf(stderr, "%s:  probably corrupt\n", filename);
            error = 1;          /* 1:  warning error */
        }

    if (!tflag)
#ifdef VMS
        CloseOutputFile();
#else /* !VMS */
#ifdef MTS                      /* MTS can't set file time */
        close(outfd);
#else /* !MTS */
        set_file_time_and_close();
#endif /* ?MTS */
#endif /* ?VMS */

#ifdef S_IFLNK
    } /* endif (!symlnk) */
#endif /* S_IFLNK */

    if (error > 1)   /* don't print redundant CRC error if error already */
        return error;

    /* logical-AND crc32val for 64-bit machines */
    if ((crc32val = ((~crc32val) & 0xFFFFFFFFL)) != lrec.crc32) {
        /* if quietflg is set, we haven't output the filename yet:  do it */
        if (quietflg)
            printf("%-22s: ", filename);
        fprintf(stdout, " Bad CRC %08lx  (should be %08lx)\n", crc32val,
                lrec.crc32);
        error = 1;              /* 1:  warning error */
    } else if (tflag) {
        if (!quietflg)
            fprintf(stdout, " OK\n");
    } else {
        if ((quietflg < 2) && !error)
            fprintf(stdout, "\n");
    }

    return error;

}       /* end function extract_or_test_member() */





#ifdef CRYPT

/*******************************/
/*  Function decrypt_member()  */
/*******************************/

static int decrypt_member()   /* return 10 if out of memory or can't get */
{                             /*  tty; 1 if password bad; 0 if password OK */
    UWORD b;
    int n, r;
    static int nopwd=FALSE;
    char *m, *prompt;
    byte h[12];


    /* get header once (turn off "encrypted" flag temporarily so we don't
     * try to decrypt the same data twice) */
    pInfo->encrypted = FALSE;
    for (n = 0; n < 12; n++) {
        ReadByte(&b);
        h[n] = (byte) b;
    }
    pInfo->encrypted = TRUE;

    /* if have key already, test it; else allocate memory for it */
    if (key) {
        if (!testp(h))
            return 0;      /* existing password OK (else prompt for new) */
        else if (nopwd)
            return 1;      /* user indicated no more prompting */
    } else if ((key = (char *)malloc(PWLEN+1)) == (char *)NULL)
        return 10;

    if ((prompt = (char *)malloc(FILNAMSIZ+15)) != (char *)NULL) {
        sprintf(prompt, "%s password: ", filename);
        m = prompt;
    } else
        m = "Enter password: ";

    /* try a few keys */
    for (r = 0;  r < 3;  ++r) {
        m = getp(m, key, PWLEN+1);
        if (prompt != (char *)NULL) {
            free(prompt);
            prompt = (char *)NULL;
        }
        if (m == (char *)NULL)
            return 10;
        if (!testp(h))
            return 0;
        if (*key == '\0') {
            nopwd = TRUE;
            return 1;
        }
        m = "password incorrect--reenter: ";
    }
    return 1;
}





/**********************/
/*  Function testp()  */
/**********************/

static int testp(h)   /* return -1 if bad password; 0 if OK */
    byte *h;
{
    UWORD b;
    int n, t;
    byte *p;

    /* set keys */
    init_keys(key);

    /* check password */
    for (n = 0; n < 12; n++)
        b = DECRYPT(h[n]);

#ifdef CRYPT_DEBUG
    printf("   lrec.crc = %08lx  crec.crc = %08lx  pInfo->ExtLocHdr = %s\n",
      lrec.crc32, pInfo->crc, pInfo->ExtLocHdr? "true":"false");
    printf("   incnt = %d  unzip offset into zipfile = %ld\n", incnt,
      cur_zipfile_bufstart+(inptr-inbuf));
    printf("   b = %02x  (crc >> 24) = %02x  (lrec.time >> 8) = %02x\n",
      b, (UWORD)(lrec.crc32 >> 24), (lrec.last_mod_file_time >> 8));
#endif /* CRYPT_DEBUG */

    /* same test as in zipbare() in crypt.c (now check only one byte): */
    if (b != (pInfo->ExtLocHdr? lrec.last_mod_file_time >> 8 :
        (UWORD)(lrec.crc32 >> 24)))
        return -1;  /* bad */

    /* password OK:  decrypt current buffer contents before leaving */
    for (n = (longint)incnt > csize ? (int)csize : incnt, p = inptr; n--; p++)
        *p = (byte) DECRYPT(*p);
    return 0;       /* OK */

} /* end function testp() */

#endif /* CRYPT */





/*******************************/
/*  Function ReadMemoryByte()  */
/*******************************/

int ReadMemoryByte(x)   /* return PK-type error code */
    UWORD *x;
{
    if (mem_i_offset < mem_i_size) {
        *x = (UWORD) mem_i_buffer[mem_i_offset++];
        return 8;
    } else
        return 0;
}





/****************************/
/*  Function FlushMemory()  */
/****************************/

int FlushMemory()   /* return PK-type error code */
{
    if (outcnt == 0)
        return 0;

    if (mem_o_offset + outcnt <= mem_o_size) {
        memcpy((char *)(mem_o_buffer+(UWORD)mem_o_offset), (char *)outbuf,
          outcnt);
        mem_o_offset += outcnt;
        return 0;
    } else
        return 50;
}





/***************************/
/*  Function memextract()  */   /* extract compressed extra field block */
/***************************/

int memextract(tgt, tgtsize, src, srcsize)  /* return 0 if success, 1 if not */
    byte *tgt, *src;
    ULONG tgtsize, srcsize;
{
    UWORD method, error = 0;
    ULONG crc, oldcrc;
    int r;

    method = makeword(src);
    crc = makelong(src+2);

    mem_i_buffer = src + 2 + 4;      /* method and crc */
    mem_i_size   = srcsize - 2 - 4;
    mem_i_offset = 0;
  
    mem_o_buffer = tgt;
    mem_o_size   = tgtsize;
    mem_o_offset = 0;

    mem_mode = 1;

    bits_left = 0;
    bitbuf = 0L;
    outpos = 0L;
    outcnt = 0;
    outptr = outbuf;
    zipeof = 0;

    switch (method) {
        case STORED:
            memcpy(tgt, src + 2 + 4, (extent) (srcsize - 2 - 4));
            break;
        case DEFLATED:
            if ((r = inflate()) != 0) {
                fprintf(stderr, "error:  %s\n", r == 3 ?
                  "not enough memory for inflate operation" :
                  "invalid compressed data for the inflate format");
                error = (r == 3)? 5 : 2;
            }
            FlushOutput();
            break;
        default:
            fprintf(stderr,
              "warning:  unsupported extra field compression type--skipping\n");
            error = 1;   /* GRR:  this should be passed on up via SetEAs() */
            break;
    }

    mem_mode = 0;

    if (!error) {
        oldcrc = crc32val;
        crc32val = 0xFFFFFFFFL;
        UpdateCRC((unsigned char *) mem_o_buffer, (int) mem_o_size);
        crc32val = (~crc32val) & 0xFFFFFFFFL;

        if (crc32val != crc) {
            printf("(Bad extra field CRC %08lx, should be %08lx)\n", crc32val,
              crc);
            error = 1;
        }
        crc32val = oldcrc; /* grrr ... this ugly kludge should be fixed */
    }

    return error;
}
