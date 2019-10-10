/* ld/pass1.c */

#include "data.h"

/*
 * Read input file
 * If not archive, add it to module list
 * If archive, add all modules that satisfy global references
 * iterating until no more can be satisfied
 * If it is not a ranlib, this must be done the hard way
 */
int
rdfile( fname )
char  * fname;
{
	fsize_t	offs, arhend;
	arh_t	arh;
	struct	stat	arstat;
	int	magic, found;
	FILE	*ifp;

	if ( (ifp = fopen(fname, "r")) == NULL )
		return (0);

	setbuf( ifp, pbuf1 );
	fstat( fileno(ifp), &arstat );

	if ( fread( &magic, sizeof(magic), 1, ifp ) != 1 )
		fatal( "can't read %s", fname );

	canint( magic );

	/*
	 * Not a library.
	 */
	if ( magic != ARMAG ) {
		rewind( ifp );
		addmod( ifp, (fsize_t)0, fname, "" );
	}

	/*
	 * Library header extracted.
	 */
	else if ( fread( &arh, sizeof(arh), 1, ifp ) == 1 ) {

		cantime( arh.ar_date );
		cansize( arh.ar_size );
		arhend = sizeof(magic) + sizeof(arh) + arh.ar_size;

		/*
		 * Not random library.
		 */
		if ( strncmp( arh.ar_name, HDRNAME, DIRSIZ ) != 0 ) {
			arhend = sizeof(magic);
		}
		
		/*
		 * Outdated random library.
		 */
		else if ( arstat.st_mtime > arh.ar_date+SLOPTIME ) {
			filemsg( fname, "outdated ranlib" );
		}

		/*
		 * Perform exhaustive search of random libraries symbol table.
		 */
		else {
			FILE	*xfp = fopen( fname, "r" );
			ars_t	ars;

			setbuf( xfp, pbuf2 );

			if ( watch )
				filemsg( fname, "ranlib" );

			/*
			 * Scan random symbol table.
			 */
			do {
				found = 0;
				fseek( xfp, offs=sizeof(magic)+sizeof(arh), 0);

				for ( ; offs < arhend; offs += sizeof(ars) ) {

					if (fread(&ars,sizeof(ars),1,xfp) != 1)
						break;

					if ( symref( &ars ) == NULL ) 
						continue;

					cansize( ars.ar_off );
					fseek( ifp, ars.ar_off+=arhend, 0 );

					/*
					 * At end of random symbol table.
					 */
					if (fread(&arh,sizeof(arh),1,ifp) != 1)
						break;

					found |= addmod( ifp,
						ars.ar_off += sizeof(arh),
						fname, arh.ar_name );
				}
			} while ( nundef && found );

			fclose( xfp );
			fclose( ifp );
			return( 1 );
		}

		/*
		 * Perform exhaustive search of sequential library.
		 */
		do {
			found = 0;
			fseek( ifp, offs = arhend, 0 );

			while ( offs < arstat.st_size ) {

				/*
				 * At end of library.
				 */
				if ( fread( &arh, sizeof(arh), 1, ifp ) != 1 )
					break;

				cansize( arh.ar_size );
				offs += sizeof(arh);
				found |= modref(ifp, offs, fname, arh.ar_name);
				fseek( ifp, offs += arh.ar_size, 0 );
			}
		} while ( nundef && found );
	}

	fclose( ifp );
	return( 1 );
}

/*
 * Check whether module satisfies any global references.
 * If so, add the module to the list and return flag indicating success
 */
int
modref( fp, offs, fname, mname )
FILE	*fp;
fsize_t	offs;
char	*fname, mname[];
{
	ldh_t	ldh;
	lds_t	lds;
	unsigned int	nsym;
	int	i;

	/*
	 * Read module header.
	 */
	if( fread( &ldh, sizeof(ldh), 1, fp ) != 1 ) {
		modmsg( fname, mname, "can't read" );
		return( 0 );
	}

	canint( ldh.l_magic );

	if ( ldh.l_magic != L_MAGIC ) {
		modmsg( fname, mname, "bad header" );
		return( 0 );
	}

	for ( i = 0; i < NLSEG; i++ )
		cansize( ldh.l_ssize[i] );

	/*
	 * Prepare to read module's symbol table.
	 */
	fseek( fp, symoff(&ldh), 1 );

	/*
	 * Scan module's symbol table.
	 */
	for ( nsym = ldh.l_ssize[L_SYM] / sizeof(lds); nsym; nsym-- ) {

		/*
		 * Read module symbol table entry.
		 */
		if ( fread( &lds, sizeof(lds), 1, fp ) != 1 ) {
			modmsg( fname, mname, "bad symbol segment" );
			return( 0 );
		}

		canint( lds.ls_type );

		/*
		 * Add module if it contains a global referenced symbol.
		 */
		if ( (lds.ls_type & L_GLOBAL)
		  && (lds.ls_type != (L_GLOBAL|L_REF))
		  && (symref(&lds) != NULL) ) {
			fseek( fp, offs, 0 );
			return( addmod( fp, offs, fname, mname ) );
		}
	}

	return( 0 );
}

/*
 * Allocate module descriptor and add to end of list.
 */
int
addmod( fp, offs, fname, mname )
FILE	*fp;
fsize_t	offs;
char	*fname, mname[];
{
	mod_t	*mp;
	ldh_t	ldh;
	lds_t	lds;
	int	i, segn;
	unsigned int	nsym;
	sym_t	*tmsym;
	
	if ( watch ) {
		modmsg( fname, mname, "adding" );
	}

	/*
	 * Read module header.
	 */
	if ( fread( &ldh, sizeof(ldh), 1, fp ) != 1 ) {
		modmsg( fname, mname, "can't read" );
		return( 0 );
	}

	canint( ldh.l_magic );
	canint( ldh.l_flag );
	canint( ldh.l_machine );

	for ( i = 0; i < NLSEG; i++ )
		cansize( ldh.l_ssize[i] );

	if ( ldh.l_magic != L_MAGIC ) {
		modmsg( fname, mname, "bad header" );
		return( 0 );
	}

	if ( ldh.l_flag & LF_SEP ) {
		modmsg( fname, mname, "cannot load separated I/D" );
		return( 0 );
	}

	/*
	 * Machine type not defined yet.
	 */
	if ( oldh.l_machine == 0 ) {

		/*
		 * Define machine type and byte ordering.
		 */
		switch ( oldh.l_machine = ldh.l_machine ) {

		case M_PDP11:
		case M_8086:
			worder = LOHI;
			break;

		case M_Z8002:
			worder = HILO;
			break;

		default:
			modmsg( fname, mname, "can't load %s code (yet)",
				mchname[ldh.l_machine] );
			exit( 1 );
		}
	}
	
	/*
	 * Mismatch on machine type.
	 */
	else if ( ldh.l_machine != oldh.l_machine ) {
		modmsg( fname, mname, "inconsistent machine" );
		return( 0 );
	}

	/*
	 * Allocate descriptor, add to list, initialize
	 */
	nsym = ldh.l_ssize[L_SYM] / sizeof(lds_t);

	if ( mdisk ) {
		mp = mtemp;
		nmod++;
		fseek( mfp, (long) sizeof(mod_t), 1 );
	}

	else {
		if ( (mp=malloc(sizeof(mod_t)+nsym*sizeof(sym_t *))) == NULL )
			fatal("out of space");

		if ( modhead == NULL )
			modhead = mp;

		else
			modtail->next = mp;

		modtail = mp;
		mp->next = NULL;
	}

	mp->fname = fname;

	for ( i = 0; i < DIRSIZ; i++ )
		mp->mname[i] = mname[0] ? mname[i] : 0;

	for ( i = 0; i < NLSEG; i++ )
		mp->seg[i].size = ldh.l_ssize[i];

	baseall( mp->seg, &ldh );
	segoffs( mp->seg, offs );
	mp->nsym = nsym;

	/*
	 * Prepare to read module symbol table.
	 */
	fseek( fp, symoff(&ldh), 1 );

	/*
	 * Add symbols
	 */
	for ( i = 0; i < nsym; i++ ) {

		/*
		 * Read entry from module symbol table.
		 */
		if ( fread( &lds, sizeof(lds), 1, fp ) != 1 ) {
			modmsg( fname, mname, "bad symbol segment" );
			mp->nsym = i;
			break;
		} else {
			canint( lds.ls_type );
			canvaddr( lds.ls_addr );
			segn = lds.ls_type & ~L_GLOBAL;

			/*
			 * Make symbols segment relative
			 */
			if ( segn < L_SYM )
				lds.ls_addr += oseg[segn].size
						- mp->seg[segn].vbase;

			if ( mdisk ) {
				tmsym = addsym( &lds, mp );

				if ( fwrite(&tmsym,sizeof(tmsym),1,mfp) != 1 )
badisk:					fatal("disk error");
			} else
				mp->sym[i] = addsym( &lds, mp );
		}
	}

	for ( i = 0; i < NLSEG; i++ )
		if ( i != L_SYM )
			oseg[i].size += ldh.l_ssize[i];

	if ( mdisk ) {
		if ( mxmsym < mp->nsym )
			mxmsym = mp->nsym;

		fseek( mfp,(long)-(sizeof(mod_t))-(mp->nsym*sizeof(tmsym)),1 );

		if( fwrite( mtemp, sizeof(mod_t), 1, mfp ) != 1 )
			goto badisk;

		fseek( mfp, (long) mp->nsym * sizeof(tmsym), 1 );
	}

	return (1);
}

/*
 * Add symbol to symbol table
 * If match, resolve references and definitions
 * otherwise, create new symbol entry.
 * Keep track of number of undefined symbols,
 * amount of space required by commons,
 * size of output symbol segment.
 */
sym_t *
addsym( lsp, mp )
register lds_t	*lsp;
mod_t	*mp;
{
	register sym_t	*sp;
	int h;

	h = hash( lsp->ls_id );

	/*
	 * Not a global symbol.
	 */
	if ( !(lsp->ls_type & L_GLOBAL) ) {

		/*
		 * Discard if local symbol and required to.
		 */
		if ( nolcl || noilcl && (lsp->ls_id[0] == 'L') )
			return( NULL );
	}
	
	/*
	 * Scan internal symbol table.
	 */
	else for ( sp = symtable[h]; sp != NULL; sp = sp->next ) {

		if ((sp->s.ls_type & L_GLOBAL) && eq(sp->s.ls_id,lsp->ls_id)) {

			if ( lsp->ls_type != (L_GLOBAL|L_REF) ) {

				/*
				 * New def, old def
				 */
				if ( sp->s.ls_type != (L_GLOBAL|L_REF) )
					symredef( sp, mp );

				/*
				 * New def, old ref
				 * common compatible with def
				 */
				else if ( (sp->s.ls_addr == 0)
				 || commdef(sp->mod, mp, lsp)) {
					if ( sp->s.ls_addr == 0 )
						nundef--;
					sp->s.ls_type = lsp->ls_type;
					sp->s.ls_addr = lsp->ls_addr;
					sp->mod = mp;
				}
			}
			
			/*
			 * New ref, old def
			 */
			else if ( sp->s.ls_type != (L_GLOBAL|L_REF) ) {
				if ( lsp->ls_addr != 0 )
					commdef( mp, sp->mod, &sp->s );
			}
			
			/*
			 * New ref > old ref
			 */
			else if ( sp->s.ls_addr < lsp->ls_addr ) {

				/*
				 * Ref becomes comm
				 */
				if ( sp->s.ls_addr == 0 )
					nundef--;

				commons += lsp->ls_addr - sp->s.ls_addr;
				sp->s.ls_addr = lsp->ls_addr;
				sp->mod = mp;
			}

			return( sp );
		}
	}

	/*
	 * Symbol not found (or is local)
	 */
	if ( (sp = newsym()) == NULL )
		fatal("out of space - S");

	oseg[L_SYM].size += sizeof(lds_t);
	sp->next = symtable[h];
	symtable[h] = sp;
	sp->s = *lsp;	/* struct assign */
	sp->mod = mp;

	/*
	 * Note reference to internal symbol 'etext', 'edata', or 'end'.
	 */
	if ( eq( sp->s.ls_id, etext_id ) )
		etext_s = sp;
	else if ( eq( sp->s.ls_id, edata_id ) )
		edata_s = sp;
	else if ( eq( sp->s.ls_id, end_id ) )
		end_s = sp;

	/*
	 * Note reference to global symbol.
	 */
	else if ( sp->s.ls_type == (L_GLOBAL|L_REF) ) {
		if ( sp->s.ls_addr == 0 )
			nundef++;
		else
			commons += (uaddr_t)(sp->s.ls_addr);
	}

	return( sp );
}

/*
 * Allocate a new sym_t from sym_t blocks to reduce malloc overhead.
 */
sym_t *
newsym()
{
	static sym_t *symb;
	static int nsym;

	/*
	 * Symbol cluster exhaused.
	 */
	if ( nsym == 0 ) {
		/*
		 * Allocate new symbol cluster.
		 */
		if ( (symb = malloc(64*sizeof(sym_t))) == NULL )
			return (NULL);
		nsym = 64;
	}

	/*
	 * Allocate symbol table entry.
	 */
	nsym -= 1;
	return( symb++ );
}

/*
 * complain about redefined symbol
 */
void
symredef( sp, mp )
sym_t	*sp;
mod_t	*mp;
{
	/*
	 * Simple module.
	 */
	if ( mp->mname[0] == '\0' )
		spmsg( sp, "redefined in file %s", mp->fname );

	/*
	 * Library module.
	 */
	else
		spmsg( sp, "redefined in file %s: module %.*s",
			mp->fname, DIRSIZ, mp->mname );
}

/*
 * Check if common compatible with symbol definition
 */
int
commdef( cmp, dmp, lsp )
mod_t	*cmp, *dmp;
lds_t	*lsp;
{
	int type = lsp->ls_type & ~L_GLOBAL;

	/*
	 * Compatible if not private code or shared data.
	 */
	if ( (type != L_PRVI) && (type != L_SHRI) )
		return (1);

	/*
	 * Incompatible simple module.
	 */
	if ( dmp->mname[0] == 0 ) {
		mpmsg( cmp, "common %.*s: conflicts with code in file %s",
			NCPLN, lsp->ls_id, dmp->fname );
	}

	/*
	 * Incompatible library module.
	 */
	else {
		mpmsg( cmp,
		    "common %.*s: conflicts with code in file %s: module %.*s",
		    NCPLN, lsp->ls_id, dmp->fname, DIRSIZ, dmp->mname );
	}

	return (0);
}

/*
 * Add system symbols to symbol table
 * Make definitions absolute
 */
void
rdsystem( sysnam )
char * sysnam;
{
	FILE	*fp;
	ldh_t	ldh;
	lds_t	lds;
	sym_t	*sp;
	int	i;

	if ( (fp = fopen(sysnam, "r")) == NULL )
		fatal("can't open %s", sysnam);

	/*
	 * Allow null input file
	 */
	if ( fread( &ldh, sizeof(ldh), 1, fp ) != 1 )
		return;

	canint( ldh.l_magic );

	/*
	 * Not a program.
	 */
	if ( ldh.l_magic != L_MAGIC ) {
		filemsg( sysnam, "bad header" );
		return;
	}

	canint( ldh.l_machine );

	/*
	 * Not the right machine.
	 */
	if ( ldh.l_machine != oldh.l_machine ) {
		filemsg( sysnam, "inconsistent machine" );
		return;
	}

	for ( i = 0; i < NLSEG; i++ )
		cansize( ldh.l_ssize[i] );

	/*
	 * Prepare to read the system symbol table.
	 */
	fseek( fp, symoff(&ldh), 1 );

	/*
	 * Read system symbol table, one entry at a time.
	 */
	for ( i = ldh.l_ssize[L_SYM] / sizeof(lds); i; i-- ) {

		/*
		 * Couldn't read entry from system symbol table.
		 */
		if ( fread( &lds, sizeof(lds), 1, fp ) != 1 ) {
			filemsg( sysnam, "bad symbol segment" );
			return;
		}

		else {
			canint( lds.ls_type );
			canvaddr( lds.ls_addr );

			/*
			 * If system symbol is global and is referenced,
			 *	update reference offset.
			 */
			if ( (lds.ls_type & L_GLOBAL)
			  && (lds.ls_type != (L_GLOBAL|L_REF))
			  && ((sp = symref(&lds)) != NULL) ) {
				sp->s.ls_type = L_GLOBAL | L_ABS;
				sp->s.ls_addr = lds.ls_addr;
				nundef--;
			}
		}
	}
}

/*
 * Return reference to given symbol if any
 */
sym_t *
symref( ldsp )
union {lds_t lds_f; ars_t ars_f;} *ldsp;
{
	register sym_t	*sp;

	/*
	 * Scan internal symbol table.
	 */
	for ( sp=symtable[hash(ldsp->lds_f.ls_id)]; sp != NULL; sp=sp->next ) {

		/*
		 * Reference to given [global] symbol found.
		 */
		if ( (sp->s.ls_type == (L_GLOBAL|L_REF))
		  && (eq(sp->s.ls_id, ldsp->lds_f.ls_id))
		  && (sp->s.ls_addr == 0) )
			break;
	}

	/*
	 * Return reference, or NULL.
	 */
	return( sp );
}

/*
 * Compute hash index into symbol table by summing chars in name
 * (mod table length)
 */
int
hash( id )
char  id[];
{
	register char * s = &id[0];
	register int h;
	int i;

	/*
	 * Sum all characters in symbol name.
	 */
	for ( h = i = 0; *s && (i < NCPLN); i++ )
		h += *s++;

	/*
	 * Convert character sum to hash.
	 */
	return( h % NHASH );
}

/*
 * Calculate file offset of symbol segment relative to end of header
 */
fsize_t
symoff( ldhp )
register ldh_t	*ldhp;
{
	register fsize_t	offs = 0;
	register int i;

	/*
	 * Scan all segments up to [but not including] the symbol segment.
	 */
	for ( i = 0; i < L_SYM; i++ ) {

		/*
		 * Uninitialized code/data do not occupy space in the file.
		 */
		if ( (i == L_BSSI) || (i == L_BSSD) )
			continue;

		/*
		 * Adjust offset by segment size.
		 */
		else
			offs += ldhp->l_ssize[i];
	}

	/*
	 * Return offset of symbol segment.
	 */
	return( offs );
}
