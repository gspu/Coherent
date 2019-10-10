

mknod                        Command                        mknod




Make a special file or named pipe

/eettcc/mmkknnoodd _f_i_l_e_n_a_m_e _t_y_p_e _m_a_j_o_r _m_i_n_o_r
/eettcc/mmkknnoodd _f_i_l_e_n_a_m_e pp

In the  first form, mknod creates a  special file, which provides
access to a device  by the filename specified.  Special files are
conventionally stored in the /dev directory.

type  can be  either  `b' (for  block-special file)  or `c'  (for
character-special file).  Block-special  files tend to be devices
such  as disks  or  magnetic tape,  upon which  COHERENT uses  an
elaborate  buffering strategy.   Character-special files  are un-
structured (character at  a time) devices such as terminals, line
printers, or communications devices.  Character-special files may
also be random-access devices; this circumvents system buffering,
allowing transfers  of arbitrary  size directly between  the user
and the hardware.

The major  device number uniquely  identifies a device  driver to
COHERENT.  The minor  device number is a parameter interpreted by
the driver; it might specify  the channel of a multiplexor or the
unit number of a drive.

The caller must be the superuser.

In the second form, mknod creates a pipe with the given filename.
Named pipes can be used for communication between processes.

***** Files *****

/dev/*

***** See Also *****

commands, mount




















COHERENT Lexicon                                           Page 1


