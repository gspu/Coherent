.TH canon.h "" "" "Header file"
.PC "Portable layout of binary data"
.B
#include <canon.h>
#include <sys/types.h>
.R
.PP
The routines declared in
.B canon.h
were designed to aid the transfer of binary information among
different implementations of \*(CO.
For technical reasons, these routines are slated to be dropped from a
future release of \*(CO.
Their use is strongly discouraged.
.\"The layout of binary data varies among machines.
.\"For example, the byte order of a 16-bit word on the PDP-11 is
.\"low-byte.high-byte, whereas on the Z8000 it is high-byte.low-byte.
.\".PP
.\"To ensure that file systems can be ported among machines
.\"with differing byte orders, \*(PN uses a canonical layout of binary data.
.\"(The word ``canonical'' in this context means, ``of or conforming
.\"to a general rule''.)
.\"Data not in primary memory (e.g., on disk or communications line)
.\"must conform to \*(CO's canonical layout.
.\".PP
.\"To insulate programs from the details of the difference
.\"between the `natural' and canonical layouts,
.\"the \*(CO system provides a set of procedures
.\"to convert from one layout to another.
.\"They are as follows:
.\".DS
.\".ta 1.5i
.\".tc . 0.1i
.\"\fBcanshort()\fR	Convert a \fBshort\fR
.\"\fBcanint()\fR	Convert an \fBint\fR
.\"\fBcanlong()\fR	Convert a \fBlong\fR
.\"\fBcanvaddr()\fR	Convert \fBvaddr_t\fR
.\"\fBcansize()\fR	Convert \fBfsize_t\fR
.\"\fBcandaddr()\fR	Convert \fBdaddr_t\fR
.\"\fBcantime()\fR	Convert \fBtime_t\fR
.\"\fBcandev()\fR	Convert \fBdev_t\fR
.\"\fBcanino()\fR	Convert \fBino_t\fR
.\".tc
.\".DE
.\".PP
.\"Each procedure takes an lvalue
.\"of the indicated type, converts it in place, and returns nothing.
.\"The argument should not have side-effects.
.\"Each procedure is its own inverse.
.\"Several procedures are designed for elements of file systems.
.\".if \nX=0 \{\
.\".PP
.\"The file formats that contain canonical binary data
.\"and the commands that deal with them are as follows:
.\".DS
.\".ta 0.5i 1.5i
.\"	\fIFormat	Commands\fR
.\".B
.\"	ar.h	ar, ld, ranlib
.\"	dir.h	ls, tar
.\"	l.out.h	as, cc, db, ld, nm, size, strip
.\".DE
.\".PP
.\"Any program that manipulates binary
.\"data within files must perform canonical conversion immediately upon input
.\"and immediately before output.
.\"The following fragment of the source code to the command
.\".B df
.\"should be instructive:
.\".DM
.\"#include <stdio.h>
.\"#include <canon.h>
.\"#include <filsys.h>
.\"char    superb[BSIZE];
.\".DE
.\".DM
.\"        .
.\"        .
.\"        .
.\".DE
.\".DM
.\"df(fs)
.\"char *fs;
.\"{
.\"        register struct filsys *sbp = &superb;
.\"        FILE *fp;
.\"        daddr_t nfree;
.\".DE
.\".DM
.\"        if ((fp = fopen(fs, "r")) == NULL) {
.\"                perror(fs);
.\"                return (1);
.\"        }
.\".DE
.\".DM
.\"        fseek(fp, (long)BSIZE, 0);
.\"        if (fread(superb, sizeof superb, 1, fp) != 1) {
.\"                fprintf(stderr, "%s: read error\\n", fs);
.\"                return (1);
.\"        }
.\".DE
.\".DM
.\"        candaddr(sbp->s_tfree);
.\"        candaddr(sbp->s_fsize);
.\"        canshort(sbp->s_isize);
.\"        nfree = sbp->s_tfree;
.\".DE
.\".DM
.\"        if (nfree > sbp->s_fsize-sbp->s_isize  ||  nfree < 0) {
.\"                fprintf(stderr, "%s: bad free count\en", fs);
.\"                return (1);
.\"        }
.\".DE
.\".DM
.\"        printf("%s: %ld\en", fs, nfree);
.\"        fclose(fp);
.\"        return (0);
.\"}
.\".DE \}
.\".SH Files
.\".B <canon.h>
.SH "See Also"
.Xr ar.h, ar.h
.Xr "byte ordering," byte_orde
.Xr "header files," header_fi
