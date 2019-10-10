

canon.h                    Header file                    canon.h




Portable layout of binary data

#include <canon.h>
#include <sys/types.h>

The layout  of binary data  varies among machines.   For example,
the byte  order of a 16-bit word on  the PDP-11 is low-byte.high-
byte, whereas on the Z8000 it is high-byte.low-byte.

To ensure  that file  systems can  be ported among  machines with
differing byte orders, COHERENT uses a canonical layout of binary
data.   (The word  ``canonical'' in this  context means,  ``of or
conforming  to a  general  rule''.) Data  not  in primary  memory
(e.g., on disk or communications line) must conform to COHERENT's
canonical layout.

To insulate  programs from the details  of the difference between
the `natural' and canonical layouts, the COHERENT system provides
a set of procedures to  convert from one layout to another.  They
are as follows:


          ccaannsshhoorrtt()Convert a sshhoorrtt
          ccaanniinntt()  Convert an iinntt
          ccaannlloonngg() Convert a lloonngg
          ccaannvvaaddddrr()Convert vvaaddddrr_tt
          ccaannssiizzee() Convert ffssiizzee_tt
          ccaannddaaddddrr()Convert ddaaddddrr_tt
          ccaannttiimmee() Convert ttiimmee_tt
          ccaannddeevv()  Convert ddeevv_tt
          ccaanniinnoo()  Convert iinnoo_tt


Each procedure takes an lvalue of the indicated type, converts it
in  place, and  returns nothing.   The  argument should  not have
side-effects.  Each procedure is its own inverse.  Several proce-
dures are designed for elements of file systems.

The file formats that  contain canonical binary data and the com-
mands that deal with them are as follows:


          _F_o_r_m_a_t    _C_o_m_m_a_n_d_s

          ar.h      ar, ld, ranlib
          dir.h     ls, tar
          l.out.h   as, cc, db, ld, nm, size, strip


Any program  that manipulates binary data  within files must per-
form canonical conversion  immediately upon input and immediately
before output.  The following  fragment of the source code to the
command df should be instructive:



COHERENT Lexicon                                           Page 1




canon.h                    Header file                    canon.h




#include <stdio.h>
#include <canon.h>
#include <filsys.h>
char    superb[BSIZE];



        .
        .
        .



df(fs)
char *fs;
{
        register struct filsys *sbp = &superb;
        FILE *fp;
        daddr_t nfree;



        if ((fp = fopen(fs, "r")) == NULL) {
                perror(fs);
                return (1);
        }



        fseek(fp, (long)BSIZE, 0);
        if (fread(superb, sizeof superb, 1, fp) != 1) {
                fprintf(stderr, "%s: read error\n", fs);
                return (1);
        }



        candaddr(sbp->s_tfree);
        candaddr(sbp->s_fsize);
        canshort(sbp->s_isize);
        nfree = sbp->s_tfree;



        if (nfree > sbp->s_fsize-sbp->s_isize  ||  nfree < 0) {
                fprintf(stderr, "%s: bad free count\n", fs);
                return (1);
        }








COHERENT Lexicon                                           Page 2




canon.h                    Header file                    canon.h



        printf("%s: %ld\n", fs, nfree);
        fclose(fp);
        return (0);
}


***** Files *****

<canon.h>

***** See Also *****

ar.h,  byte ordering,  candaddr(), candev(),  canino(), canint(),
canlong(), canshort(),  cansize(), cantime(),  canvaddr(), dir.h,
l.out.h, header files










































COHERENT Lexicon                                           Page 3


