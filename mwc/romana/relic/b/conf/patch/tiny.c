/*
 * Demonstrarte a union bug.
 */
#include <stdio.h>

typedef union {
	char	p_char;
	short	p_short;
	int	p_int;
	long	p_long;
} p_val;

main()
{
	p_val test_val;
	
	test_val.p_long = 0x11223344;

	printf("char 0x%x\n", test_val.p_char);
	printf("short 0x%x\n", test_val.p_short);
	printf("int 0x%x\n", test_val.p_int);
	printf("long 0x%x\n", test_val.p_long);

} /* main() */
