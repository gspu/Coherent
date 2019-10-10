/
/ C interface for ptrace system call
/ ptrace(req, pid, addr, data);

.globl	ptrace_

ptrace_:
	sys	032		/26
	ret
