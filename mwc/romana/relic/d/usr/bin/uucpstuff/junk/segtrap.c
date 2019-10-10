#include <signal.h>
#include <setjmp.h>
jmp_buf helpme;
int	stop;
int	catchintr();
catchintr()
{
	char	*cot = "Caught the silly interrupt\n";
	signal(SIGSEGV, SIG_IGN);
	write(1, cot, strlen(cot));
	printf("Caught interrupt\n");
	stop++;
	signal(SIGSEGV, catchintr);
	longjmp(helpme, 3);
}

main()
{
	char	*cp;
	int	c;

	stop = 0;
	signal(SIGSEGV, catchintr);
	if(setjmp(helpme) == 0) {
		cp = (char *) 0x1A00;
		c = *cp;
	}

	printf("stop is %d\n", stop);
	exit (0);
}
