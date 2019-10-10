aacccctt.hh -- Header File

Format for process-accounting file
#iinncclluuddee <aacccctt.hh>

_P_r_o_c_e_s_s  _a_c_c_o_u_n_t_i_n_g is  a feature  of  the COHERENT  system that  allows it
record what  processes each user executes and how  long each process takes.
These data can be used to track how much each user uses the system.

The  function aacccctt()  turns  process accounting  off or  on.  When  process
accounting  has been  turned on,  the COHERENT  system writes  raw process-
accounting information into  an accounting file as each process terminates.
Each  entry  in  the  accounting  file,  normally  /uussrr/aaddmm/aacccctt,  has  the
following form, as defined in the header file aacccctt.hh:

struct acct {
     char    ac_comm[10];
     comp_t  ac_utime;
     comp_t  ac_stime;
     comp_t  ac_etime;
     time_t  ac_btime;
     short   ac_uid;
     short   ac_gid;
     short   ac_mem;
     comp_t  ac_io;
     dev_t   ac_tty;
     char    ac_flag;
};

/* Bits from ac_flag */
#define AFORK  01   /* has done fork, but not exec */
#define ASU    02   /* has used superuser privileges */

Every time  a process  calls eexxeecc(), the  contents of aacc_ccoommmm  are replaced
with the  first ten characters of  the file name.  The  fields aacc_uuttiimmee and
aacc_ssttiimmee represent the CPU time used in the user program and in the system,
respectively.   aacc_eettiimmee  represents the  elapsed  time  since the  process
started running,  whereas aacc_bbttiimmee  is the  time the process  started.  The
effective  user id  and effective  group id are  aacc_uuiidd and  aacc_ggiidd. aacc_mmeemm
gives the average  memory usage of the process.  aacc_iioo  gives the number of
blocks  of input-output.   aacc_ttttyy gives  the controlling  typewriter device
major and minor numbers.

For  some  of  the  above  times,  the  aacccctt  structure  uses  the  special
representation ccoommpp_tt, defined in the header file ttyyppeess.hh. It is a floating
point  representation with  three bits  of base-8 exponent  and 13  bits of
fraction, so it fits in a sshhoorrtt integer.

_S_e_e _A_l_s_o
aacccctt(), aaccccttoonn, hheeaaddeerr ffiilleess, ssaa
