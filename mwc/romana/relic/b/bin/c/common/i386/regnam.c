/*
 * common/i386/regnam.c
 * Register names, including pseudos.
 * Indexed by register numbers as defined in mch.h.
 * i386.
 */

char	*regnames[] = {

	/* Dword. */
	"%eax",
	"%edx",
	"%ebx",
	"%ecx",
	"%esi",
	"%edi",
	"%esp",
	"%ebp",

	/* Qword. */
	"%edx:%eax",

	/* Word. */
	"%ax",
	"%dx",
	"%bx",
	"%cx",
	"%si",
	"%di",
	"%sp",
	"%bp",

	/* Byte. */
	"%al",
	"%bl",
	"%cl",
	"%dl",
	"%ah",
	"%bh",
	"%ch",
	"%dh",

	/* Floating point. */
	"%st",

	/* Pseudo. */
	"None",
	"Anyr",
	"Anyl",
	"Pair",
	"Temp",
	"Lo",
	"Hi"
};

/* end of common/i386/regnam.c */
