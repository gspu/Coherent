cchhrroooott() -- System Call (libc)

Change the root directory
#iinncclluuddee <uunniissttdd.hh>
iinntt cchhrroooott(_p_a_t_h)
cchhaarr *_p_a_t_h;

The  COHERENT  system  call cchhrroooott()  changes  the  current process's  root
directory  to  that  specified  by  _p_a_t_h.  Once the  cchhrroooott()  system  call
completes, all references to absolute directories (i.e., ones starting with
`/')  will actually  refer to  directory pointed  to by  _p_a_t_h. It  does not
change the current directory.

cchhrroooott() is  often used to  add extra security  to special or  public login
accounts.

_S_e_e _A_l_s_o
cchhrroooott, lliibbcc

_N_o_t_e_s
The process  that invokes cchhrroooott()  must be running as  the superuser rroooott,
and _p_a_t_h must name a valid directory.
