ssyynncc -- Command

Flush system buffers
ssyynncc

Most  COHERENT commands  manipulate  files stored  on a  disk.  To  improve
system performance, the COHERENT system often changes a copy of part of the
disk in  a buffer  in memory, rather  than repeatedly performing  the time-
consuming disk access required.

ssyynncc writes  information from the memory buffers to  the disk, updating the
disk  images of  all  mounted file  systems  which have  been changed.   In
addition, it writes the date and time on the root file system.

ssyynncc should  be executed before system shutdown to  ensure the integrity of
the file system.

_S_e_e _A_l_s_o
ccoommmmaannddss
