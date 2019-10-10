/* verify_speed.c: This is passed a baud rate entered and verifies that
		   it is indeed a valid baud rate. If an invalid speed
		   is entered, return a -1, otherwise return a 1.
 */

int valid[7] = {300, 1200, 2400, 4800, 9600, 19200, 38400};

#include <stdio.h>

validate_speed(speed)
int speed;			/* baud rate passed from calling function */
{
	int x;
	for (x = 0 ; x < 7 ; x++){
		if(speed == valid[x])
			return(1);
	}
	return(-1);
}
