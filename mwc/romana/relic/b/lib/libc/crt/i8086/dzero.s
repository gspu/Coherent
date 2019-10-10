////////
/
/ Intel 8086 C runtime.
/ Zero the _fpac_.
/ SMALL model.
/
////////

	.globl	dzero
	.globl	_fpac_

////////
/
/ dzero()
/ Returns 0 in AX, 0. in _fpac_.
/
////////

dzero:
	push	di

	mov	di, $_fpac_
	sub	ax, ax
	cld
	stosw
	stosw
	stosw
	stosw

	pop	di
	ret
