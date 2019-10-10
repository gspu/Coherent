#include <stdio.h>
#include <string.h>

main()
{
	char array[50] = {"this is a test \\"};
	printf("Length of array is %d\n",strlen(array));
	printf("array[%d] is {%d} array[%d] is {%d} array[%d] is {%d}\n",
		strlen(array), array[strlen(array)], strlen(array) -1, 
		array[strlen(array) -1], strlen(array) -2,
		array[strlen(array) -2]);
}
