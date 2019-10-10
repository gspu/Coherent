typedef int fd_set;

#define FD_ZERO(fd)	(*fd = 0)
#define FD_SET(b,fd)	(*fd |= 1 << (b))
#define FD_ISSET(b,fd)	(*fd & 1 << (b))
#define FD_SETSIZE 32
