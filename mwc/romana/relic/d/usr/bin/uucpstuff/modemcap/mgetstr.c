extern	char	*__modemcap;

char	*mgetstr (id, area)
register char	*id;
register char	**area;
{
	register char	*cp = __modemcap;
	register char	*str = *area;		/* start of current string	 */

	if (__modemcap == (char *) 0)		/* has mgetent() been called? ... */
		return ((char *) 0);			/* ... no, can't find string */

	while (*cp != ':' && *cp != 0)		/* find first entry in cap	 */
		cp++;

	if (*cp == 0)				/* empty entry???		 */
		return ((char *) 0);			/* ... yes, bad modemcap entry	 */
	else
		cp++;				/* point to first character in next */

	while (*cp != 0) {			/* until entry found or end of entry */
		if (cp[0] == id[0] && cp[1] == id[1]) {	/* found entry!!!	 */
			if (cp[2] != '=')	/* is it a string value???	 */
				return ((char *) 0);	/* no, something else		 */
			else
			    break;		/* yes, entry was found		 */
		} 
		else {			/* not entry, skip this entire entry */
			while (*cp != ':' && *cp != 0)
				cp++;		/* search for end of current entry */

			if (*cp != 0)
				cp++;		/* skip terminating character	 */
		}
	}
	if (*cp == 0)				/* end of modem cap entry	 */
		return ((char *) 0);

	cp += 3;				/* point to actual string	 */
	while (*cp != ':' && *cp != 0) {	/* for every character in string ... */
		if (*cp == '\\') {		/* translate escaped character	 */
			cp++;
			switch (*cp) {
			case 'n':	/* newline		 */
				**area = '\n';
				(*area)++;
				cp++;
				break;
			case 'r':	/* carriage return	 */
				**area = '\r';
				(*area)++;
				cp++;
				break;
			case 'b':	/* backspace		 */
				**area = '\b';
				(*area)++;
				cp++;
				break;
			case 'f':	/* form feed		 */
				**area = '\f';
				(*area)++;
				cp++;
				break;
			case 't':	/* tab			 */
				**area = '\t';
				(*area)++;
				cp++;
				break;
			case 'E':	/* Escape character	 */
				**area = 033;
				(*area)++;
				cp++;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				**area = ((cp[0] - '0') << 6) +
				    ((cp[1] - '0') << 3) +
				    (cp[2] - '0');
				(*area)++;
				cp += 3;
				break;
			default:
				**area = *cp++;
				(*area)++;
				break;
			}
		} 
		else if (*cp == '^') {	/* some control character	 */
			cp++;
			if (*cp >= '@' && *cp <= '_') {
				**area = *cp - '@';
				(*area)++;
			}
			cp++;
		} 
		else {			/* some normal character	 */
			**area = *cp++;		/* put character in area	 */
			(*area)++;
		}
	}
	*((*area)++) = 0;			/* null terminate area and string */
	return (str);				/* return pointer to start of string */
}
