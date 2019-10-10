#include <sys/coherent.h>
#include <stddef.h>

#include <sys/haiscsi.h>

extern dca_t	sddca;		/* Fixed disk control routines */
extern dca_t	ctdca;		/* Cartridge tape control routines */

dca_t  mdca[MAXDEVS] = {
	&sddca, 		/* Fixed disk */
	NULL,			/* No device */
	&ctdca, 		/* Cartridge tape drive */
	NULL,			/* No device */
	NULL,			/* No device */
	NULL,			/* No device */
	NULL,			/* No device */
	NULL			/* No device */
};

/* End of file */
