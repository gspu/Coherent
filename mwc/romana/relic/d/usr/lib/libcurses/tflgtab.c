/* $Header: /usr/src/lib/libcurses/RCS/tflgtab.c,v 1.1 89/04/07 13:22:17 src Exp $
 *
 *	The  information  contained herein  is a trade secret  of INETCO
 *	Systems, and is confidential information.   It is provided under
 *	a license agreement,  and may be copied or disclosed  only under
 *	the terms of that agreement.   Any reproduction or disclosure of
 *	this  material  without  the express  written  authorization  of
 *	INETCO Systems or persuant to the license agreement is unlawful.
 *
 *	Copyright (c) 1989
 *	An unpublished work by INETCO Systems, Ltd.
 *	All rights reserved.
 */

#include "curses.h"

/*
 * Default translation table: Termcap Name into address of a boolean variable.
 */
tflgent_t tflgtab[] = {
	{ "am",	&AM	},
	{ "bs", &BS	},
	{ "da", &DA	},
	{ "db", &DB	},
	{ "eo", &EO	},
	{ "hc", &HC	},
	{ "hz", &HZ	},
	{ "in", &IN	},
	{ "mi", &MI	},
	{ "ms", &MS	},
	{ "nc", &NC	},
	{ "ns", &NS	},
	{ "os", &OS	},
	{ "ul", &UL	},
	{ "xb", &XB	},
	{ "xn", &XN	},
	{ "xt", &XT	},
	{ "xs", &XS	},
	{ "xx", &XX	},
	{ "",	NULL	}
};
