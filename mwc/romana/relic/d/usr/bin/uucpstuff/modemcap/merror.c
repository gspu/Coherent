#include <stdio.h>

int merrno;			/* modem error return code		*/

char *_merr_list[] = {
	"No error",
	"Interrupt occurred",				/*  M_INTRPT	*/
	"Dialer Hung",					/*  M_D_HUNG	*/
	"No answer",					/*  M_NO_ANS	*/
	"Illegal baud rate",				/*  M_ILL_BD	*/
	"ACU Problem",					/*  M_A_PROB	*/
	"Line Problem",					/*  M_L_PROB	*/
	"Can't open LDEVS file",			/*  M_NO_LDV	*/
	"Requested device not available",		/*  M_DV_NT_A	*/
	"Requested device not known",			/*  M_DV_NT_K	*/
	"No device available at requested baud",	/*  M_NO_BD_A	*/
	"No device known at requested baud",		/*  M_NO_BD_K	*/
	"Incompatible telephone num and device chosen"	/*  M_DEV_TEL	*/
};

int	_msys_nerr = (sizeof (_merr_list) / sizeof (char *));

merror (s)
char	*s;
{
	int	i = -merrno;

	if (0 <= i && i < _msys_nerr)
		fprintf (stderr, "%s: %s\n", s, _merr_list[i]);
	else
		fprintf (stderr, "%s: Error %d\n", s, merrno);
}
