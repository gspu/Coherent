/*
 *	phase 3 of fsck - Check Connectivity
 */

#include "fsck.h"

char *lostname = "lost+found";

phase3()
{
	ino_t i;

	if (!qflag)
		printf("Phase 3 : Check Connectivity\n");
	for (i=FIRSTIN; i<=ninodes; i++) 
		if ( ((flags(i)&MODEMASK) == IDIR) &&
		     (linkctr(i) == 0) ) {
			orflags(i, UNREFDIR);
			markchildren(i);
		}

	for (i=FIRSTIN; i<=ninodes; i++)
		if ( (flags(i)&(UNREFDIR|CHILDDIR)) == UNREFDIR )
			unrefdir(i);
}

markchildren(ino)
ino_t ino;
{
	struct dinode *dip;
	struct direct *element;
	daddr_t	addrs[NADDR];
	ino_t inumber;
	fsize_t length;

	if ( badblks(ino) )
		return;
	dip = ptrino(ino, databuf);
	if ( (length=dip->di_size) <= 0 )
		return;
	l3tol(addrs, dip->di_addr, NADDR);
	cdbn = 0;
	nextblock(addrs);
	element = (struct direct *) databuf;
	element += 2;		/* skip over . and .. entries */
	if ( (length-=2*DSIZE) <= 0 )
		return;

	while ( length >= DSIZE ) {
		while ( (element<&databuf[BSIZE]) && (length>=DSIZE) ) {
			if ( ((inumber=element->d_ino) != 0) &&
			     ((flags(inumber)&MODEMASK)==IDIR) &&
			     (inumber!=ino) )
				orflags(inumber, CHILDDIR);
			element++;
			length -= DSIZE;
		}
		nextblock(addrs);
		element = (struct direct *) databuf;
	}
}

char *reconnect = "(Reconnect)";

unrefdir(ino)
ino_t ino;
{
	printf("Unref Dir\n");
	if( (pinfo(ino) == 0) || (daction == NO) || (mounted == TRUE) ) {
		if ( action(reconnect) == FALSE )
			return;
	} else 
		printf("%s FORCED\n", reconnect);

	connect(ino, IDIR);
}

#define CONNECT		1
#define NOCONNECT	0

connect(ino, type)
ino_t ino;
int type;
{
	ino_t lostino;
	struct dinode *dip;
	daddr_t addrs[NADDR];
	fsize_t length;
	struct direct *element;
	int chgino = FALSE;

	if ( ((lostino=lostfound()) == NUL) ||
		 ((flags(lostino)&MODEMASK) != IDIR) ||
			( badblks(lostino) ) ) {
		nolost();
		if (type==IDIR)
			traverse(NOCONNECT, ino, lostino);
		return(FAILURE);
	}

	dip = ptrino(lostino, buf2);
	l3tol(addrs, dip->di_addr, NADDR);
	length = dip->di_size;
	cdbn = 0;
	while ( length>=DSIZE ) {
		nextblock(addrs);
		element = (struct direct *) databuf;
		while ( element<&databuf[BSIZE] ) {
			length -= DSIZE;
			if ( element->d_ino == 0 ) {
				element->d_ino = ino;
				strncpy(element->d_name, nameit(ino), DIRSIZ);
				if ( fixblock(addrs) == NUL )
					fixblkerr();
				linkincr(ino);
				numfiles++;
				if ( length<0 ) {
					dip->di_size -= length;
					chgino = TRUE;
				}
				if ( (type == IDIR) &&
				     (connected(ino, lostino)==YES) ) {
					dip->di_nlink++;
					writeino(lostino, buf2);
					traverse(CONNECT, ino, lostino);
				} else if (chgino) {
					writeino(lostino, buf2);
				}
				return(SUCCESS);
			}
			element++;
		}
	}
	
	noroom();
	if (type == IDIR)
		traverse(NOCONNECT, ino, lostino);
	return(FAILURE);
}

traverse(flag, ino, lostino)
int flag;
ino_t ino, lostino;
{
	struct direct dirlost, nothing, newtree;

	dirinit(&dirlost, lostino, lostname);
	dirinit(&nothing, 0, "??");
	dirinit(&newtree, ino, nameit(ino));
	depth = 0;
	if (flag == CONNECT) 
		path[depth++] = &dirlost;
	else
		path[depth++] = &nothing;

	path[depth++] = &newtree;
	checkpath(ino);
}

dirinit(element, ino, name)
struct direct *element;
ino_t ino;
char *name;
{
	element->d_ino = ino;
	strncpy(element->d_name, name, DIRSIZ);
}

connected(ino, lostino)
ino_t ino;		/* inumber just reconnected	  */
ino_t lostino;		/* inumber of lostfound directory */
{
	struct dinode *dip;
	daddr_t addrs[NADDR];
	fsize_t length;
	struct direct *element;

	if ( badblks(ino) ) {
		badconnect(ino);
		return(NO);
	}
	dip = ptrino(ino, databuf);
	l3tol(addrs, dip->di_addr, NADDR);
	length = dip->di_size;
	cdbn = 0;
	while ( length >= DSIZE ) {
		nextblock(addrs);
		element = (struct direct *) databuf;
		while ( (element<&databuf[BSIZE]) && (length>=DSIZE) ) {
			if ( strcmp(element->d_name, "..") == 0 ) {
				printf("Dir i-number = %u connected.  ", ino);
				printf("Parent was i-number = %u.\n",
							element->d_ino);
				element->d_ino = lostino;
				if ( fixblock(addrs) == NUL )
					fixblkerr();
				return(YES);
			}
			length -= DSIZE;
			element++;
		}
	}
	noparent(ino);
	return(NO);
}

badconnect(ino)
ino_t ino;
{
	printf("Dir i-number = %u connected.  It has bad/dup blocks.\n", ino);
}

noparent(ino)
ino_t ino;
{
	printf("Dir i-number = %u connected.  It has no .. entry.\n", ino);
}

ino_t
lostfound()
{
	static daddr_t addrs[NADDR];
	struct dinode *dip;
	struct direct *element;
	fsize_t length;

	dip = ptrino(ROOTIN, databuf);
	l3tol(addrs, dip->di_addr, NADDR);
	length = dip->di_size;
	cdbn = 0;
	while ( length>=DSIZE ) {
		nextblock(addrs);
		element = (struct direct *) databuf;
		while ( (element<&databuf[BSIZE]) && (length>=DSIZE) ) {
			if ( strcmp(element->d_name, lostname) == 0 ) 
				return(element->d_ino);
			length -= DSIZE;
			element++;
		}
	}

	return(NUL);
}

nolost()
{
	printf("Sorry.  No %s directory.\n", lostname);

}

noroom()
{
	printf("Sorry.  No space in %s directory.\n", lostname);
}

char namebuf[DIRSIZ];

char *
nameit(ino)
register ino_t ino;
{
	register int i = DIRSIZ-1;

	namebuf[i] = '\0';
	while ( (ino != 0) && (i>0) ) {
		namebuf[--i] = '0' + ino%10;
		ino = ino/10;
	}
	return(&namebuf[i]);
}
