mmkkddiirr() -- System Call (libc)

Create a directory
#iinncclluuddee <ssyyss/ttyyppeess.hh>
#iinncclluuddee <ssyyss/ssttaatt.hh>
iinntt mmkkddiirr(_p_a_t_h, _m_o_d_e)
cchhaarr *_p_a_t_h;
iinntt _m_o_d_e;

The COHERENT  system call mmkkddiirr()  creates the directory  specified by _p_a_t_h
and gives it the file mode specified by _m_o_d_e. If _p_a_t_h is relative (that is,
it  doesn't begin  with  a `/'  character), mmkkddiirr()  creates the  directory
relates to the current directory of the process that calls mmkkddiirr(). If _p_a_t_h
is absolute  (i.e., begins with  a `/'), ppaatthh  specifies a directory  to be
created  relative to  the  root directory  for this  process.  See  Lexicon
article cchhrroooott()  for details.  If  _p_a_t_h specifies more  than one directory
level, all  parent names  specified must exist,  must be accessible  by the
calling process, and actually must be directories.

Argument _m_o_d_e is formed  by logically OR'ing permissions constants found in
header file  <ssyyss/ssttaatt.hh>. These constants begin with  SS_ and determine the
permissions for the directory.  See the Lexicon article ssttaatt.hh for details.

If  the directory  is successfully  created, mmkkddiirr()  returns zero.   If an
error occurs, mmkkddiirr() returns -1 and sets eerrrrnnoo to an appropriate value.

_S_e_e _A_l_s_o
lliibbcc, mmkkddiirr, rrmmddiirr, rrmmddiirr(), ssttaatt.hh
POSIX Standard, section 5.4.1
