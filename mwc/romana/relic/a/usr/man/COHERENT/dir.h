

dir.h                      Header File                      dir.h




Directory format

#include <dir.h>

A  COHERENT directory  is exactly like  an ordinary  file, except
that a  user's process may write on it  only through system calls
such as creat,  link, mknod, or unlink.  The system distinguishes
directories from other types of files by the mode word S_IFDIR in
the i-node.  (For more information on i-nodes, see stat).

Every directory  is an array  of entries of  the following struc-
ture, as defined in the header file dir.h:


#define                  DIRSIZ 14

struct direct {
     ino_t d_ino;        /* i-number */
     char d_name[DIRSIZ];/* name */
};


Any entry in which d_ino has a value of zero is unused.

The command  mkdir creates a directory,  with the convention that
its first  two entries are `.'  and `..'.  The name  `.' is self-
referential -- a link to  the directory itself.  The name `..' is
a link  to the parent directory.  Because  the root directory has
no parent, its `..' is a link to itself.

The d_ino entry of the  directory structure is stored in the file
system in canonical form, as described in canon.h.

***** See Also *****

canon.h, header files, mkdir, stat()




















COHERENT Lexicon                                           Page 1


