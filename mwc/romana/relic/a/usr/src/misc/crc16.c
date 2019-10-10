/*
 * Tables for the table-driven CRC16 algorithm.
 * This should be relatively uniform statistically.
 */
static	unsigned short crctab1[] = {
	0000000,	0140301,	0140601,	0000500,
	0141401,	0001700,	0001200,	0141101,
	0143001,	0003300,	0003600,	0143501,
	0002400,	0142701,	0142201,	0002100
};

static	unsigned short crctab2[] = {
	0000000,	0146001,	0154001,	0012000,
	0170001,	0036000,	0024000,	0162001,
	0120001,	0066000,	0074000,	0132001,
	0050000,	0116001,	0104001,	0043000
};

/*
 * Read a string return the hash value computed using CRC-16 methods.
 * usefull for various kinds of hashing including diff programs
 * and symbol tables.
 */
unsigned short
crc16(p)
register char *p;
{
	register unsigned char tmp, c;
	register unsigned short h;

	for(h = 0; c = *p; p++) {
		tmp = c ^ h;
		h = (h >> 8) ^ crctab1[tmp & 15] ^ crctab2[tmp >> 4];
	}
	return(h);
}
