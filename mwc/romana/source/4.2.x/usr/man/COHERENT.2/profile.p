.pprrooffiillee -- System Administration

Execute commands at login
$HHOOMMEE/.pprrooffiillee

The shell reads file $HHOOMMEE/.pprrooffiillee whenever  a user logs in.  The user can
edit its  contents to set  up her environment  however she prefers,  and to
execute programs routinely upon login.

The following gives one user's .pprrooffiillee:

    MAIL=/usr/spool/mail/sally
    PATH=/usr/bin:/bin:/v/sally/bin:.
    EDITOR=me
    PS1="Sally(!) "
    PS2="MORE(!)> "
    PAGER=scat
    set -h
    set -o emacs
    echo "CALENDAR:"
    calendar
    echo ""
    /usr/games/fortune

The first  six entries set environmental variables; note  that these are in
addition to the variables set in /eettcc/pprrooffiillee.

The next two entries

    set -h
    set -o emacs

set two features of the Korn shell, which is used by the person.  The first
turns  on its  hashing feature,  and the  second turns  on MicroEMACS-style
editing of the command line.

The last four entries

    echo "CALENDAR:"
    calendar
    echo ""
    /usr/games/fortune

execute   two  programs   upon  login.   The   two  eecchhoo   commands  print,
respectively,  the word  CCAALLEENNDDAARR  and a  blank  line on  the screen.   The
command ccaalleennddaarr reads the  user's personal calendar and prints all entries
the relate  to today (or  to the weekend,  should today be  a Friday).  The
command ffoorrttuunnee  prints a randomly selected (and,  we hope, amusing) select
from file /uussrr/ggaammeess/lliibb/ffoorrttuunneess.

This example is relatively simple.  A  user's .pprrooffiillee can be turned into a
complex shell program if you wish.

_S_e_e _A_l_s_o
AAddmmiinniisstteerriinngg CCOOHHEERREENNTT, kksshh, .kksshhrrcc, pprrooffiillee, sshh, UUssiinngg CCOOHHEERREENNTT
