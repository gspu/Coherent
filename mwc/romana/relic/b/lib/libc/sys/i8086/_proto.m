/ libc/sys/i8086/_proto.m
/ System call interface prototype.
/ Assumes NAME and VALUE are defined appropriately.

	.globl	NAME
NAME:
	sys	VALUE
	ret

/ end of _proto.m
