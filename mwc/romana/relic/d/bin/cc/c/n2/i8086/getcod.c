/*
 * The routines in this file
 * read in code from the intermediate file.
 * It is almost machine independent but it has been
 * broken out just in case a strange machine creeps
 * along.
 */
#ifdef   vax
#include "INC$LIB:cc2.h"
#else
#include "cc2.h"
#endif

/*
 * Read in a code item.
 * Return a pointer to a filled in
 * INS node.
 */
INS *
getcode()
{
	register INS	*ip;
	register OPINFO	*opp;
	register int	i;
	register int	opcode;

	opcode = bget();
	opp = &opinfo[opcode];
	if (opp->op_style == OF_LPTR) {
		if (iget() != (A_LID|A_DIR))
			cbotch("lptr");
		ip = newi(sizeof(INS));
		ip->i_type = LLLINK;
		ip->i_fp = NULL;
		ip->i_bp = NULL;
		ip->i_labno = iget();
		ip->i_sp = NULL;
		ip->i_ip = NULL;
		return (ip);
	}
	if ((opp->op_flag&OP_JUMP) != 0) {
		if (iget() != (A_LID|A_DIR))
			cbotch("jump");
		ip = newi(sizeof(INS));
		ip->i_type = JUMP;
		ip->i_long = 0;
		ip->i_rel = opcode;
		ip->i_fp = NULL;
		ip->i_bp = NULL;
		ip->i_labno = iget();
		ip->i_sp = NULL;
		ip->i_ip = NULL;
		return (ip);
	}
	ip = newn(opp->op_naddr);
	ip->i_type = CODE;
	ip->i_fp = NULL;
	ip->i_bp = NULL;
	for (i=0; i<opp->op_naddr; ++i)
		getfield(opcode, &ip->i_af[i]);
	ip->i_op = opcode;
	ip->i_naddr = opp->op_naddr;
	return (ip);
}

/*
 * This routine reads in an AUTOS item
 * that appears in a function body. All it does is
 * read the size of the stack frame into the global cell
 * `framesize', which is used by `genprolog' to put out
 * the function prolog. The register field of the AUTOS
 * item (the second ival_t) is thrown away.
 */
getautos()
{
	framesize = iget();
	iget();
}
