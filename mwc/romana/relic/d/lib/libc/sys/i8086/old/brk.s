/
/ C interface for brk system call
/ brk(newend);
/ returns actual new end (may be rounded up to hardware boundary)
/ brk((char *)0) does not change the end.
/

.globl	brk_

brk_:
	sys	021		/17
	ret
