PS1='ksh.! '
PS2='ksh.!> '
set -o emacs ignoreeof
set -h
MAILCHECK=30
MAIL=/usr/spool/mail/norm
EDITOR=/bin/emacs
VISUAL=/bin/emacs
FCEDIT=/bin/emacs
export uupub
uupub=/usr/spool/uucppublic
unset ENV
