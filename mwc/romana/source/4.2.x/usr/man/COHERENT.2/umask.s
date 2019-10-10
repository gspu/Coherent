uummaasskk() -- System Call (libc)

Set file-creation mask
#iinncclluuddee <ssyyss/ssttaatt.hh>
iinntt uummaasskk(_m_a_s_k)
iinntt _m_a_s_k;

uummaasskk()  allows  a  process to  restrict  the  mode  of  files it  creates.
Commands that  create files should specify the  maximum reasonable mode.  A
parent  (e.g. the  shell sshh)  usually calls uummaasskk()  to restrict  access to
files created by subsequent commands.

_m_a_s_k should be constructed from any of the permission bits found by cchhmmoodd()
(the low-order nine bits).  When a file is created with ccrreeaatt() or mmkknnoodd(),
every bit set in the _m_a_s_k is zeroed in _m_o_d_e; thus, bits set in _m_a_s_k specify
permissions that will be denied.

uummaasskk() returns the old value of the file-creation mask.

_E_x_a_m_p_l_e
Setting _m_a_s_k  to octal 022 (i.e.,  000 010 010) causes  a file created with
mode octal 0666 to actually have permissions of

    rw- r-- r--

Setting _m_a_s_k  to zero (i.e., 000  000 000) causes a  file created with mode
octal 0666 to actually have permissions of

    rw- rw- rw-

_S_e_e _A_l_s_o
ccrreeaatt(), lliibbcc, mmkknnoodd(), sshh, ssttaatt.hh
POSIX Standard, section 5.3.3

_N_o_t_e_s
A file's  default permission  cannot be set  to execute, regardless  of the
value of _m_a_s_k.
