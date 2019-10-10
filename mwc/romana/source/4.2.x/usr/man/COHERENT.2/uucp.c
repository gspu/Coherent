uuuuccpp -- Command

Spool files for transmission to other systems
uuuuccpp [ -ccCCddffmmrr ] [-nn_u_s_e_r] [-xx_a_c_t_i_v_i_t_y] _s_o_u_r_c_e ... _d_e_s_t

The command uuuuccpp  spools every file _s_o_u_r_c_e for copying  to _d_e_s_t. _s_o_u_r_c_e and
_d_e_s_t can specify a remote system.

uuuuccpp recognizes the following options:

-cc Do not  copy the source file  into spool directory; rather  use the file
   itself.

-CC Copy  the source  file into  spool directory;  same as  -pp. This  is the
   default.

-dd Create directories as required on the destination system.

-ff Do not make intermediate directories for the file copy.

-II _f_i_l_e
   Use _f_i_l_e as configuration file.

-jj Report job's process identifier.

-mm Send mail to requester when the file is sent.

-nn_u_s_e_r
   Notify _u_s_e_r  on destination system when the file  is received.  _u_s_e_r can
   contain a path.  Note that  _u_s_e_r is relative to the destination machine,
   not to originating machine  or to any intervening machine.  For example,
   consider the command:

       uucp -nlepanto!fred myfile joe!/tmp

   Here, you  are copying mmyyffiillee  from your machine into  directory /ttmmpp on
   machine jjooee, and sending  notification to user  ffrreedd on machine lleeppaannttoo.
   If, however,  machine jjooee does not know how  to address machine lleeppaannttoo,
   then ffrreedd will never be notified of the transfer.

-pp Copy  the source  file into  spool directory;  same as  -CC. This  is the
   default.

-RR Copy directories recursively.

-rr Spool transfer request, but do not initiate uuuucciiccoo.

-ss _f_i_l_e
   Write status upon completion of job into _f_i_l_e.

-uu _u_s_e_r
   Set user name to _u_s_e_r.

-WW Do not add the current directory to file names on the remote system.

-xx_a_c_t_i_v_i_t_y
   Log a given _a_c_t_i_v_i_t_y. These logs  can help you debug problems with UUCP.
   uuuuccpp recognizes the following activities:

   aabbnnoorrmmaall
        Log abnormal events that occur while spooling a file for copying.

   ccoonnffiigg
        Log  problems   that  arise   with  reading  or   interpreting  the
        configuration files ddiiaall, ppoorrtt, and ssyyss.

   eexxeeccuuttee
        Log each step uuuuccpp takes as it executes.

   ssppoooollddiirr
        Log   activity   that   involved   the  UUCP   spooling   directory
        /uussrr/ssppooooll/uuuuccpp and its subdirectories.

_E_x_a_m_p_l_e_s
The first example copies file ffoooo to directory /bbaarr on system ggeeoorrggee:

    uucp foo george!/bar

The next example copies file /ffoooo from system ggeeoorrggee into directory /ttmmpp on
your system:

    uucp george!/foo /tmp

The next example copies file /ffoooo from system ggeeoorrggee into file or directory
/bbaarr on system iivvaann:

    uucp george!/foo ivan!/bar

Note that  this assumes your  system can talk  to both ggeeoorrggee  and iivvaann and
that your system has permission to  read file /ffoooo on system ggeeoorrggee as well
as to write file /bbaarr on system iivvaann.

The next example downloads files /ffoooo and /bbaarr from remote systems iivvaann and
ggeeoorrggee into directory /ttmmpp on your system:

    uucp ivan!/foo george!/bar /tmp

The last example downloads file ffoooo from system iivvaann via system ggeeoorrggee:

    uucp george!ivan!foo

For  an  example  of using  the  command  ffiinndd  with  uuuuccpp to  spool  files
automatically, see the entry for ffiinndd.

_F_i_l_e_s
/uussrr/lliibb/uuuuccpp/ssyyss -- List of reachable systems
/uussrr/ssppooooll/uuuuccpp/.LLoogg/*/_s_i_t_e_n_a_m_e -- uuuuccpp activities log files for _s_i_t_e_n_a_m_e
/uussrr/ssppooooll/uuuuccpp/_s_i_t_e_n_a_m_e -- Spool directory for work

_S_e_e _A_l_s_o
ccoommmmaannddss, mmaaiill, uuuucciiccoo, UUUUCCPP, uuuuddeeccooddee, uuuueennccooddee, uuuuttoouucchh, uuuuxxqqtt
