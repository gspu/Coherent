cchhmmoodd -- Command

Change the modes of a file
cchhmmoodd +_m_o_d_e_s _f_i_l_e
cchhmmoodd -_m_o_d_e_s _f_i_l_e

The  COHERENT system  assigns a  _m_o_d_e to  every file,  to govern  how users
access the file.   The mode grants or denies permission  to read, write, or
execute a file.

The mode grants permission separately to the owner of a file, to users from
the  owner's group,  and  to all  other  users.  For  a directory,  execute
permission  grants or  denies the  right to  search the  directory, whereas
write permission grants or denies the right to create and remove files.

In addition,  the mode contains three bits that  perform special tasks: the
set-user-id bit, the set-group-id bit, and the save-text or ``sticky'' bit.
See  the  Lexicon entry  for  the  COHERENT system  call  cchhmmoodd() for  more
information on how to use these bits.

The command cchhmmoodd changes  the permissions of each specified _f_i_l_e according
to  the given  _m_o_d_e argument.   _m_o_d_e  may be  either an  octal number  or a
symbolic mode.   Only the  owner of  a _f_i_l_e or  the superuser may  change a
file's mode.  Only the superuser may set the sticky bit.

A symbolic mode may have the following form.  No spaces should separate the
fields in the actual _m_o_d_e specification.

    [_w_h_i_c_h] _h_o_w _p_e_r_m ... [, ...]

_w_h_i_c_h specifies  the permissions that are affected by  the command.  It may
consist of one or more of the following:

    aa   All permissions, equivalent to ggoouu
    gg   Group permissions
    oo   Other permissions
    uu   User permissions

If no _w_h_i_c_h  is given, aa is assumed and  cchhmmoodd uses the file creation mask,
as described in uummaasskk.

_h_o_w specifies how the permissions will be changed.  It can be

    =   Set permissions
    +   Add permissions
    -   Take away permissions

_p_e_r_m specifies  which permissions  are changed.  It  may consist of  one or
more of the following:

    gg   Current group permissions
    oo   Current other permissions
    rr   Read permission
    ss   Setuid upon execution
    tt   Save text (sticky bit)
    uu   Current user permissions
    ww   Write permission
    xx   Execute permission

Multiple _h_o_w/_p_e_r_m pairs  have the same _w_h_i_c_h applied to  them.  One or more
specifications separated  by commas tell cchhmmoodd  to apply each specification
to the file successively.

An octal _m_o_d_e argument to cchhmmoodd  is obtained by _O_Ring the desired mode bits
together.  For a list of the  recognized octal modes, see the Lexicon entry
for cchhmmoodd().

_E_x_a_m_p_l_e_s
The first  example below  sets the  owner's permissions to  read +  write +
execute, and the group and other permissions to read + execute.  The second
example adds execute permission for everyone.

    chmod u=rwx,go=rx file
    chmod +x file

_S_e_e _A_l_s_o
cchhggrrpp, cchhmmoodd(), cchhmmoogg, cchhoowwnn, ccoommmmaannddss, llss, ssttaatt, uummaasskk
