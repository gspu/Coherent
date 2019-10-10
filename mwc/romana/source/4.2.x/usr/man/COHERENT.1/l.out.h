ll.oouutt.hh -- Header File

Format for COHERENT 286 objects
#iinncclluuddee <ll.oouutt.hh>

The  header  file  ll.oouutt.hh describes  the  ll.oouutt  object  format, which  is
produced by the compiler, assembler, and the linker under COHERENT 286.

The assembler  outputs an unlinked object module, which  must be bound with
any  required  libraries  (leaving no  unresolved  symbols)  to produce  an
executable file,  or _l_o_a_d _m_o_d_u_l_e.  A call to  one of the  eexxeecc routines can
then execute the load module directly.

The  link  module  begins  with  a  header, which  gives  global  and  size
information about each segment.   Segments of the indicated size follow the
header  in a  fixed  order.  The  header  file ll.oouutt.hh  defines the  header
structure as follows:

struct  ldheader {
        short l_magic;
        short l_flag;
        short l_machine;
        unsigned short l_entry;
        fsize_t l_ssize[NLSEG];
};

ll_mmaaggiicc  is the  magic  number that  identifies  a link  module; it  always
contains LL_MMAAGGIICC. ll_ffllaagg contains  flags indicating the type of the object.
ll_mmaacchhiinnee  is  the processor  identifier,  as defined  in  the header  file
mmttyyppee.hh. ll_ttbbaassee  is the start  of the text segment.   ll_eennttrryy contains the
machine address where execution of the module commences.  ll_ssssiizzee gives the
size of each segment.

_F_i_l_e_s
ll.oouutt -- Default load module name
<ll.oouutt.hh> -- Define format of COHERENT 286 objects
<mmttyyppee.hh> -- Machine identifiers

_S_e_e _A_l_s_o
aass, cccc, ccoorree, eexxeecc, lldd, lliibbcc, mmttyyppee, nnmm

_N_o_t_e_s
COHERENT 386 uses the common object file format (COFF) for its executables.
See the Lexicon entry for ccooffff.hh for information on this format.
