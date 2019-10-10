

uucp                         Command                         uucp




Ready files for transmission to other systems

uuuuccpp [ -bbccCCddmm ] _s_o_u_r_c_e_1 ... _s_o_u_r_c_e_N _d_e_s_t

uucp copies files source1 through sourceN to the destination sys-
tem  dest.   Either  source  or  destination  files  can  contain
specifications for the remote system.

uucp recognizes the following options:

-cc Instead of copying the source file to the spool directory, use
   the file itself.  This is a default.

-CC Copy the source file to the spool directory.

-dd Make directories on _d_e_s_t if they are necessary for copying the
   files.

-ff Do not make intermediate directories for the file copy.

-gg_g_r_a_d_e
   grade is a single ASCII character indicating the importance of
   the files being transmitted: the lower the value of grade, the
   more important the files.

-mm Send mail to the requester when the file is sent.

-nn_u_s_e_r
   Notify user  on destination system  that file was  sent.  Note
   that user may contain a path:

           -nuser!site


-xx_d_e_b_u_g
   debug is a single-digit number, 0 to 9.  The higher the level,
   the more information yielded.

***** Examples *****

The first  example copies  file foo  to directory /bar  on system
george:


        uucp foo george!/bar


The next example copies  file /foo from system george into direc-
tory /tmp on your system:







COHERENT Lexicon                                           Page 1




uucp                         Command                         uucp



        uucp george!/foo /tmp


The next example copies file /foo from system george into file or
directory /bar on system ivan:


        uucp george!/foo ivan!/bar


Note that  this assumes your  system can talk to  both george and
ivan and  that your  system has permission  to read file  /foo on
system george as well as to write file /bar on system ivan.

The next  example downloads files /foo and  /bar from remote sys-
tems ivan and george into directory /tmp on your system:


        uucp ivan!/foo george!/bar /tmp


***** Files *****

/uussrr/lliibb/uuuuccpp/LL.ssyyss -- List of reachable systems
/uussrr/lliibb/uuuuccpp/PPeerrmmiissssiioonnss -- List of system permissions
/uussrr/ssppooooll/uuuuccpp/.LLoogg/*/_s_i_t_e_n_a_m_e -- uuuuccpp  activities log files for
_s_i_t_e_n_a_m_e
/uussrr/ssppooooll/uuuuccpp/_s_i_t_e_n_a_m_e -- Spool directory for work

***** See Also *****

commands,  mail,  uucico,   UUCP,  uudecode,  uuencode,  uutouch,
uuwatch, uuxqt
























COHERENT Lexicon                                           Page 2


