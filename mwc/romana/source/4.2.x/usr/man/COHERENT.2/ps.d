ppss -- Device Driver

Driver to return information about processes
/ddeevv/ppss

The file /ddeevv/ppss lets you read the kernel's process table.  It is a part of
the driver mmeemm, which manages memory; thus, it has major number 0 and minor
number 6.

/ddeevv/ppss is  a read-only device that  exists only to support  the command ppss
and  its  variants.   The  command  ppss  reads  this  device  to  display  a
``snapshot'' of the processes that the COHERENT kernel is executing.

Reading /ddeevv/ppss deposits an array  of the structure ssttMMoonniittoorr into the read
buffer.  The number of bytes requested  by the system call rreeaadd() should be
enough to accommodate the entire process table.  Header file <ssyyss/ccoohh_ppss.hh>
defines ssttMMoonniittoorr.

_S_e_e _A_l_s_o
ddeevviiccee ddrriivveerrss, ppss (command)
