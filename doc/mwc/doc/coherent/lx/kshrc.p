.TH .kshrc "" "" "System Administration"
.PC "Set personal environment for Korn shell"
.PP
Whenever you invoke the Korn shell
.BR ksh ,
it executes the script named in the environmental variable
.BR ENV .
By custom, this is usually the file
.BR ${HOME}/.kshrc .
.PP
To ensure that
.B .kshrc
is executed whenever you log in,
insert the line
.DM
	export ENV=${HOME}/.kshrc
.DE
.PP
into your
.BR .profile .
.PP
.B .kshrc
should include all items that you wish to have known to all of the shells
that you invoke \(em both the login shell and all subshells.
These should include aliases, environmental variables, and the
.B set
commands that you use to fine-tune the operation of the shell.
If you wish to define items in your login shell but hide them from
subshells, simply place them in your \fB.profile\fR instead of
your \fB.kshrc\fR.
For example, the command
.DM
	set -o emacs
.DE
.PP
turns on \*(ME-style command-line editing for all of your subshells
when you insert it into your \fB.kshrc\fR, but turns it on only for your
login shell if you insert it only into your \fB.profile\fR.
.PP
The following gives a sample \fB.kshrc\fR:
.DM
# Set the main prompt (PS1) to be the machine (i.e., site) name, the
# tty name (i.e., session name) and the current directory.  The
# second-level prompt (PS2) used for multi-line commands is much simpler.
SITE=`cat /etc/uucpname`
TTY=`tty | sed s/^.....//`
PS1='$SITE $TTY $PWD: '
PS2='MORE> '
.DE
.DM
# Turn on hashing, tracking, and filename completion (-h), EMACS-like
# command-line editing, and ignore end-of-file (<ctrl-D>) as a way to
# log out.
set -h -o emacs ignoreeof
.DE
.DM
#
# Add the word "logout" as an alias for "exit".
#
alias logout='exit'
.DE
.DM
# Add EMACS command line editing command "delete-word-backword" bound
# to the key sequence <Esc><Backspace>. Note that there are four
# characters inside the apostrophes; the shell interprets a ^
# followed by a character as meaning <Ctrl> character.
bind '^[^H'=delete-word-backward
.DE
.DM
#  Select MicroEMACS as the default editor to use with "fc" commands
FCEDIT=emacs
.DE
.SH "See Also"
.Xr "Administering COHERENT," administe
.Xr "ENV," env
.Xr "ksh," ksh
.Xr "profile," profile
.Xr ".profile," profile.p
.Xr "Using COHERENT" using_coh
