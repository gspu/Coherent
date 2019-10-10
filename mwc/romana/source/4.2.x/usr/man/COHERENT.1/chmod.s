cchhmmoodd() -- System Call (libc)

Change file-protection modes
#iinncclluuddee <ssyyss/ssttaatt.hh>
cchhmmoodd(_f_i_l_e, _m_o_d_e)
cchhaarr *_f_i_l_e; iinntt _m_o_d_e;

cchhmmoodd() sets the mode bits for _f_i_l_e. The mode bits include protection bits,
the set-user-id bit, and the sticky bit.

_m_o_d_e is constructed  from the logical OR of the  mode constants declared in
the header file ssttaatt.hh, as follows:

     SS_IISSUUIIDDSet user identifier on execution
     SS_IISSGGIIDDSet group identifier on execution
     SS_IISSVVTTXXSave file on swap device (``sticky bit'')
     SS_IIRRUUSSRRRead permission for owner
     SS_IIWWUUSSRRWrite permission for owner
     SS_IIXXUUSSRRExecute permission for owner
     SS_IIRRGGRRPPRead permission for members of owner's group
     SS_IIWWGGRRPPWrite permission for members of owner's group
     SS_IIXXGGRRPPExecute permission for members of owner's group
     SS_IIRROOTTHHRead permission for other users
     SS_IIWWOOTTHHWrite permission for other users
     SS_IIXXOOTTHHExecute permission for other users

For  directories, some  protection  bits have  a  different meaning:  write
permission  means  files  may  be  created  and  removed,  whereas  execute
permission means that the directory may be searched.

The  save-text bit  (or ``sticky  bit'') is  a flag to  the system  when it
executes a shared for of a load module.  After the system runs the program,
it  leaves  shared  segments   on  the  swap  device  to  speed  subsequent
reinvocation  of  the  program.  Setting  this  bit  is  restricted to  the
superuser  (to control  depletion  of swap  space which  might result  from
overuse).

Only the owner of a file or the superuser may change its mode.

_S_e_e _A_l_s_o
ccrreeaatt(), lliibbcc, ssttaatt.hh
POSIX Standard, section 5.6.4

_D_i_a_g_n_o_s_t_i_c_s
cchhmmoodd()  returns -1  for  errors, such  as  _f_i_l_e being  nonexistent or  the
invoker being neither the owner nor the superuser.
