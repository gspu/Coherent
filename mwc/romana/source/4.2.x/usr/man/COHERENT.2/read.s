rreeaadd() -- System Call (libc)

Read from a file
#iinncclluuddee <uunniissttdd.hh>
iinntt rreeaadd(_f_d, _b_u_f_f_e_r, _n)
iinntt _f_d; cchhaarr *_b_u_f_f_e_r; iinntt _n;

rreeaadd() reads up  to _n bytes of data from  the file descriptor _f_d and writes
them into  _b_u_f_f_e_r. The amount of  data actually read may  be less than that
requested if rreeaadd() detects EEOOFF. The data are read beginning at the current
seek position  in the  file, which  was set by  the most  recently executed
rreeaadd() or llsseeeekk() routine.  rreeaadd()  advances the seek pointer by the number
of characters read.

_E_x_a_m_p_l_e
For an example of how to use this function, see the entry for ooppeenn().

_S_e_e _A_l_s_o
lliibbcc, uunniissttdd.hh
POSIX Standard, section 6.4.1

_D_i_a_g_n_o_s_t_i_c_s
With a successful call, rreeaadd() returns the number of bytes read; thus, zero
bytes signals the end of the  file.  It returns -1 if an error occurs, such
as bad file descriptor, bad _b_u_f_f_e_r address, or physical read error.

_N_o_t_e_s
rreeaadd()  is a  low-level call  that  passes data  directly to  COHERENT.  It
should not be intermixed  with high-level calls, such as ffrreeaadd(), ffwwrriittee(),
or ffooppeenn().
