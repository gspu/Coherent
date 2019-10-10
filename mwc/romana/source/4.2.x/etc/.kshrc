# /etc/.kshrc
# Wed Oct 27 05:14:15 1993 CST

# Use set -o emacs for emacs history mode.
# Use set -o vi for vi history mode.
set -o vi
echo "ksh: vi command history mode.  Edit /etc/.kshrc to change this."

# Set shell prompt to give login device, user name, and current directory.
PS1=`tty | sed 's/^.....//'`" $USER \$PWD: "

# Allow ^C to send SIGINT even when editing command line.
bind '^C=abort'

# Some subshells (e.g. without tty) may be unable to use this file.
unset ENV
