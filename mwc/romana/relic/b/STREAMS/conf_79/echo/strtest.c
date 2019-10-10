#include <stropts.h>
#include <fcntl.h>
#include <stdio.h>


int main (argc, argv)
int		argc;
char	      *	argv [];
{
	int		fd;
	char		buf [20];
	int		cnt;

	if ((fd = open ("/dev/echo", O_RDWR)) == -1) {

		perror ("Couldn't open /dev/echo");
		return -1;
	}

	fprintf (stderr, "Returned %d\n", fd);

	fprintf (stderr, "PUSH ioctl () says %d\n", ioctl (fd, I_PUSH, "dump"));
	fprintf (stderr, "SRDOPT ioctl () says %d\n", ioctl (fd, I_SRDOPT, RMSGN));

	write (fd, "Foo!", 5);
	cnt = read (fd, buf, sizeof (buf));
	buf [cnt] = 0;

	fprintf (stderr, "Read %d bytes, result = %s\n", cnt, buf);

	close (fd);

	return 0;
}
