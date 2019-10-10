

umount                       Command                       umount




Unmount file system

/eettcc/uummoouunntt _s_p_e_c_i_a_l

umount unmounts a file system special that was previously mounted
with the mount command.

The script /bin/umount calls /etc/umount, and provides convenient
abbreviations for commonly used devices.  For example, typing


        umount f0


executes the command


        /etc/umount /dev/fha0


The system  administrator should edit this  script to reflect the
devices used on your specific system.

***** Files *****

/eettcc/mmttaabb -- Mount table
/ddeevv/*
/bbiinn/uummoouunntt -- Script that calls /eettcc/uummoouunntt

***** See Also *****

clri, commands, fsck, icheck, mount

***** Diagnostics *****

Errors can  occur if special does  not exist or is  not a mounted
file system.



















COHERENT Lexicon                                           Page 1


