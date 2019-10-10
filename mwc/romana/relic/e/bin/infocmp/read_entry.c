/*********************************************************************
*                         COPYRIGHT NOTICE                           *
**********************************************************************
*        This software is copyright (C) 1982 by Pavel Curtis         *
*                                                                    *
*        Permission is granted to reproduce and distribute           *
*        this file by any means so long as no fee is charged         *
*        above a nominal handling fee and so long as this            *
*        notice is always included in the copies.                    *
*                                                                    *
*        Other rights are reserved except as explicitly granted      *
*        by written permission of the author.                        *
*                Pavel Curtis                                        *
*                Computer Science Dept.                              *
*                405 Upson Hall                                      *
*                Cornell University                                  *
*                Ithaca, NY 14853                                    *
*                                                                    *
*                Ph- (607) 256-4934                                  *
*                                                                    *
*                Pavel.Cornell@Udel-Relay   (ARPAnet)                *
*                decvax!cornell!pavel       (UUCPnet)                *
*********************************************************************/

/*
 *	read_entry.c -- Routine for reading in a compiled terminfo file
 *
 *  $Log:	read_entry.c,v $
 * Revision 1.1  92/03/13  10:21:11  bin
 * Initial revision
 * 
 * Revision 3.2  91/07/28  14:12:20  munk
 * Made the large arrays static
 *
 * Revision 3.1  84/12/13  11:21:14  john
 * Revisions by Mark Horton
 * 
 * Revision 2.1  82/10/25  14:49:55  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/24  15:18:22  pavel
 * Beta-one Test Release
 * 
 * Revision 1.3  82/08/23  22:31:15  pavel
 * The REAL Alpha-one Release Version
 * 
 * Revision 1.2  82/08/19  19:11:49  pavel
 * Alpha Test Release One
 * 
 * Revision 1.1  82/08/12  22:25:13  pavel
 * Initial revision
 * 
 *
 */

#ifndef COHERENT
static char RCSid[] =
	"$Header: /src386/usr/bin/infocmp/RCS/read_entry.c,v 1.1 92/03/13 10:21:11 bin Exp $";
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include "term.h"
#include "object.h"

#define OFFSET_BUFSIZE	100

#define min(a, b)	((a) > (b)  ?  (b)  :  (a))

/*
 *	int
 *	read_entry(filename, ptr)
 *
 *	Read the compiled terminfo entry in the given file into the
 *	structure pointed to by ptr, allocating space for the string
 *	table and placing its address in ptr->str_table.
 *
 */

#define swap(x)		(((x >> 8) & 0377) + 256 * (x & 0377))

static char	TermNames[128];	/* Buffer for terminal names for first term */
static char	StringTable[1024];	/* String table for first terminal  */

int
read_entry(filename, ptr)
char		*filename;
struct term	*ptr;
{
	int		fd;
	int		numread;
	int		num_strings;
	int		cur_string;
	char		*malloc();
	int		i;
	struct header	header;
	unsigned char	bytebuf[2];
	char		ch;
	static union
	{
	    unsigned char    byte[2];
	    short            number;
	}		offset_buf[OFFSET_BUFSIZE];

	fd = open(filename, 0);

	if (fd < 0)
	    return(-1);

	read(fd, &header, sizeof(header));

	if (must_swap())
	{
	    header.magic = swap(header.magic);
	    header.name_size = swap(header.name_size);
	    header.bool_count = swap(header.bool_count);
	    header.num_count = swap(header.num_count);
	    header.str_count = swap(header.str_count);
	    header.str_size = swap(header.str_size);
	}

	if (header.magic != MAGIC)
	{
	    close(fd);
	    return(-1);
	}

	read(fd, TermNames, min(127, header.name_size));
	TermNames[127] = '\0';
	ptr->term_names = TermNames;
	if (header.name_size > 127)
	    lseek(fd, (long) (header.name_size - 127), 1);

	read(fd, ptr->Booleans, min(BOOLCOUNT, header.bool_count));
	if (header.bool_count > BOOLCOUNT)
	    lseek(fd, (long) (header.bool_count - BOOLCOUNT), 1);
	else
	    for (i=header.bool_count; i < BOOLCOUNT; i++)
		ptr->Booleans[i] = 0;

	if ((header.name_size + header.bool_count) % 2 != 0)
	    read(fd, &ch, 1);

	if (must_swap())
	    read(fd, ptr->Numbers, min(NUMCOUNT, header.num_count * 2));
	else
	{
	    for (i=0; i < min(header.num_count, NUMCOUNT); i++)
	    {
		read(fd, bytebuf, 2);
		if (bytebuf[0] == 0377  &&  bytebuf[1] == 0377)
		    ptr->Numbers[i] = -1;
		else
		    ptr->Numbers[i] = bytebuf[0] + 256 * bytebuf[1];
	    }
	}

	if (header.num_count > NUMCOUNT)
	    lseek(fd, (long) (2 * (header.num_count - NUMCOUNT)), 1);
	else
	    for (i=header.num_count; i < NUMCOUNT; i++)
		ptr->Numbers[i] = -1;

	if (cur_term)	/* cur_term is non-zero only if we've been called */
	{
	    ptr->str_table = malloc(header.str_size);
	    if (ptr->str_table == NULL)
	    {
		close(fd);
		return (-1);
	    }
	}
	else
	    ptr->str_table = StringTable;

	num_strings = min(STRCOUNT, header.str_count);
	cur_string = 0;

	while (num_strings > 0)
	{
	    numread = read(fd, offset_buf, 2*min(num_strings, OFFSET_BUFSIZE));
	    if (numread <= 0)
	    {
		close(fd);
		return(-1);
	    }

	    if (must_swap())
	    {
		for (i = 0; i < numread / 2; i++)
		{
		    ptr->Strings[i + cur_string] =
			(offset_buf[i].byte[0] == 0377
					    &&  offset_buf[i].byte[1] == 0377)
			? 0
			: ((offset_buf[i].byte[0] + 256*offset_buf[i].byte[1])
							      + ptr->str_table);
		}
	    }
	    else
	    {
		for (i = 0; i < numread / 2; i++)
		{
		    ptr->Strings[i + cur_string] =
			(offset_buf[i].number == -1)
			?
			    0
			:
			    offset_buf[i].number + ptr->str_table;
		}
	    }

	    cur_string += numread / 2;
	    num_strings -= numread / 2;
	}

	if (header.str_count > STRCOUNT)
	    lseek(fd, (long) (2 * (header.str_count - STRCOUNT)), 1);
	else
	    for (i=header.str_count; i < STRCOUNT; i++)
		ptr->Strings[i] = 0;

	numread = read(fd, ptr->str_table, header.str_size);
	close(fd);
	if (numread != header.str_size)
	    return(-1);

	return(0);
}



/*
 *	int
 *	must_swap()
 *
 *	Test whether this machine will need byte-swapping
 *
 */

int
must_swap()
{
	union
	{
	    short num;
	    char  byte[2];
	}		test;

	test.num = 1;
	return(test.byte[1]);
}
