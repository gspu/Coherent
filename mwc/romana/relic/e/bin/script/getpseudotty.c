#include <errno.h>
#include <fcntl.h>

static char PtyName[32] = "/dev/ptyXY";
static char TtyName[32] = "/dev/ttyXY";

int getpseudotty(m, s)
char *m, *s;
{
	register char *l, *d;
	int index, f;
   
	index = strlen(PtyName) - 2;
	for (l = "pqrstuvwxyz"; *l; ++l) {
		for (d = "0123456789abcdef"; *d; ++d) {
			PtyName[index] = *l;
			PtyName[index+1] = *d;
			if ((f = open(PtyName, O_RDWR)) != -1) {
				strcpy(m, PtyName);
				TtyName[index] = *l;
				TtyName[index+1] = *d;
				strcpy(s, TtyName);
				return f;
			} else {
				perror("script: opening master");
				if(errno == ENXIO)
					break;
			}
		}
	}
	return -1;
}
