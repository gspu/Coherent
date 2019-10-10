/* ld/main.c */

#include "data.h"

extern	char	*mktemp();

main( argc, argv )
int	argc;
char	*argv[];
{
	flag_t	dcomm = 0,		/* define commons even if reloc out */
		nosym = 0,		/* do not output symbol table */
		memld = 0,		/* do in-memory load */
		reloc = 0;		/* output relocation info */
	char	*ofname = "l.out",
		*entrys = NULL,
		*mfn,
		*lp;
	seg_t	*sgp;
	sym_t	*sp;
	mod_t	*mp;
	FILE	*ofp;
	struct	stat	statbuf;
	int	i, segn;
	unsigned int	symno;
	fsize_t	daddr;
	uaddr_t	addr;
#if BREADBOX
	int	abort();
#endif

	setbuf( stdout, NULL );
	setbuf( stderr, NULL );

	/*
	 * set separate code/data flag (required for 286 protected mode)
	 */
	oldh.l_flag |= LF_SEP;

	nmod = mxmsym = mdisk = 0;

	if ( argc <= 1 )
		usage( "no args" );

	/*
	 * Pass 1: Scan argument list.
	 */
	for ( i = 1; i < argc; i++ ) {

		/*
		 * Not -option; attempt to read file
		 */
		if ( *argv[i] != '-' ) {
			if ( !rdfile(argv[i]) )
				fatal( "can't open %s", argv[i] );
		}

		/*
		 * Decode options
		 */
		else switch ( argv[i][1] ) {

		default:
			usage( "unknown option %s", argv[i] );
			continue;

		case 'b':
			/*
			 * -b: undocumented option
			 * 	big link stores module structs on disk
			 */
			mdisk++;

			if ( (mfp = fopen(mfn=mktemp("/tmp/ldXXXXXX"),"wr") )
			   ==NULL) {
			   	unlink( mfn );
				fatal( "can't open temp file" );
			}

			mtemp = malloc( sizeof(mod_t) );
			continue;

		case 'd':
			/*
			 * -d: Define commons (even if undefined symbols).
			 */
			dcomm++;
			continue;

		case 'e':
			/*
			 * -e ent: Set entry point to given symbol/octal number.
			 */
			if ( ++i >= argc )
				usage( "Bad -e option" );

			else {
				undef( argv[i] );
				entrys = argv[i];
			}
			continue;

		case 'i':
			continue;

		case 'k':
			/*
			 * -k[sys]: Kernel process - use system symbol table.
			 */
			oldh.l_flag |= LF_KER;

			if ( argv[i][2] != '\0' )
				rdsystem( &argv[i][2] );
			else
				rdsystem( "/coherent" );
			continue;

		case 'l':
			/*
			 * -l<lib>: Use standard library <lib>.
			 */
			lp = malloc( strlen(argv[i])+16 );
			sprintf( lp, "/lib/lib%s.a", &argv[i][2] );

			if ( rdfile(lp) )
				;

			else {
				sprintf( lp, "/usr/lib/lib%s.a", &argv[i][2] );
				if ( rdfile(lp) )
					;
				else
					fatal(	"can't open lib%s.a",
						&argv[i][2]);
			}
			continue;

		case 'm':
			/*
			 * -m: Perform in-memory load if possible.
			 */
			memld++;
			continue;

		case 'n':
			/*
			 * -n: Bind output shared.
			 */
			oldh.l_flag |= LF_SHR;
#ifdef IAPX86
			/*
			 * Special case, no -n without -i on IBM PC
			 */
			oldh.l_flag |= LF_SEP;
#endif
			continue;

		case 'o':
			/*
			 * -o file: Place output in named file (default l.out).
			 */
			if ( ++i >= argc )
				usage( "bad -o option" );
			else
				ofname = argv[i];
			continue;

		case 'r':
			/*
			 * -r: Retain relocation information.
			 */
			reloc++;
#ifdef IAPX86
			/*
			 * Special case, -r disables -i.
			 */
			oldh.l_flag &= ~LF_SEP;
#endif
			continue;

		case 's':
			/*
			 * -s: Discard symbol table.
			 */
			nosym++;
			continue;

		case 'u':
			/*
			 * -u sym: Undefine 'sym' (forcing library search).
			 */
			if ( ++i >= argc )
				usage( "bad -u option" );
			else
				undef( argv[i] );
			continue;

		case 'w':
			/*
			 * -w: Watch everything happen.
			 */
			watch++;
			message( "watch!" );
			continue;

		case 'X':
			/*
			 * -X: Discard C internal local symbols.
			 */
			noilcl++;
			continue;

		case 'x':
			/*
			 * -x: Discard all local symbols.
			 */
			nolcl++;
			continue;

		case 'C':
			/*
		         * -C: Bind output combined code/data.
			 * (otherwise it will be separate code/data as
			 * required in 286 protected mode)
			 */
			oldh.l_flag &= ~LF_SEP;
			continue;
		}
	}

	if ( oldh.l_machine == 0 )
		fatal( "no input found" );

	/*
	 * all modules have been read
	 * resolve meanings of various flags
	 */

	/*
	 * Relocation or undefined symbols...
	 */
	if ( reloc || nundef ) {

		/*
		 * Requires symbol table be retained
		 */
		nosym = 0;

		if ( oldh.l_flag & LF_SEP ) {

			message("link failed, the following symbols are undefined:");

			for ( i = 0; i < NHASH; i++ ) {
				for ( sp=symtable[i]; sp!=NULL; sp=sp->next ) {
					if(sp->s.ls_type==(L_GLOBAL|L_REF)
					   && sp->s.ls_addr==0)
					   	spmsg(sp,"");
				}
			}

			if ( nundef )
				exit( 1 );
		}
	}

	/*
	 * No relocation, no undefined symbols
	 */
	else {
		/*
		 * Must define commons
		 */
		dcomm++;

		/*
		 * Discarding reloc info
		 */
		if ( oseg[L_REL].size != 0 ) {
			oldh.l_flag |= LF_NRB;		/* make note */
			oseg[L_REL].size = 0;
		}
	}

	/*
	 * To define common symbols...
	 */
	if ( dcomm )
		oseg[L_BSSD].size += commons;	/* we need this much room */

	/*
	 * Discard symbol table.
	 */
	if ( nosym )
		oseg[L_SYM].size = 0;

	if ( watch )
		message( "# undefined=%d", nundef );

	/*
	 * Set segment bases
	 */
	baseall( oseg, &oldh );

#ifdef IAPX86
	if ( oldh.l_machine == M_8086 ) {
		if ( (oseg[L_SHRI].size +
		      oseg[L_PRVI].size +
		      oseg[L_BSSI].size) >= MAXSEG86 )
			fatal("FATAL: code segment too large");

		if ( (oseg[L_SHRD].size +
		      oseg[L_PRVD].size +
		      oseg[L_BSSD].size) >= (MAXSEG86 - DEFSTACK) )
			fatal("FATAL: data segment too large");

		if ( (oseg[L_SHRD].size +
		      oseg[L_PRVD].size +
		      oseg[L_BSSD].size) >= (MAXSEG86 - DEFSTACK - WARNSIZE) )
			message(
			   "Warning: data segment may be too large to execute");
	}
#endif

	/*
	 * Open output file
	 */
	if ( ((ofp = fopen(ofname, "w")) == NULL)
	 &&  ((unlink(ofname) != 0) || ((ofp = fopen(ofname, "w")) == NULL)) )
		fatal( "cannot create %s", ofname );

	stat( ofname, &statbuf );
	chmod( ofname, statbuf.st_mode & ~(S_IEXEC|(S_IEXEC>>3)|(S_IEXEC>>6)));
	setbuf( ofp, NULL );

	/*
	 * set disk offsets of segments
	 * if not bigger than a breadbox, build output file in memory;
	 * otherwise, open the file for each segment,
	 * to get independent buffering
	 */
	daddr = segoffs( oseg, 0L );

	if ( watch )
		message( "size: %ld", daddr );

#if BREADBOX
	if ( memld || (daddr < BREADBOX) )
		outbuf = malloc( (int) daddr );
#endif
	for ( i = 0; i < NLSEG; i++ ) {

		if ( (i == L_BSSI) || (i == L_BSSD) )
			continue;

		sgp = &oseg[i];

		if ( sgp->size == 0 )
			continue;

#if BREADBOX
		if ( outbuf != NULL ) {
			if ( (outputf[i] = malloc(sizeof(FILE))) == NULL )
				fatal("out of space");

			outputf[i]->_cc = sgp->size;
			outputf[i]->_cp = &outbuf[sgp->daddr];
			outputf[i]->_pt = &abort;
		}
		else
#endif
		{
			if ( (outputf[i] = fopen(ofname, "r+w")) == NULL )
				fatal( "cannot open %s (seg %d)", ofname, i );

			fseek( outputf[i], sgp->daddr, 0 );

			if ( i == L_SHRI )
				setbuf( outputf[i], pbuf1 );

			if ( i == L_PRVD )
				setbuf( outputf[i], pbuf2 );
		}
	}

	/*
	 * Define internal symbols which have been referenced
	 */
	if ( dcomm ) {
		i = oldh.l_flag & LF_KER;
		endbind( etext_s, L_PRVI, i );
		endbind( edata_s, L_PRVD, i );
		endbind( end_s,   L_BSSD, i );
	}

	/*
	 * Run through symbol table fixing everything up
	 */
	symno = 0;
	for ( i=0; i < NHASH; i++ ) {

		for ( sp = symtable[i]; sp != NULL; sp = sp->next ) {

			/*
			 * Make defined symbol relative to virtual 0
			 */
			if ( (segn = sp->s.ls_type & ~L_GLOBAL) < L_SYM ) {
				sp->s.ls_addr += oseg[segn].vbase;
			}

			else if ( segn == L_SYM ) {
				spmsg( sp, "references symbol segment" );
			}

			/*
			 * Leave absolutes alone
			 */
			else if ( sp->s.ls_type != (L_GLOBAL|L_REF) ) {
				;
			}

			/*
			 * Undefined global
			 */
			else if ( sp->s.ls_addr == 0 ) {
				if ( ! reloc )
					spmsg( sp, "undefined" );
			}

			/*
			 * Define commons
			 */
			else if ( dcomm ) {
				addr = sp->s.ls_addr;
				sp->s.ls_addr = oseg[L_BSSD].vbase
						+oseg[L_BSSD].size
						-commons;
				commons -= addr;
				sp->s.ls_type = L_GLOBAL | L_BSSD;
			}

			/*
			 * Assign symbol number
			 */
			sp->symno = symno++;

			/*
			 * Output to symbol segment
			 */
			if ( ! nosym ) {
				canint( sp->s.ls_type );
				canvaddr( sp->s.ls_addr );
				putstruc( &sp->s, sizeof(sp->s),
					outputf[L_SYM], &oseg[L_SYM] );
				canint( sp->s.ls_type );
				canvaddr( sp->s.ls_addr );
			}
		}
	}

	/*
	 * get entry point (cannot precede symbol table fixup)
	 */
	oldh.l_entry = (entrys != NULL)
			? lentry(entrys)
			: (oldh.l_flag & LF_KER) ? drvbase[oldh.l_machine] : 0;
	/*
	 * Pass 2
	 */
	if ( mdisk ) {

		free( mtemp );

		if ((mp=malloc(sizeof(mod_t)+ mxmsym*sizeof(sym_t *))) == NULL)
			fatal("out of space - M");

		rewind( mfp );

		for ( i = 0; i < nmod; i++ ) {

			if ( fread( mp, sizeof(mod_t), 1 ,mfp ) != 1 )
baddisk:			fatal( "disk error" );

			if ( fread( mp->sym, sizeof(sym_t *), mp->nsym, mfp )
			   != mp->nsym)
			   	goto baddisk;

			loadmod( mp );
		}

		unlink( mfn );
	}
	
	else {
		for ( mp = modhead; mp != NULL; mp = mp->next )
			loadmod( mp );
	}

	/*
	 * All over but the flushing
	 */
	canint( oldh.l_magic );
	canint( oldh.l_flag );
	canint( oldh.l_machine );
	canvaddr( oldh.l_entry );

	for ( i = 0; i < NLSEG; i++ ) {
		oldh.l_ssize[i] = oseg[i].size;
		cansize(oldh.l_ssize[i]);
	}

	if ( outbuf == NULL )
		fwrite(&oldh, sizeof(oldh), 1, ofp);

	else {
		for ( i = 0; i < sizeof(oldh); i++ )
			outbuf[i] = *((char *)&oldh + i);
		fwrite( outbuf, 1, (int)daddr, ofp );
	}

	/*
	 * Make executable if no undefined symbols, and common storage defined.
	 */
	if ( (nundef == 0) || (dcomm != 0) ) {
		chmod(ofname, statbuf.st_mode|S_IEXEC|(S_IEXEC>>3)|(S_IEXEC>>6));
		return( 0 );
	}

	/*
	 * Supposed to be left relocatable.
	 */
	else if ( reloc )
		return( 0 );

	/*
	 * Undefined symbols left.
	 */
	else
		return( nundef );
}

/*
 * Set virtual bases in array of segments
 * according to flags in header.
 */
void
baseall( sega, ldhp )
register seg_t	sega[];
register ldh_t	*ldhp;
{
	register uaddr_t addr;

	addr = (ldhp->l_flag & LF_KER) ? drvbase[ldhp->l_machine] : 0;

	/*
	 * Shared code segment first.
	 */
	addr = setbase( &sega[L_SHRI], ldhp, addr );

	/*
	 * Shared data segment next if shared but not separate.
	 */
	if ( (ldhp->l_flag & (LF_SHR|LF_SEP)) == LF_SHR )
		addr = setbase( &sega[L_SHRD], ldhp, addr );

	/*
	 * Round up to page boundary if shared.
	 */
	if ( ldhp->l_flag & LF_SHR )
		addr = newpage(ldhp, addr);

	/*
	 * Private code segment next.
	 */
	addr = setbase( &sega[L_PRVI], ldhp, addr );

	/*
	 * Private data segment next if shared but not separate.
	 */
	if ( (ldhp->l_flag & (LF_SHR|LF_SEP)) == LF_SHR )
		addr = setbase( &sega[L_PRVD], ldhp, addr );

	/*
	 * Uninitialized code segment next.
	 */
	addr = setbase( &sega[L_BSSI], ldhp, addr );

	/*
	 * Set data offset if separate code/data.
	 */
	if ( ldhp->l_flag & LF_SEP )
		addr = (ldhp->l_flag & LF_KER) ? drvbase[ldhp->l_machine] : 0;

	/*
	 * Shared data segment next if not shared.
	 */
	if ( (ldhp->l_flag & (LF_SHR|LF_SEP)) != LF_SHR )
		addr = setbase( &sega[L_SHRD], ldhp, addr );

	/*
	 * Round up to page boundary if shared and separate.
	 */
	if ( (ldhp->l_flag & (LF_SHR|LF_SEP)) == (LF_SHR|LF_SEP) )
		addr = newpage( ldhp, addr );

	/*
	 * Private data segment next unless shared and not separate.
	 */
	if ( (ldhp->l_flag & (LF_SHR|LF_SEP)) != LF_SHR )
		addr = setbase( &sega[L_PRVD], ldhp, addr );

	/*
	 * Uninitialized data segment next.
	 */
	setbase( &sega[L_BSSD], ldhp, addr );
}

/*
 * Set virtual base in given segment and return next address
 * Check for wraparound and driver space overflow
 */
uaddr_t
setbase( segp, ldhp, addr )
register seg_t	*segp;
register ldh_t	*ldhp;
register uaddr_t addr;
{
	segp->vbase = addr;
	addr += segp->size;

	/*
	 * Address wrap-around.
	 */
	if ( addr < segp->vbase ) {
		fatal("address wraparound");
	}
	
	/*
	 * User process.
	 */
	else if ( ! (ldhp->l_flag & LF_KER) ) {
		;
	}
	
	/*
	 * Invalid kernel process.
	 */
	else if (addr > drvtop[ldhp->l_machine]) {
		fatal("driver larger than %dKb",
			(drvtop[ldhp->l_machine]
			-drvbase[ldhp->l_machine])/1024);
	}

	return (addr);
}

/*
 * Set segment disk offsets, given sizes
 * return size of module
 */
fsize_t
segoffs(sega, offs)
seg_t	sega[];
register fsize_t	offs;
{
	register seg_t	*sgp;

	offs += sizeof(ldh_t);

	for ( sgp = &sega[0]; sgp < &sega[NLSEG]; sgp++ ) {

		/*
		 * Uninitialized code/data do not affect module size.
		 */
		if ( (sgp == &sega[L_BSSI]) || (sgp == &sega[L_BSSD]) )
			continue;

		/*
		 * Record segment address.
		 * Calculate module size.
		 */
		else {
			sgp->daddr = offs;
			offs += sgp->size;
		}
	}

	/*
	 * Return module size.
	 */
	return (offs);
}

/*
 * Bump address to next hardware segment boundary
 */
uaddr_t
newpage( ldhp, addr )
ldh_t	*ldhp;
uaddr_t	addr;
{
	register uaddr_t inc = segsize[ldhp->l_machine]-1;

	return ((addr+inc)&~inc);
}

/*
 * Determine entry point; either octal address or symbol name.
 */
uaddr_t
lentry(str)
char	*str;
{
	char	id[NCPLN], *s = &id[0];
	uaddr_t	oaddr=0;
	int	notoct=0;
	register sym_t	*sp;

	/*
	 * Scan string.
	 */
	for ( ; *str; str++ ) {

		if ( s < &id[NCPLN] )
			*s++ = *str;

		/*
		 * Accumulate octal digits.
		 */
		if ( ('0' <= *str) && (*str <= '7') )
			oaddr = oaddr*8 + *str - '0';

		/*
		 * Not an octal digit.
		 */
		else
			++notoct;
	}

	/*
	 * Octal string: return numeric value.
	 */
	if (!notoct)
		return (oaddr);

	/*
	 * Extend string with nulls.
	 */
	while ( s < &id[NCPLN] )
		*s++ = 0;

	/*
	 * Search for string in symbol table.
	 */
	for ( sp = symtable[hash(id)]; sp != NULL; sp = sp->next ) {
		if ( (sp->s.ls_type & L_GLOBAL)
		 &&  (sp->s.ls_type != (L_GLOBAL|L_REF))
		 &&  eq(sp->s.ls_id, id) )
			return( sp->s.ls_addr );
	}

	/*
	 * Symbol not found.
	 */
	message("entry point %.*s undefined", NCPLN, id);
	return (0);
}

/*
 * Define referenced internal symbol (as end of given segment)
 */
void
endbind( sp, sn, ldrv )
register sym_t	*sp;
int	sn, ldrv;
{
	if ( sp == NULL ) {
		return;
	}

	else if ((sp->s.ls_type == (L_GLOBAL|L_REF)) && (sp->s.ls_addr == 0)) {
		sp->s.ls_type = L_GLOBAL|sn;
		sp->s.ls_addr = oseg[sn].size;
	}

	else if ( ldrv ) {
		return;
	}

	else {
		spmsg(sp, "redefines builtin");
	}
}

/*
 * Add reference to symbol table
 */
void
undef( s )
char * s;
{
	lds_t	lds;
	int	i;

	/*
	 * Copy reference, extending with NULL.
	 */
	for ( i = 0; i < NCPLN; i++ ) {
		lds.ls_id[i] = *s;
		if ( *s != '\0' )
			s++;
	}

	/*
	 * Define symbol as global reference.
	 */
	lds.ls_type = L_GLOBAL|L_REF;
	lds.ls_addr = 0;

	/*
	 * Add [undefined] to symbol table.
	 */
	addsym(&lds, NULL);
}
