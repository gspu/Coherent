/*
 * Make a duplicate of `ofd' on
 * file descriptor `nfd', closing
 * `nfd' if necessary.
 */

dup2(ofd, nfd)
int ofd, nfd;
{
	return (dup(ofd|0100, nfd));
}
