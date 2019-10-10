/* l.out.c -- routines for manipulating l.out executable files.  */

#include <l.out.h>
#include <canon.h>
#include <sys/types.h>

#include "tboot.h"

/* Convert l.out to load table.
 * Used to generate loading instructions for use by tboot main().
 * Returns true on successful translation.
 */

int
lout2load(ip, table, data_seg)
	struct inode *ip;		/* input: File to read.		*/
	struct load_segment table[];	/* output: How to read it.	*/
	uint16 *data_seg;	/* output: Where to point es.	*/
{
	int i;				/* Loop counter.  */
	struct ldheader imageheader;	/* l.out header for boot image.  */

#ifdef VERBOSE
	puts("Reading the l.out header.\r\n");
#endif /* VERBOSE */

	/* Read the header.  */
	iread(ip, &imageheader,
	      (fsize_t) 0, (uint16) sizeof(struct ldheader));


	/* Canonicalize the header.  */
	canint(imageheader.l_magic);
	canint(imageheader.l_flag);
	canint(imageheader.l_machine);
	canvaddr(imageheader.l_entry);
	for(i = 0; i < NLSEG; ++i) {
		cansize(imageheader.l_ssize[i]);
	}

	/* Copy the l.out header info into table.  */

#define L_TEXT table[0]
#define L_DATA table[1]
	/* Calculate remaining entries.  */
	if (imageheader.l_flag & LF_SEP) { /* if sep i/d executable */
#ifdef VERBOSE
		puts("Generating table for sep i/d l.out.\r\n");
#endif /* VERBOSE */
		L_TEXT.valid = (1==1);
		L_TEXT.message = "\r\nLoading l.out code segments...\r\n";
		/* Load the shared and private code segments as one.  */
		L_TEXT.load_toseg = sys_base; /* This is where we want the OS.  */
		L_TEXT.load_tooffset = 0;
		L_TEXT.load_offset = sizeof(struct ldheader); /* Skip the header.  */
		L_TEXT.load_length = imageheader.l_ssize[L_SHRI] + /* Both segments as one.  */
			      imageheader.l_ssize[L_PRVI];

		
		L_DATA.valid = (1==1);
		L_DATA.message = "\r\nLoading l.out data segments...\r\n";
		/* Load both data segments.  */

		/* Round up to next 16 byte paragraph.  */
		L_DATA.load_toseg = (sys_base +
			(imageheader.l_ssize[L_SHRI] + /* Shared code */
			imageheader.l_ssize[L_PRVI] +  /* Private code */
			15) / 16);
		L_DATA.load_tooffset = 0;
		L_DATA.load_offset = (fsize_t) sizeof(struct ldheader) + /* l.out header */
			imageheader.l_ssize[L_SHRI] + /* Shared code */
			imageheader.l_ssize[L_PRVI]; /* Private code */
		L_DATA.load_length = imageheader.l_ssize[L_SHRD] + /* Both segments as one.  */
			imageheader.l_ssize[L_PRVD];

		table[2].valid = (1==2);	/* Terminate the list.  */
		
		*data_seg = (uint16) (sys_base +
			(imageheader.l_ssize[L_SHRI] +	/* Shared code */
			 imageheader.l_ssize[L_PRVI] +	/* Private code */
			 15) / 16);	/* Rounded up a paragraph.  */

	} else { /* if not sep i/d executable */
#define SEGMENT table[0]
		
#ifdef VERBOSE
		puts("Generating table for non-sep i/d l.out.\r\n");
#endif /* VERBOSE */

		SEGMENT.valid = (1==1);
		SEGMENT.message = "\r\nLoading all l.out segments...\r\n";
		/* Load the shared and private code segments as one.  */
		SEGMENT.load_toseg = sys_base; /* This is where we	*/
		SEGMENT.load_tooffset = 0;	/* want the OS.		*/
		/* Skip the header.  */
		SEGMENT.load_offset = (fsize_t) sizeof(struct ldheader);
		/* Load all segments as one.  */
		SEGMENT.load_length = imageheader.l_ssize[L_SHRI] +
		       imageheader.l_ssize[L_PRVI] +
		       imageheader.l_ssize[L_SHRD] +
		       imageheader.l_ssize[L_PRVD];

		table[1].valid = (1==2);	/* Terminate the list.  */

		/* Tiny model: ds = cs */
		*data_seg = sys_base;
	} /* if not sep i/d executable */

	return (1==1);
}


/*
 * Get entries from l.out name list.
 */

void
l_out_nlist(fn, nlp)
char *fn;
struct nlist *nlp;
{
	register struct nlist *np;
	register int ntodo = 0;
	int lfp;
	struct ldheader lh;
	struct ldsym ste;
	fsize_t symsize;
	register int n;

	for (np = nlp; np->n_name[0] != '\0'; np++) {
		np->n_type = np->n_value = 0;
		ntodo++;
	}
	
	if ((lfp = open(fn, 0)) == -1)
		return;


	n = read(lfp, &lh, sizeof lh);

	canint(lh.l_magic);
	if (n!=sizeof(lh) || lh.l_magic!=L_MAGIC) {
		puts("L.out bad MAGIC.\r\n");
		close(lfp);
		return;
	}
	for (n=0; n<=L_SYM; n++)
		cansize(lh.l_ssize[n]);
	symsize = sizeof lh + lh.l_ssize[L_SHRI] + lh.l_ssize[L_SHRD]
	    + lh.l_ssize[L_PRVI] + lh.l_ssize[L_PRVD] + lh.l_ssize[L_DEBUG];
	lseek(lfp, symsize, 0);
	symsize = lh.l_ssize[L_SYM];
	for ( ; symsize>0 && ntodo; symsize -= sizeof ste) {
		if (read(lfp, &ste, sizeof ste) != sizeof(ste))
			break;
		for (np = nlp; np->n_name[0] != '\0'; np++)
			if (strncmp(np->n_name, ste.ls_id, NCPLN) == 0) {
				canint(ste.ls_type);
				canvaddr(ste.ls_addr);
				np->n_type = ste.ls_type;
				np->n_value = ste.ls_addr;
				if (--ntodo == 0)
					break;
			}
	}
	close(lfp);
}
