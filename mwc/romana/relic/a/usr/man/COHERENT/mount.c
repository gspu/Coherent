

mount                        Command                        mount




Mount a file system

/eettcc/mmoouunntt [ _s_p_e_c_i_a_l _d_i_r_e_c_t_o_r_y [ -rruu ] ]

mount mounts  a file system  from the block  special file special
onto directory in  the system's directory hierarchy.  This opera-
tion makes  the root directory of the  mounted file system acces-
sible using the specified directory name.

If  the -r  option is  specified, the  file system  is read-only.
This  is useful  for preventing  inadvertent changes  to precious
file systems.  The system will not update information such as ac-
cess times if the -r option is used.

The -u option causes mount to write an entry into the mount table
file /etc/mtab  without actually  performing the mount.   This is
used to note the file system.

When invoked with no arguments, mount summarizes the mounted file
systems and where they attach.

The command umount unmounts a previously mounted file system.

The script  /bin/mount calls /etc/mount,  and provides convenient
abbreviations for commonly used devices.  For example,


        mount f0


executes the command:


        /etc/mount /dev/fha0 /f0


The system  administrator should edit this  script to reflect the
devices used on your system.

***** Files *****

/eettcc/mmttaabb -- Mount table
/eettcc/mmnnttttaabb -- Mount table
/bbiinn/mmoouunntt -- Shell script that calls /eettcc/mmoouunntt

***** See Also *****

commands, fsck, mkfs, mknod, umount

***** Diagnostics *****

Errors can occur if special or directory does not exist or if the
user has no permissions on special.



COHERENT Lexicon                                           Page 1




mount                        Command                        mount



The message


        /etc/mtab older than /etc/boottime


indicates that /etc/mtab has probably been invalidated by booting
the system.

Attempting to mount a block-special file which does not contain a
COHERENT file system may have disastrous consequences.  mkfs must
be used  to create  a file  system on a  blank disk before  it is
mounted.












































COHERENT Lexicon                                           Page 2


