/
/ C interface for setpgrp system call.
/ setpgrp(group)

.globl	setpgrp_

setpgrp_:
	sys	076		/62
	ret
