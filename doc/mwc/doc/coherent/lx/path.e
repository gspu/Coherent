.TH PATH "" "" "Environmental Variable"
.PC "Directories that hold executable files"
.PP
.B PATH
names a default set of directories that
are searched by \*(CO when it seeks an executable file.
You can set
.B PATH
with the command
.BR PATH .
For example, typing
.DM
	PATH=/bin:/usr/bin
.DE
.PP
tells \*(CO to search for executable files first in
.BR /bin ,
and then in
.BR /usr/bin .
Note the use of the colon \*(Ql:\*(Qr to separate directory names.
.SH "See Also"
.Xr "environmental variables," environva
.Xr "path.h" path.h
