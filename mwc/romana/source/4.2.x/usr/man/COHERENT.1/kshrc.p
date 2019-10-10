.kksshhrrcc -- System Administration

Set personal environment for Korn shell

Whenever you invoke the Korn shell kksshh, it executes the script named in the
environmental  variable   EENNVV.  By  custom,   this  is  usually   the  file
${HHOOMMEE}/.kksshhrrcc.

To ensure that .kksshhrrcc is executed whenever you log in, insert the line

    export ENV=${HOME}/.kshrc

into your .pprrooffiillee.

.kksshhrrcc should include  all items that you wish to  have known to all of the
shells that  you invoke -- both  the login shell and  all subshells.  These
should include aliases,  environmental variables, and the sseett commands that
you use  to fine-tune the  operation of the  shell.  If you  wish to define
items in your  login shell but hide them from  subshells, simply place them
in your .pprrooffiillee instead of your .kksshhrrcc.  For example, the command

    set -o emacs

turns on  MicroEMACS-style command-line editing  for all of  your subshells
when you  insert it into your  .kksshhrrcc, but turns it on  only for your login
shell if you insert it only into your .pprrooffiillee.

The following gives a sample .kksshhrrcc:

# Set the main prompt (PS1) to be the machine (i.e., site) name, the
# tty name (i.e., session name) and the current directory.  The
# second-level prompt (PS2) used for multi-line commands is much simpler.
SITE=`cat /etc/uucpname`
TTY=`tty | sed s/^.....//`
PS1='$SITE $TTY $PWD: '
PS2='MORE> '

# Turn on hashing, tracking, and filename completion (-h), EMACS-like
# command-line editing, and ignore end-of-file (<ctrl-D>) as a way to
# log out.
set -h -o emacs ignoreeof

#
# Add the word "logout" as an alias for "exit".
#
alias logout='exit'

# Add EMACS command line editing command "delete-word-backword" bound
# to the key sequence <Esc><Backspace>. Note that there are four
# characters inside the apostrophes; the shell interprets a ^
# followed by a character as meaning <Ctrl> character.
bind '^[^H'=delete-word-backward

#  Select MicroEMACS as the default editor to use with "fc" commands
FCEDIT=emacs

_S_e_e _A_l_s_o
AAddmmiinniisstteerriinngg CCOOHHEERREENNTT, EENNVV, kksshh, pprrooffiillee, .pprrooffiillee, UUssiinngg CCOOHHEERREENNTT
