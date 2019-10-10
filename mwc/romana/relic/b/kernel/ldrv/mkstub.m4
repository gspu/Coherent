define(sym, substr(basename, -2, ))dnl
	.globl	sym`_'
sym`_':	mov	ax,`$K'sym`_'
	.byte	0x9A
	.word	xcalled
	.word	0x0060
	ret
