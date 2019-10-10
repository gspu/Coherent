#ifndef __SYS_H
#define __SYS_H

extern "C" {
	int open(char *, int);
	int read(int, void *, int);
	int close(int);
}

inline int open(const char * fn, int n) { return open( (char *)fn, n); }

#endif
