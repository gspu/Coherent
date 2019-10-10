#include <time.h>
#include <sys/types.h>
main()
{
	time_t timenumber;
	struct tm *timestruct;

	time(&timenumber);
	printf("%s", ctime(&timenumber));
	timestruct = gmtime(&timenumber);
	printf("%s", asctime(timestruct));
	timestruct = localtime(&timenumber);
	printf("%s", asctime(timestruct));
}
