/*
 * fakeff.h
 * Include file to simulate all the far memory calls from COH 286.
 * Far pointers are simulated with virtual addresses.
 */
#ifndef FAKEFF_H	/* Rest of file.  */
#define FAKEFF_H

typedef	unsigned long	faddr_t;

/*
 * Read a byte from far address 'fp'.  Note that if an address fault
 * occurs, the system will panic.
 */
#define ffbyte(fp)	(*((unsigned char *) (fp)))

/*
 * Read a word from far address 'fp'.  Note that if an address fault
 * occurs, the system will panic.
 */
#define ffword(fp)	(*((unsigned short *) (fp)))

/*
 * Copy 'n' bytes from far address 'fp' to address 'k' in the kernel
 * data segment.  Returns the number of bytes copied.
 * Note that if an address fault occurs, the system will panic.
 */
#define fkcopy(fp, k, n)	kkcopy(fp, k, n)

/*
 * Copy 'count' bytes from 'from_fp' to 'to_fp'.
 * Note that if an address fault occurs, the system will panic.
 */
#define ffcopy(from_fp, to_fp, count)	(void) kkcopy(from_fp, to_fp, count)

/*
 * Write byte 'b' to address 'fp'.  Note that if an address
 * fault occurs, the system will panic.
 */
#define sfbyte(fp, b)	{*(unsigned char *)(fp) = (unsigned char)(b);}

/*
 * Write word 'w' to address 'fp'.  Note that if an address
 * fault occurs, the system will panic.
 */
#define sfword(fp, w)	{*((unsigned short *)(fp)) = (unsigned short)(w);}

faddr_t ptov();
void vrelse();

#endif /* FAKEFF_H */
