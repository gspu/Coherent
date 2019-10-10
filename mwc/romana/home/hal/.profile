MAIL=/usr/spool/mail/hal
set -- $(tty) $(uname -n)
TTY=${1#/dev/}
SITE=$2
PROMPT="$TTY $SITE $(pwd) > "
