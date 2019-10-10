/
/ C interface for _brk system call.
/ char *_brk(cp);
/ char *cp;

.globl	_brk_

_brk_:
	sys	021		/17
	ret
