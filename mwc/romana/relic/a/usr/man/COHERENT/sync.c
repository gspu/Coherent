

sync                         Command                         sync




Flush system buffers

ssyynncc

Most COHERENT commands manipulate files stored on a disk.  To im-
prove  system performance,  the COHERENT  system often  changes a
copy  of part  of the  disk in  a buffer  in memory,  rather than
repeatedly performing the time-consuming disk access required.

sync writes information from  the memory buffers to the disk, up-
dating the  disk images  of all  mounted file systems  which have
been changed.   In addition, it  writes the date and  time on the
root file system.

sync should be executed  before system shutdown to ensure the in-
tegrity of the file system.

***** See Also *****

commands




































COHERENT Lexicon                                           Page 1


