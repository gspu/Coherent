/*
 * Return 1 if under Coherent else 0.
 */
#include	<sys/utsname.h>
#include	<string.h>

if_COHERENT()
{
	static char OS_VERSION[] = "COHERENT";
	struct utsname	tsname;

	if (uname(&tsname) < 0)
		return (0);
	return (!strncmp(tsname.version, OS_VERSION, sizeof(OS_VERSION) - 1));
}

#ifdef TEST
main()
{
	puts(if_COHERENT()
		? "Congratulations! This is COHERENT!"
		: "You fool! This is not COHERENT!");
}
#endif
