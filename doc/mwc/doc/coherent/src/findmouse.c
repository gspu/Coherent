/*
 * find a mouse plugged into port argv[1]
 */
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <termio.h>

char *mouse;
int mouse_fd;
struct termio old_tty, new_tty;

/* do the right thing by signals */
sig_handler()
{
	ioctl(mouse_fd, TCSETAF, &old_tty);
	exit(EXIT_SUCCESS);
}

/* cry and die */
void fatal(message)
char *message;
{
	fprintf (stderr, "%s\n", message);
	exit(EXIT_FAILURE);
}

/* run the whole shebang */
main(argc, argv)
int argc; char **argv;
{
	struct pollfd fds[1];

	if (argc != 2)
		fatal ("Usage: findmouse /dev/com[1-4]pl");

	if (strncmp(argv[1], "/dev/com1pl", 11) &&
			strncmp(argv[1], "/dev/com2pl", 11) &&
			strncmp(argv[1], "/dev/com3pl", 11) &&
			strncmp(argv[1], "/dev/com4pl", 11))
		fatal ("Usage: findmouse /dev/com[1-4]pl");

	mouse = argv[1];

	signal(SIGINT, sig_handler);
	signal(SIGQUIT, sig_handler);
	signal(SIGHUP, sig_handler);

	if ((mouse_fd = open(mouse, O_RDONLY)) < 0)
		fatal ("Cannot open this device.");

	if (ioctl(mouse_fd, TCGETA, &old_tty) < 0)
		fatal ("Cannot read this device'ss line mode.");

	new_tty = old_tty;
	new_tty.c_cflag &= ~(CBAUD | HUPCL);
	new_tty.c_cflag |= CLOCAL | B1200;
	new_tty.c_iflag = IGNBRK;
	new_tty.c_oflag = new_tty.c_lflag = 0;

	/*
	 * VMIN = 0, VTIME = 0 has the same effect as setting O_NDELAY on the
	 * input line.
	 */
	new_tty.c_cc[VMIN] = 0;
	new_tty.c_cc[VTIME] = 0;

	/* Set up to poll the input line. */
	fds->fd = mouse_fd;
	fds->events = POLLIN;

	if (ioctl(mouse_fd, TCSETAF, &new_tty) < 0)
		fatal ("Cannot set tty line mode for device %s\n", mouse);

	fprintf(stdout, "\nNow reading from %s. To exit, type <ctrl-C>.\n",
		mouse);
	fprintf(stdout,
		"If you see stuff on the screen when you move the mouse,\n");
	fprintf(stdout,
		"then you have found the mouse port.\n");
	fprintf(stdout, "\nNow wiggle your mouse:\n");

	for (;;) {
		size_t read_count;
		unsigned char mousebuf [128];

		/* Block waiting for mouse input. */
		if (poll (fds, 1, -1) < 0)
			break;

		/* Drain input in large chunks until it becomes time to block. */
		while ((read_count = read (mouse_fd, mousebuf,
				sizeof (mousebuf))) != 0) {
			unsigned char * scan = mousebuf;

			do
				printf ("%s: %02x\n", mouse, * scan ++);
			while (-- read_count != 0);
		}
	}
}
