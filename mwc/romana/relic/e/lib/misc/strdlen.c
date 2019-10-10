/*
 * strdlen calculates the length of the displayed string.
 * Should understand escape sequences and cntrl characters.
 * for most terminals.
 */
#include	<string.h>

#define	ESCAPE		'\033'
#define NORMAL		1
#define ST_ESCAPE	2
#define LOOK_ALPHA	3
#define LOOK_EQUAL	4
#define ALPHA_GRAVE	5

int strdlen(string)
char	*string;
{
	register char	*pcChar;
	register int	iStrLen = 0;
	int		iState;

	for (pcChar = string, iState = NORMAL; *pcChar; *pcChar++) {
		switch(iState) {
		case NORMAL:
			if (*pcChar == ESCAPE) {
				iState = ST_ESCAPE;
				continue;
			}
			if (iscntrl((int) *pcChar)) 
				continue;
			break;
		case ST_ESCAPE:
			if (*pcChar == ESCAPE) {
				iState = NORMAL;
				break;		
			}
			switch(*pcChar) {
			case '[':
				iState = LOOK_ALPHA;
				continue;
			case '>':
				iState = LOOK_EQUAL;
				continue;
			case '`':
				iState = NORMAL;
				continue;	
			default:
				if (isalpha((int) *pcChar))
					iState = NORMAL;
				else
					if (isdigit((int) *pcChar))
						iState = ALPHA_GRAVE;
					else
						iState = LOOK_ALPHA;
				continue;
			}
		case ALPHA_GRAVE:
			if (isalpha((int) *pcChar)) {
				iState = NORMAL;
				break;
			}
			continue;
		case LOOK_ALPHA:
			if (isalpha((int) *pcChar) || *pcChar == '`' ||
						*pcChar == '@') 
				iState = NORMAL;
			continue;
		case LOOK_EQUAL:
			iState = NORMAL;
			if (*pcChar == '=')
				continue;
			break;
		}
		iStrLen++;
	}
	return(iStrLen);
}

#if TEST
static void
checkit(s)
char *s;
{
	static int testNo;
	int i;

	testNo++;
	if (3 != (i = strdlen(s)))
		printf("%d = test %d\n", i, testNo);
}

main()
{
	/* Test ESCAPE */
	checkit("\0338abc");
	checkit("\033[12mabc");
	checkit("\033>abc");
	checkit("\033>=abc");
	checkit("\033Dabc");
	checkit("\033`abc");
	checkit("\03312`abc");
	checkit("\033[@abc");
	checkit("\033[12ma\033[11mbc\033[11m");
	/* Test CTRL */
	checkit("\001ab\002c");
}
#endif
