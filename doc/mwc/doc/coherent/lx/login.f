.TH login "" "" "System Administration"
.PC "Set default values for logging in"
.B "/etc/default/login"
.PP
.II /etc/default/login
The command
.B login
reads the file
.BR /etc/default/login ,
which gives
.BR login 's 
default settings.
These settings dictate some of
.BR login 's
behaviors.
.B login
exports some settings as environmental variables, to help control
the behavior of some other programs within \*(CO.
.PP
For a list of the settings normally set by
.BR /etc/default/login ,
see the Lexicon entry for the command
.BR login .
.SH "See Also"
.Xr "Administering COHERENT," administe
.Xr "login" login
