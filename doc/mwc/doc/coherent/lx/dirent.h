.TH dirent.h "" "" "Header File"
.PC "Define directory-related data elements"
.B "#include <dirent.h>"
.PP
.B dirent.h
defines the data type
.B DIR
and the structure
.BR dirent .
It is used with the portable directory-manipulation routines
.BR closedir() ,
.BR getdents() ,
.BR opendir() ,
.BR readdir() ,
.BR rewinddir() ,
and
.BR telldir() .
.SH "See Also"
.Xr "closedir()," closedir
.Xr "getdents()," getdents
.Xr "header files," header_fi
.Xr "opendir()," opendir
.Xr "readdir()," readdir
.Xr "rewinddir()," rewinddir
.Xr "telldir()" telldir
.br
\*(PX Standard, \(sc5.1.1
