#include <coff.h>

/* The PLIST struct describes all aspects of a single patch operation.  */
typedef struct plist {
 	SYMENT *p_lvnp; /* nlist entry for LHS of assignment.  */
	SYMENT *p_rvnp; /* nlist entry for RHS of assignment.  */
	long	p_lval; /* Value of LHS of assignment.  */
	long	p_rval; /* Value of RHS of assignment.  */
	int	p_type; /* Type of data being patched.  Can be one of:
			 * 'c' - character
			 * 's' - short
			 * 'i' - integer
			 * 'l' - long
			 */
	union {
		char	p_char;
		short	p_short;
		int	p_int;
		long	p_long;
	} p_val;
} PLIST;

#if !defined(TRUE) || !defined(FALSE)
#undef TRUE
#undef FALSE
#define TRUE (1==1)
#define FALSE (1==2)
#endif

void main();
int getnames();
void badsym();
void getone();
void setfile();
void setkmem();
int patch();
long myatol();
void usage();
char *index();
char *realloc();
