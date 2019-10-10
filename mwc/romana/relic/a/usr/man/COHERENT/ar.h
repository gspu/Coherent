

ar.h                       Header File                       ar.h




Format for archive files

#include <ar.h>

An archive is a file that  has been built from a number of files.
Archives are  maintained by the ar  command.  Usually, an archive
is a library of object files used by the linker ld.

The header ar.h describes the format of an archive.  All archives
start with a magic number  ARMAG, which identifies the file as an
archive.   The members  of the archive  follow the  magic number,
each preceded by the structure ar_hdr:


#define DIRSIZ  14               /* from <dir.h> */
#define ARMAG   0177535          /* magic number */



struct ar_hdr {
        char    ar_name[DIRSIZ]; /* member name */
        time_t  ar_date;         /* time inserted */
        short   ar_gid;          /* group owner */
        short   ar_uid;          /* user owner */
        short   ar_mode;         /* file mode */
        size_t  ar_size;         /* file size */
};


The structure at the  head of each member is immediately followed
by ar_size bytes, which are the data of the file.

To enhance  the performance of ld, the  command ranlib provides a
random library facility.  ranlib produces archives that contain a
special entry named _ _.SYMDEF at the beginning.

All integer members of the structure (everything but ar_name) are
in canonical form to  ease portability.  See canon.h for more in-
formation.

***** See Also *****

ar, canon.h, header files, ld, ranlib













COHERENT Lexicon                                           Page 1


