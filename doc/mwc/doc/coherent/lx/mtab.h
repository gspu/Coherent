.TH mtab.h "" "" "Header File"
.PC "Currently mounted file systems"
.B "#include <mtab.h>"
.PP
The file
.B /etc/mtab
contains an entry for each file system mounted by the command
.BR mount .
This does not include the root file system,
which is already mounted when the system boots.
.PP
Both the commands
.B mount
and
.B umount
use the following structure, defined in
.BR mtab.h .
It contains the name of each special file mounted,
the directory upon which it is mounted, and any flags passed to
.B mount
(such as read only).
.DM
#define	MNAMSIZ	32
struct	mtab {
	char	mt_name[MNAMSIZ];
	char	mt_special[MNAMSIZ];
	int	mt_flag;
};
.DE
.SH Files
.B /etc/mtab
.SH "See Also"
.Xr "header files," header_fi
.Xr "mount," mount
.Xr "umount" umount
