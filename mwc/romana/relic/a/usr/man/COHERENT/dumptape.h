

dumptape.h                 Header File                 dumptape.h




Define data structures used on dump tapes

#include <dumptape.h>

dumptape.h  defines the  data structures used  on dump  tapes.  A
dump tape  begins with  a header  record.  This contains  the at-
tributes of  the tape.  The remainder of the  tape is filled with
arrays of  dumpdata records.  The  map comes first,  then all the
directories, then all the files.

***** See Also *****

dump, header files











































COHERENT Lexicon                                           Page 1


