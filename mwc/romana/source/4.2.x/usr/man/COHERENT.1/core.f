ccoorree -- System Administration

Format of a core-dump file
#iinncclluuddee <ssyyss/ccoorree.hh>

When  a   process  terminates  abnormally   because  a  it   encounters  an
unrecoverable  error  or  receives  an  asynchronous  signal  from  another
process, COHERENT tries to write a  copy of its image in memory into a file
called ccoorree. You can examine this file with the debugger ddbb and other tools
to try to determine what went wrong.

The structure cchh_iinnffoo appears at the  head of a ccoorree file.  The header file
ccoorree.hh defines it as follows:

    struct  ch_info {
        unsigned short ch_magic;
        unsigned int ch_info_len;
    };

Field cchh_mmaaggiicc  is always  set to  the constant CCOORREE_MMAAGGIICC.  This ``magic''
value signifies  to COHERENT that  this is a core  file.  Field cchh_iinnffoo_lleenn
gives a count of information bytes  in the core file, including the cchh_iinnffoo
structure itself.

If the value of cchh_iinnffoo_lleenn exceeds the size of the cchh_iinnffoo structure, this
indicates that  data follow the  cchh_iinnffoo structure.  These  data follow the
cchh_iinnffoo structure,  and are in  the form of a  ccoorree_pprroocc structure.  Header
file <ssyyss/ccoorree.hh> defines this structure as follows:

    struct core_proc {
        gregset_t cp_registers;
        int cp_signal_number;
        struct _fpstate cp_floating_point;
        dregset_t cp_debug_registers;
    };

This substitutes  for a dump of  the uu area, whose  information is reserved
for the kernel alone.

This is  followed by an image  of each process segment.   The data for each
segment consists of  the following: a header, which is  a structure of type
ccoorree_sseegg; ccss_ppaatthhlleenn bytes of data that give the path name of the file from
the segment data originated; and ccss_dduummppeedd bytes of core-image data.

ccoorree.hh defines strucutre ccoorree_sseegg as follows:

     struct core_seg {
          size_t cs_pathlen;            /* length of path name */
          off_t cs_dumped;              /* dumped size in bytes */
          caddr_t cs_base;              /* virtual base address */
          off_t cs_size;                /* full size in bytes */
          unsigned long cs_reserved[8];
     };

The order  of the segments is  the text segment first (if  it is present --
usually it  is omitted), followed by  the data segment, and  then the stack
segment.  The  contents of the text segment can  usually be identified from
the program being debugged.  The patchable kernel variable DDUUMMPP_TTEEXXTT allows
the  COHERENT kernel  to  dump text  segments  as well  as  data and  stack
segments.

Patchable  kernel variable  DDUUMMPP_LLIIMM  sets the  maximum size  of a  segment
within a  ccoorree file.  The  system uses this  limit to keep  keep core files
from getting out of hand.

_S_e_e _A_l_s_o
AAddmmiinniisstteerriinngg CCOOHHEERREENNTT, ccoorree.hh, ssiiggnnaall(), wwaaiitt()

_D_i_a_g_n_o_s_t_i_c_s
COHERENT will not write ccoorree if  that file already exists as a non-ordinary
file or if  there is more than one link to  it.  The 0200 bit in the status
returned to the parent process by wwaaiitt() indicates a successful dump.

For  a list  of signals  that automatically  trigger a  core dump,  see the
Lexicon entry for ssiiggnnaall().
