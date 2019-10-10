/* 
 * gpkt.c
 *
 * author: Peter S. Housel
 *
 * The |cksum()| routine is taken from UUPC, Copyright 1985, 1986, 1987 by
 * Richard H. Lamb, with (possible) changes Copyright 1987 by Stuart Lynne
 *
 * All other code is Copyright 1989 by Peter S. Housel.
 * Redistribution for any purpose is permitted provided this message
 * is included intact. No warranty of any sort is provided.
 *
 * gpkt version 1.1 7/21/89
 */

/* This program was written based on the original UUPC 'g' driver,
 * John Gilmore's version of uuslave, and Greg Chesson's protocol
 * description article. The last was especially helpful.
 *
 * This code was written around a severely hacked version of UUPC.
 * The call interface is almost identical to the original, but
 * the internal implementation is quite different. Also, many
 * functions are called that were not available in the original
 * UUPC support functions. It should serve as an adequate framework.
 *
 * The framing strategy requires that a |read()| be interruptable
 * by a |SIGALRM|. No "busy wait" or nonblocking read is required.
 */
extern	int	bad_count;
extern	int	total_errors;

#include "dcp.h"
#include "alarm.h"

#define MAXPKT		64	/* incredibly conservative... actually 4096 */
#define SWINDOW		3	/* initial send window size */
#define RWINDOW		7	/* window size we want to recieve */
#define SPKTSIZE	64	/* initial send packet size */
#define RPKTSIZE	64	/* window size we want to receive */

#define MAXLOST		5	/* max lost packets (closes or such) */
#define TIMEOUT		5	/* max seconds of before timeout */

#define LOSTPKT		-1	/* packet lost, got timeout */
#define BADPKT		-2	/* bad checksum, or data read timed out */

#define ENV_DLE		0	/* framing char at start of envelope */
#define ENV_K		1	/* packet length specifier */
#define ENV_C0		2	/* low-order checksum */
#define ENV_C1		3	/* high-order checksum */
#define ENV_C		4	/* control byte */
#define ENV_X		5	/* xor check byte */
#define ENV_LEN		6	/* overall envelope length */

#define TT_CTRL		0	/* control packet */
#define TT_DATA		2	/* data packet */
#define TT_SDATA	3	/* short data packet */
#define TT_ALTCHAN	1	/* 'alternate channel' - invalid */

#define X_CLOSE		1	/* close down protocol */
#define X_RJ		2	/* reject recieved packet */
#define X_SRJ		3	/* selectively reject packet - invalid */
#define X_RR		4	/* reciever ready */
#define X_INITC		5	/* third init packet */
#define X_INITB		6	/* second init packet */
#define X_INITA		7	/* first init packet */

#define OP_OPEN		1	/* request to open/init protocol */
#define OP_CLOSE	2	/* request to close protocol */
#define OP_WRITE	3	/* request to send packet */
#define OP_READ		4	/* request to read packet */

#define MAGIC (unsigned) 0xAAAA	/* checksum magic value */

/* from original dcp - determinie if a <= b < c, for mod 8 seq numbers */
#define between(a,b,c) (((a)<=(b) && (b)<(c)) \
|| ((c)<(a) && (a)<=(b)) \
			|| ((b)<(c) && (c)<(a)))

unsigned cksum(/* unsigned char *data, int len */);

struct {
	short sdata;		/* 'is this a short data packet' flag */
	unsigned length;	/* length of this packet */
	unsigned char *bufloc;	/* location of this data pkt's buffer */
}
inpbufs[8], outbufs[8]; /* input/output queues */

static int needack;		/* do we need to acknowledge a rcv'd pkt? */
static int neednack;		/* do we need to reject a recieved pkt? */
static int recv;		/* seq. number of last correctly rcv'd pkt */
static int lastread;		/* seq. number of last pkt ret. to caller */
static int send;		/* first packet in output window */
static int next;		/* next pkt to send  send <= next < nstuff */
static int nstuff;		/* next loc. to stuff a pkt in output queue */
/* (last pkt in output window) + 1 */
static int initpk;		/* current init sequence send packet */
static int skipping;		/* skipping out-of-seq packets after RJ */
static int spktsize;		/* send data size (requested by other end) */
static int swindow;		/* send output window size (ditto) */
static int nlost;		/* number of consecutive timeouts */
static int chanopen = 0;	/* 'channel open' flag */

static unsigned char buf[ENV_LEN + RPKTSIZE+ BUFSIZ];	/* packet framing buffer */
static unsigned char *low, *high;		/* framing buffer limits */

static unsigned char *ibufset, *obufset;	/* i/o packet buffer sets */

/* |gopenpk()| opens the 'g' packet protocol on the serial line. It
 * initializes its state variables, allocates buffers, etc., then calls
 * |gmachine()| to do the actual protocol negotiation/initialization.
 */
gopenpk()
{
	int i;				/* index */
	unsigned char *p, *q;		/* pointers into buffer set */

	high = low = buf;		/* empty input buffer */
	needack = neednack = 0;	/* don't need to accept or reject anything */
	initpk = X_INITA;		/* send INITA during initialization */
	recv = lastread = 0;		/* initial empty read queue, seq=0 */
	send = next = nstuff = 1;	/* first in output queue, seq=1 */
	skipping = nlost = 0;		/* nothing lost yet, not skipping */

	if (gmachine(OP_OPEN) < 0)	/* do the open */
		return -1;
	/* allocate send and recieve buffers */
	if (NULL == (p = ibufset = (unsigned char *)malloc(8 * RPKTSIZE)))
	    return -1;
	if (NULL == (q = obufset = (unsigned char *)malloc(8 * spktsize)))
	    return -1;
	for(i = 0; i < 8; ++i) {
		inpbufs[i].bufloc = p;
		p += RPKTSIZE;
		outbufs[i].bufloc = q;
		q += spktsize;
	}

	pktsize = spktsize;	/* for dcp compatibility */
	return 0;
}

/* |gclosepk()| closes down the packet protocol using the |OP_CLOSE| operation
 * of |gmachine()|.
 */
gclosepk()
{
	return gmachine(OP_CLOSE);
}

/* |ggetpkt()| reads one packet and returns it. The data is stored in
 * the buffer pointed to by |cdata|, and the length is stored in |*lenp|.
 * It calls |gmachine()| to get the data, and copies it from the proper input
 * buffer to the user's buffer area. "Short data" packets are handled here,
 * as opposed to within |gmachine()|.
 */
int
ggetpkt(cdata, lenp)
unsigned char *cdata; 
int *lenp;
{
	int	i;
	int nextread;
	unsigned char *bufp;

	if (!chanopen)
		return -1;
	nextread = (lastread + 1) & 7;
	printmsg(M_HIGHPROTO, "waiting for input pkt seq=%d", nextread);
	if (gmachine(OP_READ) < 0)
		return -1;
	*lenp = inpbufs[nextread].length;
	bufp = inpbufs[nextread].bufloc;
	printmsg(M_HIGHPROTO, " lenp starts out at %d, and *bufp is %d",
		*lenp, *bufp);
	if (inpbufs[nextread].sdata) {
		if (*bufp < 128) {
			/* less than 128 bytes shorter than packet length */
			printmsg(M_HIGHPROTO, "decrementing lenp once");
			*lenp -= *bufp++;
		} else {		/* more than 128 bytes shorter */
			printmsg(M_HIGHPROTO, "decrementing lenp twice");
			*lenp -= (*bufp++ & 127) * 256;
			*lenp -= *bufp++;
		}
	}
	printmsg(M_HIGHPROTO,
	"about to call memcpy, cdata %04x, bufp %04x, lenp %04x, *lenp %d",
	cdata, bufp, lenp, *lenp);
	i = *lenp;
	memcpy(cdata, bufp, i);
	lastread = nextread;
	return 0;
}

/* |gsendpkt()| queues the packet pointed to by |cdata| of length |len|
 * into the packet driver output buffer, and calls |gmachine()| to send
 * it. (|gmachine()| will return when the packet has been transmitted but
 * not necessarily acknowledged, with window size greater than 1.) If
 * |flag| is nonzero, |cdata| is considered a null-terminated string
 * which will be null-padded to the packet size and transmitted.
 */
int
gsendpkt(cdata, len, flag)
unsigned char *cdata; 
int len, flag;
{
	unsigned char *destp;
	unsigned diff;

	if (!chanopen)
		return -1;

	outbufs[nstuff].sdata = 0;
	destp = outbufs[nstuff].bufloc;
	if (flag && len < spktsize) {
		printmsg(M_HIGHPROTO, "Padded message packet |%s|", cdata);
		strncpy(destp, cdata, spktsize);
	} else {
		if ((diff = spktsize - len) > 127) { /* really short packet? */
			*destp++ = (diff >> 8) | 128;
			*destp++ = diff & 255;
			outbufs[nstuff].sdata = 1;
		} else if (diff > 0) {			/* short packet */
			*destp++ = diff;
			outbufs[nstuff].sdata = 1;
		} else
			outbufs[nstuff].sdata = 0;
		memcpy(destp, cdata, len);		/* copy into buffer */
	}
	printmsg(M_HIGHPROTO, "queued data packet seq=%d len=%d", nstuff, len);
	outbufs[nstuff].length = spktsize;
	nstuff = (nstuff + 1) & 7;

	return gmachine(OP_WRITE);
}

/* |gmachine()| is the heart of the 'g' packet driver. Its basic strategy
 * is:
 *	- transmit a packet if necessary
 *	- return if possible,
 *	- else read a packet and act on it
 *	- repeat
 *
 * |OP_OPEN| requests that the channel be opened, and |OP_CLOSE| requests that
 * it be closed. If |why| is |OP_WRITE|, |gmachine()| will return when the
 * last packet in the output queue has been transmitted (but not necessarily
 * acknowledged). |OP_READ| requests will return as soon as a new packet
 * arrives.
 */
int gmachine(why)
int why;
{
	int xxx, yyy, len;
	unsigned char *bufp;
	int shortdat;
	int i;
	while(1) {
		if (OP_CLOSE == why) {
			gspack(TT_CTRL, X_CLOSE, 0, (unsigned char *)NULL, 0);
			chanopen = 0;
			printmsg(M_MEDPROTO, "Sending CLOSE request...");
		} else if (neednack) {
			gspack(TT_CTRL, X_RJ, recv, (unsigned char *)NULL, 0);
			neednack = 0;
			printmsg(M_MEDPROTO, "Sending RJ... recv=%d", recv);
		} else if (send != nstuff	/* nonzero output queue? */
		&& between(send, next, nstuff)	/* 'next' in queue? */
		&& between(send, next, (send + swindow) & 7)) {
			/* in out win. */
			printmsg(M_MEDPROTO, "Sending data packet %d", next);
			gspack(outbufs[next].sdata ? TT_SDATA : TT_DATA,
			next, recv, outbufs[next].bufloc, outbufs[next].length);
			needack = 0;
			next = (next + 1) & 7;
			if (OP_WRITE == why && next == nstuff)
				/* go back for more */
				return 0;
		} else if (needack) {
			gspack(TT_CTRL, X_RR, recv, (unsigned char *)NULL, 0);
			needack = 0;
			printmsg(M_MEDPROTO, "Sending RR... recv=%d", recv);
		} else if (OP_OPEN == why) {
			if (X_INITB == initpk)
				i = ilog2(RPKTSIZE) - 5;
			    		/* INITB contains packet size, */
			else
				i = RWINDOW;
					/* INITA, INITC contain window size */
			printmsg(M_MEDPROTO, "inita/b pkt/wndow i = %d", i);
			/* ??? examine for INITC */
			gspack(TT_CTRL, initpk, i, (unsigned char *)NULL, 0);
		}
		if (OP_READ == why && recv != lastread) {
			return 0;
		}
		shortdat = 0;
		bufp = buf + ENV_LEN;
		switch(grpack(&xxx, &yyy, &len)) {
		case LOSTPKT:
			printmsg(M_MEDPROTO, "Lost packet...");
			if (nlost > MAXLOST) {
				plog(M_DATA, "Lost packets from other side");
				return -1;
			}
			next = send;	/* retransmit last un-ack'ed pkt, */
			if (OP_READ == why)	/* request retransmit */
				neednack = 1;
			skipping = 0;
			break;
		case BADPKT:
			printmsg(M_MEDPROTO, "Bad packet...");
			bad_count++;
			total_errors ++;
			neednack = 1;	/* reject! */
			skipping = 1;	/* ignore the rest of the 'window' */
			if (bad_count > 20) {
				plog(M_DATA, "Too many errors");	
				return -1;				
			}
			break;
		case TT_SDATA:
			shortdat = 1;
			/* fall through */
		case TT_DATA:
			bad_count = 0;
			send = (yyy + 1) & 7;	/* recieve acknowledgement */
			if (xxx != ((recv + 1) & 7)) {
				printmsg(M_MEDPROTO, 
				"Recieved data out of sequence (%d != %d)",
				xxx, (recv + 1) & 7);
				if (!skipping) {
					neednack = 1;	/* we must have missed one */
					skipping = 1;
				} else
					printmsg(M_MEDPROTO, "(Ignoring)");
			} else {
				recv = xxx;
					/* this is most recent correct pkt */
				needack = 1; /* we will ack it */
				skipping = 0;
				inpbufs[xxx].length = len;
				inpbufs[xxx].sdata = shortdat;
				memcpy(inpbufs[xxx].bufloc, bufp, len);
			}
			break;
		case TT_CTRL:
			bad_count = 0;
			skipping = 0;
			switch(xxx) {
			case X_CLOSE:
				printmsg(M_MEDPROTO, "* CLOSE *");
				gspack(TT_CTRL, X_CLOSE, 0, NULL, 0);
				chanopen = 0;
				if (OP_CLOSE == why)	/* expected? */
					return 0;
				else
					return -1;	/* nope */
			case X_RJ:
				printmsg(M_MEDPROTO, "got RJ yyy=%d", yyy);
				next = send = (yyy + 1) & 7;
				break;
			case X_RR:
				printmsg(M_MEDPROTO, "got RR yyy=%d", yyy);
				send = (yyy + 1) & 7;
				break;
			case X_INITC:
				printmsg(M_MEDPROTO, "* INITC *");
				swindow = yyy;
				if (X_INITC == initpk) {
					chanopen = 1;
					return 0;
				}
				break;
			case X_INITB:
				spktsize = 32 << yyy;
				printmsg(M_MEDPROTO, "* INITB *, spktsz %d",
					spktsize);
				if (X_INITB == initpk)
				    initpk = X_INITC;
				break;
			case X_INITA:
				printmsg(M_MEDPROTO, "* INITA *");
				swindow = yyy;
				initpk = X_INITB;
				break;	
			default:
				printmsg(M_MEDPROTO,
				"bad control packet: xxx=%d", xxx);
			}
			break;
		default:
			break;
		}
	}
}

/*
 * |grpack()| is responsible for reading one 'g'-protocol packet from the
 * input communications channel. This includes framing, detecting timeouts,
 * and checksum validation.
 * The basic strategy is to keep a count of how many bytes have currently
 * been read and how many are needed. When enough bytes for a packet header
 * ("envelope") have been read, it is validated. If it is valid, and it
 * has a nonzero data segment, the data portion is read. When it has
 * everything, it does a checksum test and returns, with the control 
 * information stored in |*xxxp|, |*yyyp|, and the data segment length stored
 * in |*lenp|.
 */
int grpack(xxxp, yyyp, lenp)
int *xxxp, *yyyp; 
int *lenp;
{
	int need;			/* need how many bytes? */
	int env;			/* 'have pkt envelope' flag */
	int gotdle;			/* do we have envelope hdr? */
	int tt;				/* packet type */
	unsigned sum;			/* checksum */
	int remain;			/* bytes which we stil need */
	int i;

	env = gotdle = 0;
	need = ENV_LEN;			/* initially, need a header */

	SETALRM(TIMEOUT);		/* time out if we don't have a packet */

	while(1) {
		if (low == high) /* prevent framebuffer overruns */
			low = high = buf;
		printmsg(M_LOWPROTO, "=> l=%d h=%d g=%d", (int)(low - buf),
			(int)(high - buf), gotdle);
		while((remain = need - (high - low)) > 0) {
			if (timedout || (i = sread2(high, remain)) < 0) {
				CLRALRM();
				++nlost;
				low = high = buf;
				/* empty out partial packet, if any */
				return env ? BADPKT : LOSTPKT;
			}
			high += i; /* got some data - move upper limit up */
		}
		if (!gotdle) {
			while(low < high) { /* look for header 'DLE' prefix */
				if (DLE == *low) {
					gotdle = 1;
					break;
				} else
					++low;
			}
			continue;
		} else if (!env) {
			/* found DLE, but haven't found header yet */
			if (low > buf) { /* move envelope to buf beginning */
				register unsigned char *dst = buf;
				while(low < high)
					*dst++ = *low++;
				low = buf;
				high = dst;
			}
			if (buf[ENV_X] != (buf[ENV_K]^buf[ENV_C0]^
				buf[ENV_C1]^buf[ENV_C])	|| 
				buf[ENV_K] < 1 || buf[ENV_K] > 9) {
					/* valid? */
				++low;
				gotdle = 0;
				printmsg(M_LOWPROTO,
				"grpack: rejecting an envelope");
				continue;
			}
			env = 1;		/* we have an envelope */
			/* store away control info */
			tt    = (buf[ENV_C] >> 6) & 3;	
			*xxxp = (buf[ENV_C] >> 3) & 7;
			*yyyp = (buf[ENV_C]     ) & 7;
			if (buf[ENV_K] == 9)	/* does it have data? */
				*lenp = 0;
			else
				*lenp = 16 << buf[ENV_K];
			/* size = 32 * 2^(k-1) */
			need += *lenp;		/* now need that many more */
			printmsg(M_LOWPROTO,
			"grpack: tt=%d, xxx=%d, yyy=%d, need=%d",
			tt, *xxxp, *yyyp, need);
			continue;
		} else {			/* have everything we need */
			if (*lenp)
				sum = MAGIC - (cksum(buf + ENV_LEN, *lenp) ^
				buf[ENV_C]);
			else {
				sum = MAGIC - buf[ENV_C];
				printmsg(M_LOWPROTO,
				"0 length data, data %x, %x, %x", buf[ENV_C],
				buf[ENV_C0], buf[ENV_C1]);
			}
			if (((sum >> 8) & 0xFF) != buf[ENV_C1]
				|| (sum & 0xFF) != buf[ENV_C0]) {
				CLRALRM();
				nlost = 0;
				printmsg(M_LOWPROTO,
				"grpack: bad check, sum is %x", sum);
				low += ENV_LEN;
				/* we will search bad data seg for a header */
				return BADPKT;
			} else {
				CLRALRM();	/* got it all... return */
				nlost = 0;
				low += need;
				if (*lenp)
					printmsg(M_DATA, "|%s|",
					   visbuf(buf + ENV_LEN, *lenp));
				return tt;
			}
		}
	}
}

/*
 * gspack simply sends out a packet, by assembling an envelope and writing
 * it, and the data segment, to the communications channel.
 */
gspack(tt, xxx, yyy, data, size)
int tt, xxx, yyy; 
unsigned char *data; 
int size;
{
	unsigned char envelope[ENV_LEN];	/* packet envelope */
	unsigned sum;				/* checksum */
	unsigned char ctrl;			/* header control byte */
	int k;					/* size = 32 * 2^(k-1) */

	ctrl = ((tt & 3) << 6) | ((xxx & 7) << 3) | (yyy & 7);
	if (0 == size) {
		k = 9;				/* no data seg */
		sum = MAGIC - ctrl;
	} else {
		int tmp = size;
		for(k = -5; tmp != 0; ++k)		/* find k */
			tmp >>= 1;
		sum = MAGIC - (cksum(data, size) ^ ctrl);
	}
	envelope[ENV_DLE] = DLE;
	envelope[ENV_K]   = k;
	envelope[ENV_C1]  = (sum >> 8) & 0xFF;
	envelope[ENV_C0]  = sum & 0xFF;
	envelope[ENV_C]   = ctrl;
	envelope[ENV_X]   = k ^ envelope[ENV_C0] ^ envelope[ENV_C1] ^ ctrl;
	swrite(envelope, ENV_LEN);		/* send envelope */
	if (0 != size)
		swrite(data, size);		/* send data segment */
	printmsg(M_LOWPROTO, "gspack: tt=%d xxx=%d yyy=%d k=%d sum=%x",
	tt, xxx, yyy, k, sum);
	if (0 != size)
		printmsg(M_DATA, "|%s|", visbuf(data, size));
}

/*
 * chksum(data, len) came directly from dcp. It checksums the given data
 * area using the g protocol algorithm.
 */
unsigned cksum(data, len)
int len; 
unsigned char *data;
{
	unsigned int i, j, tmp, chk1, chk2;
	chk1 = 0xffff;
	chk2 = 0;
	j = len;
	for (i = 0; i < len; i++) {
		if (chk1 & 0x8000) {
			chk1 <<= 1;
			chk1++;
		} else
			chk1 <<= 1;
		tmp = chk1;
		chk1 += (data[i] & 0xff);
		chk2 += chk1 ^ j;
		if ((chk1 & 0xffff) <= (tmp & 0xffff))
			chk1 ^= chk2;
		j--;
	}
	return (chk1 & 0xffff);
}

/* |ilog2(value)| returns (int)floor(log2(value)).
 */
int ilog2(value)
unsigned value;
{
	int i;

	if (value == 0)
		return -1;
	for(i = 0; value > 1; ++i)
		value >>= 1;
	return i;
}
