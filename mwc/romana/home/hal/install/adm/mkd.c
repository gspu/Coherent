/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1994 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */
/* mkd.c - revised mkdist utility */

Design.
=======

Inputs.
-------

Distribution directory.
Taz list, with subdirectories listed before their parents.
Ker list, one kernel for each type of distribution.
Disk 1 list - files that must be on the boot diskette.
Distribution type, e.g. { fva | fha }.
Destination for the taz archives.
Destination for the file lists.

Maybe later - files preferred on disk 1 if there is room.

Outputs.
--------
A set of taz archives.
A set of file lists, one for each distribution volume.

Actions.
--------


