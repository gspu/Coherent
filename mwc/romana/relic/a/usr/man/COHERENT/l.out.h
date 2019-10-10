

l.out.h                    Header File                    l.out.h




Object file format

#include <l.out.h>

The header  file l.out.h describes  the format for  the output of
compilers, assemblers, and the linker.

The assembler  outputs an unlinked  object module, which  must be
bound with any required libraries (leaving no unresolved symbols)
to produce an executable file, or  load module.  A call to one of
the exec routines can then execute the load module directly.

The link module begins with a header, which gives global and size
information about  each segment.  Segments of  the indicated size
follow  the header  in a  fixed order.   The header  file l.out.h
defines the header structure for the Z8000 and M68000 as follows:


struct  ldheader {
        short     l_magic;
        short     l_flag;
        short     l_machine;
        short     l_tbase;
        size_t    l_ssize[NLSEG];
        long      l_entry;
};


It describes  the header structure for  the i8086, i8088, i80286,
and PDP-11 as follows:


struct  ldheader {
        int            l_magic;
        int            l_flag;
        int            l_machine;
        vaddr_t        l_entry;
        size_t         l_ssize[NLSEG];
};


l_magic is the magic number that identifies a link module; it al-
ways contains L_MAGIC.  l_flag contains flags indicating the type
of the object.  l_machine is the processor identifier, as defined
in the  header file  mtype.h.  l_tbase is  the start of  the text
segment.  l_entry contains the machine address where execution of
the module commences.  l_ssize gives the size of each segment.

***** Files *****

ll.oouutt -- Default load module name
<l.out.h>
<mmttyyppee.hh> -- Machine identifiers



COHERENT Lexicon                                           Page 1




l.out.h                    Header File                    l.out.h



***** See Also *****

as, cc, COHERENT system calls, core, exec, ld, mtype, nm

***** Notes *****

In  the early  releases  of COHERENT,  the  header structure  was
defined only as shown above  for i8086.  It was changed to handle
32-bit addresses.   In the  future, the header  structure defined
above for Z8000 and  M68000 machines will be implemented on i8086
and i80286 systems as well.














































COHERENT Lexicon                                           Page 2


