

chmod                        Command                        chmod




Change the modes of a file

cchhmmoodd +_m_o_d_e_s _f_i_l_e
cchhmmoodd -_m_o_d_e_s _f_i_l_e

The COHERENT  system assigns a mode to every  file, to govern how
users access  the file.  The mode grants  or denies permission to
read, write, or execute a file.

The mode grants permission separately  to the owner of a file, to
users  from the  owner's group,  and to all  other users.   For a
directory,  execute  permission grants  or  denies  the right  to
search the  directory, whereas write permission  grants or denies
the right to create and remove files.

In addition,  the mode contains  three bits that  perform special
tasks: the  set-user-id bit, the set-group-id  bit, and the save-
text or  ``sticky'' bit.  See the Lexicon  entry for the COHERENT
system call chmod for more information on how to use these bits.

The command chmod  changes the permissions of each specified file
according  to the  given mode  argument.  mode  may be  either an
octal number or a symbolic mode.  Only the owner of a file or the
superuser may  change a file's mode.  Only  the superuser may set
the sticky bit.

A symbolic  mode may have  the following form.   No spaces should
separate the fields in the actual mode specification.


             [_w_h_i_c_h] _h_o_w _p_e_r_m ... [, ...]


which specifies the permissions that are affected by the command.
It may consist of one or more of the following:


             aa       All permissions, equivalent to ggoouu
             gg       Group permissions
             oo       Other permissions
             uu       User permissions


If no which is given, a  is assumed and chmod uses the file crea-
tion mask, as described in umask.

how specifies how the permissions will be changed.  It can be


             =       Set permissions
             +       Add permissions
             -       Take away permissions




COHERENT Lexicon                                           Page 1




chmod                        Command                        chmod




perm specifies which  permissions are changed.  It may consist of
one or more of the following:


             gg       Current group permissions
             oo       Current other permissions
             rr       Read permission
             ss       Setuid upon execution
             tt       Save text (sticky bit)
             uu       Current user permissions
             ww       Write permission
             xx       Execute permission


Multiple _h_o_w/_p_e_r_m pairs have the same which applied to them.  One
or more  specifications separated by  commas tell chmod  to apply
each specification to the file successively.

The octal modes (see stat) are as follows:


         0044000000    Set user id upon execution
         0022000000    Set group id upon execution
         0011000000    Sticky bit (save text)
         0000440000    Owner read permission
         0000220000    Owner write permission
         0000110000    Owner execute permission
         0000004400    Group read permission
         0000002200    Group write permission
         0000001100    Group execute permission
         0000000044    Others read permission
         0000000022    Others write permission
         0000000011    Others execute permission


An octal mode argument to  chmod is obtained by oring the desired
mode bits together.

***** Examples *****

The first  example below sets  the owner's permissions  to read +
write +  execute, and the  group and other permissions  to read +
execute.   The   second  example  adds   execute  permission  for
everyone.


    chmod u=rwx,go=rx file
    chmod +x file


***** See Also *****

chgrp, chown, commands, ls, stat, umask



COHERENT Lexicon                                           Page 2


