.TH "arcoff.h" "" "" "Header File"
.PC "COFF archive-file header"
#include <coff/arcoff.h>
.PP
.B arcoff.h
declares the structure
.BR ar_hdr ,
which is the header to a member of an archive.
.B ar_hdr
is structured as follows:
.DM
.ta 0.5i 1.0i 3.0i
struct ar_hdr
{
	char	ar_name[16];	/* file member name - `/' terminated */
	char	ar_date[12];	/* file member date - decimal */
	char	ar_uid[6];	/* file member user id - decimal */
	char	ar_gid[6];	/* file member group id - decimal */
	char	ar_mode[8];	/* file member mode - octal */
	char	ar_size[10];	/* file member size - decimal */
	char	ar_fmag[2];	/* ARFMAG - string to end header */
};
.DE
.PP
The COFF common-archive format has the following structure:
.ie p .PH 1 1 \\*(XD/arcoff.h.eps
.el \{
.DM
	_______________________________________________
	|__________ARCHIVE_MAGIC_STRING_______________|
	|__________ARCHIVE_FILE_MEMBER_1______________|
	|                                             |
	|       Archive File Header "ar_hdr"          |
	|.............................................|
	|             Member Contents                 |
	|               1. External symbol directory  |
	|               2. Text file                  |
	|_____________________________________________|
	|________ARCHIVE_FILE_MEMBER_2________________|
	|                "ar_hdr"                     |
	|.............................................|
	|       Member Contents (.o or text file)     |
	|_____________________________________________|
	|       .               .               .     |
	|       .               .               .     |
	|       .               .               .     |
	|_____________________________________________|
	|________ARCHIVE_FILE_MEMBER_n________________|
	|               "ar_hdr"                      |
	|.............................................|
	|               Member Contents               |
	|_____________________________________________|
.DE
\}
.SH "See Also"
.Xr a_out.h, a_out.h
.Xr "file formats," file_form
.Xr "header files" header_fi
.br
Gircyc, G.R.: \fIUnderstanding and Using COFF\fR.
Sebastopol, Calif, O'Reilly & Associates, Inc., 1990.
