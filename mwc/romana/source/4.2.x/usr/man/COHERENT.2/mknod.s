mmkknnoodd() -- System Call (libc)

Create a special file
#iinncclluuddee <ssyyss/iinnoo.hh>
#iinncclluuddee <ssyyss/ssttaatt.hh>
mmkknnoodd(_n_a_m_e, _m_o_d_e, _a_d_d_r)
cchhaarr *_n_a_m_e; iinntt _m_o_d_e, _a_d_d_r;

mmkknnoodd() is the COHERENT system call that creates a special file.  A _s_p_e_c_i_a_l
_f_i_l_e is one through which a device is accessed, or a named pipe.

_m_o_d_e gives the type of special file to be created.  It can be set to IIFFBBLLKK,
for  a  block-special device,  such  as  a disk  driver;  to  IIFFCCHHRR, for  a
character-special device,  such as  a serial-port  driver; to IIFFDDIIRR,  for a
directory; or  to IIFFPPIIPPEE, for a named pipe.   _m_o_d_e also contains permission
mode bits.

_a_d_d_r_e_s_s  is a  parameter interpreted  by the driver;  it might  specify the
channel of a multiplexor or the  unit number of a drive.  Note that this is
not used with named pipes.

If all goes  well, mmkknnoodd() returns zero.  If an  error occurs, it returns a
negative value and sets eerrrrnnoo to an appropriate value.

_S_e_e _A_l_s_o
lliibbcc, ddeevviiccee ddrriivveerrss, nnaammeedd ppiippee, ppiippee()

_N_o_t_e_s
Only the superuser rroooott can use mmkknnoodd(). This is a security feature.
