#include	<time.h>
#include	<sys/types.h>

main()
{
	time_t t;

	time(&t);
	printf(ctime(&t));
}
