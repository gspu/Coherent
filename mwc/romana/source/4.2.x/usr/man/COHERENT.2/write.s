wwrriittee() -- System Call (libc)

Write to a file
#iinncclluuddee <uunniissttdd.hh>
iinntt wwrriittee(_f_d, _b_u_f_f_e_r, _n)
iinntt _f_d; cchhaarr *_b_u_f_f_e_r; iinntt _n;

wwrriittee() writes _n bytes of data,  beginning at address _b_u_f_f_e_r, into the file
associated with the file descriptor _f_d. Writing begins at the current write
position, as  set by the  last call to  either wwrriittee() or  llsseeeekk(). wwrriittee()
advances  the position  of the  file  pointer by  the number  of characters
written.

_E_x_a_m_p_l_e
For an example of how to use this function, see the entry for ooppeenn().

_S_e_e _A_l_s_o
lliibbcc, uunniissttdd.hh
POSIX Standard, section 6.4.2

_D_i_a_g_n_o_s_t_i_c_s
wwrriittee()  returns  -1 if  an  error occurred  before  the wwrriittee()  operation
commenced,  such as  a bad  file descriptor _f_d  or invalid  _b_u_f_f_e_r pointer.
Otherwise, it returns the number of bytes written.  It should be considered
an error if this number is not the same as _n.

_N_o_t_e_s
wwrriittee()  is a  low-level call  that passes data  directly to  COHERENT.  It
should  not be  mixed  with high-level  calls, such  as ffrreeaadd(),  ffwwrriittee(),
ffppuuttss(), or ffpprriinnttff().
