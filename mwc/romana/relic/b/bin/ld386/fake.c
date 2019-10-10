/*
 * Fake versions of driver_alloc and driver_fail
 * to test the loader.
 */
#include "ld.h"
/*
 * build like normal file.
 */
driver_alloc(aouth)
AOUTHDR *aouth;
{
	aouth->text_start = 0xA8;
	aouth->data_start = ((aouth->text_start + aouth->tsize) & 0x0fff)
				+ DATABASE;
	return(1); /* ok */
}

/*
 * Say we got here.
 */
void
driver_fail()
{
	printf("driver_fail called\n");
}
