
/* Fill a region of memory with zeros.  */
zero_fill(from, to)
char *from;
char *to;
{
        register char *p;

        p = from;
        while (p<to) {
                *p = 0;
                p++;
        }
}

/* Read a 16 byte number from the CMOS.  */
unsigned int
read16_cmos(addr)
unsigned int addr;
{
        unsigned char read_cmos();
	
	return((read_cmos(addr+1)<<8) + read_cmos(addr));
} /* read16_cmos() */


