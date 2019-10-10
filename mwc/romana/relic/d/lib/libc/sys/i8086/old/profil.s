/
/ C interface for profil system call.
/ profil(buff, bufsiz, offset, scale);

.globl	profil_

profil_:
	sys	054		/44
	ret
