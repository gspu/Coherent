/*
 * Get entries from coff nlist.
 */
#include <stdio.h>
#include <coff.h>

/*
 * Symbol name.
 */
static char *
symName(sym, str_tab, work)
SYMENT *sym;
char *str_tab, *work;
{
	if (!sym->n_zeroes)
		return (str_tab + sym->n_offset - 4);

	/* make sure it's zero terminated */
	memcpy(work, sym->n_name, SYMNMLEN);
	work[SYMNMLEN] = '\0';
	return (work);
}

coffnlist(fn, nlp, names, count)
char *fn;	/* file name */
SYMENT *nlp;	/* names to look up */
char *names;	/* long names */
int count;	/* size of passed table */
{
	FILEHDR head;
	FILE *fp;
	char *str_tab;
	long str_length;
	int aux, i;

	if (NULL == (fp = fopen(fn, "rb")))
		return(0);

	if (1 != fread(&head, sizeof(head), 1, fp) ||
	    head.f_magic != C_386_MAGIC) {
		fclose (fp);
		return (0);
	}

	fseek(fp, head.f_symptr + (SYMESZ * head.f_nsyms), 0);
	if (1 != fread(&str_length, sizeof(str_length), 1, fp))
		str_length = 0;
	if (str_length) {
		unsigned len;

		len = str_length -= 4;
		if (len != str_length || NULL == (str_tab = malloc(len))) {
			fclose (fp);
			return (0);
		}

		if (1 != fread(str_tab, len, 1, fp)) {
			free(str_tab);
			fclose (fp);
			return (0);
		}
	}

	fseek(fp, head.f_symptr, 0);
	for (i = aux = 0; i < head.f_nsyms; i++) {
		SYMENT sym;	/* symbol read in */
		int taux, j;

		if (1 != fread(&sym, SYMESZ, 1, fp)) {
			free(str_tab);
			fclose (fp);
			return (0);
		}
		if (aux) {
			aux--;
			continue;
		}
		aux = sym.n_numaux;
		for (j = taux = 0; j < count; j++) {
			static char n1[SYMNMLEN + 1], n2[SYMNMLEN + 1];
			register SYMENT *np;

			if (taux) {
				taux--;
				continue;
			}
			np = nlp + j;
			taux = np->n_numaux;
			if (np->n_type != 0xFFFF ||
			    strcmp(symName(np, names, n1),
			    	   symName(&sym, str_tab, n2)))
				continue;
			np->n_value  = sym.n_value;
			np->n_scnum  = sym.n_scnum;
			np->n_type   = sym.n_type;
			np->n_sclass = sym.n_sclass;
			break;
		}
	}
	free(str_tab);
	fclose (fp);
	return (1);
}

#ifdef TEST
main()
{
	static SYMENT sym[3];
	static char ptr[] = "a_very_long_name";

	strcpy(sym[0]._n._n_name, "x");
	sym[0].n_type = -1;

	strcpy(sym[1]._n._n_name, "y");
	sym[1].n_type = -1;

	sym[2]._n._n_n._n_zeroes = 0;
	sym[2]._n._n_n._n_offset = sizeof(long);
	sym[2].n_type = -1;

	coffnlist("tx.o", sym, ptr, 3);
}
#endif
