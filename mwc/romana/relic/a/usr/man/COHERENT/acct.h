

acct.h                     Header File                     acct.h




Format for process-accounting file

#include <acct.h>

Process  accounting is  a  feature of  the  COHERENT system  that
allows it  record what processes each user  executes and how long
each process  takes.  These  data can be  used to track  how much
each user uses the system.

The  function acct  turns  process accounting  off  or on.   When
process accounting has been turned on, the COHERENT system writes
raw  process-accounting information  into an  accounting  file as
each process terminates.  Each entry in the accounting file, nor-
mally /usr/adm/acct,  has the following  form, as defined  in the
header file aacccctt.hh:


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


Every  time a  process  performs an  exec call,  the contents  of
ac_comm are  replaced with the  first ten characters  of the file
name.  The  fields ac_utime and  ac_stime represent the  CPU time
used  in  the  user  program  and  in the  system,  respectively.
ac_etime represents  the elapsed  time since the  process started
running, whereas  ac_btime is the time  the process started.  The
effective user  id and effective group id  are ac_uid and ac_gid.
ac_mem  gives the  average memory  usage  of the  process.  ac_io
gives  the number  of blocks of  input-output.  ac_tty  gives the
controlling typewriter device major and minor numbers.

For some of the above  times, the acct structure uses the special
representation comp_t, defined in the header file types.h.  It is
a  floating point  representation with three  bits of  base-8 ex-
ponent and 13 bits of fraction, so it fits in a short integer.



COHERENT Lexicon                                           Page 1




acct.h                     Header File                     acct.h



***** See Also *****

acct(), accton, header files, sa






















































COHERENT Lexicon                                           Page 2


