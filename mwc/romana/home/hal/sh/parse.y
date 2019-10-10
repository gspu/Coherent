%{
#include <sys/compat.h>
#include <common/tricks.h>

#include "shellio.h"
#include "sh.h"

#define YYERROR	{ yyerrflag=1; goto YYerract; }

static	NODE	      *	node ();
static	NODE	      *	last_node ();

#define	WRITE_PARSE(node)	(debug_flag ? \
				 write_parse (node), shell_endl () : 0)

#define	YYTEXT()	(CSTRING_STRING (sh_yytext))

%}

%union {
	NODE	      *	yu_node;
	char	      *	yu_strp;
	CSTRING	      *	yu_cstrp;
	int		yu_nval;
}

%token _ANDF
%token _ASGN
%token _CASE
%token _CBRAC
%token _DO
%token _DONE
%token _DSEMI
%token _ELIF
%token _ELSE
%token _ESAC
%token _FI
%token _FOR
%token _IF
%token _IN
%token _IORS
%token _NAME
%token _NULL
%token _OBRAC
%token _ORF
%token _PARENS
%token _RET
%token _THEN
%token _UNTIL
%token _WHILE
%token _NOT

%type <yu_node>	case_line	case_list	cmd		cmd_list
%type <yu_node>	cmd_seq		do_list		else_part	in_name_list
%type <yu_node>	logical_cmd	name_list	opt_cmd_seq	pattern_list
%type <yu_node>	pipe_cmd	cmd_list_opt	pipeline

%type <yu_node>	command		simple_command	redirect_list	cmd_prefix
%type <yu_node>	cmd_word	cmd_suffix	cmd_name	asgn_node
%type <yu_node>	redirect_node	name_node	next_prefix
%type <yu_node>	function_definition	compound_command
%type <yu_node>	redirect_or_assign	redirect_or_name_or_assign
%type <yu_node> next_pattern_list	redirect_list_opt

%type <yu_nval> whuntile

%type <yu_cstrp> name_cstr pattern_cstr

%%

session: turn_on_keywords _session;

_session:
	'\n' {
		sesp->s_node = NULL;
		reset (RCMD);
	}
|
	cmd_list {
		WRITE_PARSE ($1);
		sesp->s_node = $1;
		reset (errflag ? RERR : RCMD);
	}
|	error '\n' {
		reset (RERR);
	}
|
;

if:	_IF optnls ;

then:	_THEN optnls ;

elif:	_ELIF optnls ;

else:	_ELSE optnls ;

whuntile:
	_WHILE optnls {	$$ = NWHILE; }
|	_UNTIL optnls {	$$ = NUNTIL; }
;

do:	_DO optsemi optnls ;

oror:	_ORF optnls;

andand:	_ANDF optnls;

or:	'|' optnls;

oparen:	'(' optnls ;

obrack:	_OBRAC optnls ;

cparen:	')' optnls ;

dsemi:	_DSEMI optnls ;

cmd_list:
	logical_cmd optsemi {
		$$ = $1;
	}
|	logical_cmd '&' cmd_list_opt {
		$$ = node (NBACK, $1, $3);
	}
|	logical_cmd ';' cmd_list {
		$$ = node (NLIST, $1, $3);
	}
;

cmd_list_opt:
	cmd_list
|	{ $$ = NULL; }
;

logical_cmd:
	pipeline
|	logical_cmd oror pipeline {
		$$ = node (NORF, $1, $3);
	}
|	logical_cmd andand pipeline {
		$$ = node (NANDF, $1, $3);
	}
;

pipeline:
	pipe_cmd
|	_NOT  pipe_cmd {
		$$ = node (NNOT, NULL, $2);
	}
;

pipe_cmd:
	cmd or pipe_cmd {
		$$ = node (NPIPE, $1, $3);
	}
|	cmd
;


/*
 * In the original grammar, no distinction between simple command and compound
 * commands was made. This, along with the right-recursive formulation of the
 * command grammar, created a need for lookahead that defeated the complex
 * machinery for context-sensitive lexing that is required.
 *
 * Since this is the level above the "command" production, this seems like the
 * best place to deal with temp files (the command production encompasses the
 * cases where redirection is legal).
 */

cmd:	command {
		TEMP_FILE     *	temp = capture_temp ();

		$$ = temp == NULL ? $1 : node (NFILE, temp, $1);
	}
;

turn_on_keywords: {
		keyflag = 1;
	}
;

command:
	simple_command {
		$$ = node (NCOMS, $1, NULL);
	}
|	compound_command redirect_list_opt {
		$$ = node (NCOMS, node (NCTRL, $1, $2), NULL);
	}
|	function_definition {
		$$ = node (NCOMS, $1, NULL);
	}
|	_RET name_cstr {
		$$ = node (NRET, $2, NULL);
	}
|	_RET {
		$$ = node (NRET, NULL, NULL);
	}
;
	
function_definition:
	name_cstr _PARENS optnls obrack cmd_seq _CBRAC  {
		$$ = node (NCTRL, node (NFUNC, $1, $5), NULL);
	}
;

redirect_list:
	redirect_node
|	redirect_list redirect_node {
		($$ = $1)->n_next = $2;
	}
;

redirect_list_opt:
	redirect_list
|	{ $$ = NULL; }
;

simple_command:
	cmd_prefix cmd_word cmd_suffix {
		(last_node ($1)->n_next = $2)->n_next = $3;
		$$ = $1;
	}
|	cmd_prefix
|	cmd_name cmd_suffix {
		($$ = $1)->n_next = $2;
	}
;

cmd_prefix:
	redirect_or_assign next_prefix {
		($$ = $1)->n_next = $2;
	}
;

next_prefix:	cmd_prefix | { $$ = NULL; } ;

redirect_or_assign:
	asgn_node  | redirect_node
;

cmd_name:	name_node ;
cmd_word:	name_node ;


/*
 * The main part of this shell has some silliness with assignments and some
 * flag called '-k'. To support this, we allow assignments after the command
 * name and code elsewhere turns them back into parameters... it seems
 * preferable to do it here, but because of the '-k' thing we'll just
 * accept them.
 */

cmd_suffix:
	redirect_or_name_or_assign cmd_suffix {
		($$ = $1)->n_next = $2;
	}
|	{ $$ = NULL; }
;

redirect_or_name_or_assign:
	redirect_node | name_node | asgn_node
;


/*
 * The form of the following productions arranges for the contents of the
 * last lexed token to be duplicated ASAP, hopefully before lookahead gets
 * involved.
 */

redirect_node:	_IORS {
		$$ = node (NIORS, dupcstr (sh_yytext, 0), NULL);
	}
;

name_node: name_cstr {
		$$ = node (NARGS, $1, NULL);
	}
;

name_cstr: _NAME {
		$$ = dupcstr (sh_yytext, 0);
	}
;

asgn_node: _ASGN {
		$$ = node (NASSG, dupcstr (sh_yytext, 0), NULL);
	}
;

compound_command:
	_FOR name_cstr turn_on_keywords in_name_list optsep do_list _DONE {
		$$ = node (NFOR, $2, node (NFOR2, $4, node (NLIST, $6, NULL)));
	}
|	_CASE name_cstr turn_on_keywords optsep _IN optsep case_list _ESAC {
		$$ = node (NCASE, $2, $7);
	}
|	whuntile cmd_seq do_list _DONE {
		$$ = node ($1, $2, node (NLIST, $3, NULL));
	}
|	if cmd_seq then opt_cmd_seq else_part _FI {
		$$ = node (NIF, node (NPAIR, $2, $4), $5);
	}
|	oparen opt_cmd_seq ')' {
		$$ = node (NPARN, $2, NULL);
	}
|	obrack opt_cmd_seq _CBRAC {
		$$ = node (NBRAC, $2, NULL);
	}
;

in_name_list:
	_IN name_list {
		$$ = $2;
	}
|	{
		struct {
			size_t	foo;
			char	bar [4];
		} foo = { 4, "\"$@\"" };
		$$ = node (NARGS, dupcstr ((CSTRING *) & foo, 0), NULL);
	}
;

name_list:
	name_node name_list {
		($$ = $1)->n_next = $2;
	}
|	{ $$ = NULL; }
;

case_list:
	case_line dsemi case_list {
		last_node ($$ = $1)->n_next = $3;
	}
|	case_line
|	{ $$ = NULL; }
;

case_line:
	pattern_list cparen opt_cmd_seq {
		$$ = node (NCASE2, $3, $1);
	}
|	oparen pattern_list cparen opt_cmd_seq {
		$$ = node (NCASE2, $4, $2);
	}
;

pattern_list:
	pattern_cstr next_pattern_list {
		$$ = node (NCASE3, $1, $2);
	}
;

pattern_cstr:
	pattern_name {
		$$ = dupcstr (sh_yytext, 0);
	}
;

pattern_name:
	_NAME
|	_ASGN
|	_CASE
|	_DO
|	_DONE
|	_ELIF
|	_ELSE
|	_FI
|	_FOR
|	_IF
|	_IN
|	_RET
|	_THEN
|	_UNTIL
|	_WHILE
|	_NOT
;

next_pattern_list:
	'|' pattern_list {
		$$ = $2;
	}
|	{ $$ = NULL; }
;

do_list:
	do opt_cmd_seq {
		$$ = $2;
	}
|	{ $$ = NULL; }
;

else_part:
	elif cmd_seq then opt_cmd_seq else_part {
		$$ = node (NIF, node (NPAIR, $2, $4), $5);
	}
|	else opt_cmd_seq {
		$$ = node (NELSE, $2, NULL);
	}
|	{ $$ = NULL; }
;

opt_cmd_seq: cmd_seq | { $$ = NULL; } ;

cmd_seq:
	cmd_list nls cmd_seq {
		$$ = node (NLIST, $1, $3);
	}
|	cmd_list optnls
;

optsep:	sep | ;

sep:	nls
|	';'
|	';' nls
;

optnls:	nls | ;

nls:	'\n'
|	nls '\n'
;

optsemi: ';' | ;

%%
/*
 * Create a node. Note that the arguments to this function are really kinda
 * flexible... "auxp" could actually be pretty much anything.
 */

static NODE *
node (type, auxp, next)
VOID	      *	auxp;
NODE	      *	next;
{
	NODE *np;

	np = (NODE *) balloc (sizeof (NODE));
	np->n_type = type;
	np->n_next = next;
	np->n_auxp = auxp;

	if (NODE_LOOPS (np))
		np->n_next->n_next = np;

	return np;
}

/*
 * Get the last node in a node list.
 */

static NODE *
last_node (nodep)
NODE	      *	nodep;
{
	while (nodep->n_next)
		nodep = nodep->n_next;
	return nodep;
}


/*
 * Pretty-print a counted string with some potential internal structure.
 */

void
dump_cstring (cstrp)
CSTRING	      *	cstrp;
{
	size_t		len = CSTRING_LENGTH (cstrp);
	char	      *	str = CSTRING_STRING (cstrp);

#if	SHOW_CSTRING
	shell_outchar ('<');
#endif
	while (len --) {
		if (* str) {
			shell_outchar (* str ++);
			continue;
		}

		/*
		 * An embedded NUL is a signal of an embedded sub-string.
		 */

		str ++;
		dump_cstring (cstrp = (CSTRING *) str);
		str += CSTRING_SIZE (cstrp);
		len -= CSTRING_SIZE (cstrp);

		if (str [- 1] != 0) {
			shell_outstr ("Bad cstring - '");
			shell_outchar (str [- 1]);
			shell_outchar ('\'');
			shell_endl ();
			return;
		}
	}
#if	SHOW_CSTRING
	shell_outchar ('>');
#endif
}


/*
 * NIGEL: Do a pretty formatted dump of the parse tree.
 */

void
write_parse (nodep)
NODE	      *	nodep;
{
	NODE	      *	loop = NULL;

	shell_outchar ('(');

	while (nodep) {
		shell_outstr (nodeinfo [nodep->n_type].ni_name);
		switch (NODE_UNION_TAG (nodep)) {

		case STRP:
			shell_outstr (" \"");
			shell_outstr (nodep->n_strp);
			shell_outchar ('"');
			break;

		case NODEP:
			shell_outchar (' ');
			write_parse (nodep->n_auxp);
			break;

		case CSTRP:
			shell_outchar (' ');
			dump_cstring (nodep->n_cstrp);
			break;

		default:
			shell_outint (NODE_UNION_TAG (nodep));
			break;
		}

		if (nodep->n_next != NULL)
			shell_outstr (", ");

		if (loop != NULL) {
			loop->n_next->n_next = loop;
			break;
		} else {
			if (NODE_LOOPS (nodep)) {
				nodep->n_next->n_next = NULL;
				loop = nodep;
			}

			nodep = nodep->n_next;
		}
	}
	shell_outchar (')');
}



#define NBPC 8
#define NKEY 8
static char keys[NKEY] = { 0 };
static int  keyi = NKEY * NBPC;

keyflush()
{
	register char *kp;

	for (kp = keys+NKEY; kp > keys; *--kp = 0)
		;
	keyi = NKEY * NBPC;
}

keypop()
{
	register char	*kp;
	register int	km;

	if ((km = keyi ++) >= NKEY * NBPC)
		panic (11);

	kp = keys + (km / NBPC);
	km = 1 << (km %= NBPC);
	keyflag = (* kp & km) ? 1 : 0;
	*kp &= ~km;
}

keypush()
{
	register char	*kp;
	register int	km;

	if ((km = --keyi) < 0)
		panic (12);

	if (keyflag) {
		kp = keys + (km / NBPC);
		km = 1 << (km %= NBPC);
		* kp |= km;
	}
}
/*
 * The following fragments might implement named pipes.
 * The token declaration goes in the header.
 * The nopen production should go with the others of its ilk.
 * The production fragment goes into arg:
%token _NOPEN _NCLOSE
nopen:	_NOPEN optnls ;

|	nopen pipe_cmd ')' {
		$$ = node (NRPIPE, $2, NULL);
	}
|	oparen pipe_cmd _NCLOSE {
		$$ = node (NWPIPE, $2, NULL);
	}
CLOSE {
		$$ = node (NWPIPE, $2, NULL);
	}
 *
 */
