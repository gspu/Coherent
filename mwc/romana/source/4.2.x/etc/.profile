# /etc/.profile original 08/11/93 by Udo Munk (udo@umunk.GUN.de)

# Mods at MWC
# Tue Nov  9 09:25:08 1993 CST
# COHERENT executes this profile for the single user shell.

# ----- DO NOT CHANGE THE FOLLOWING SECTION FOR SINGLE USER PROFILE -----

#
# don't let user abort the shutdown now
#
trap "" 1 2 3

# unmount any mounted file systems other than root.
/etc/umount.all || {
	echo "Warning from /etc/.profile: Some non-root file systems remain mounted."
	echo -n "Hit <Enter> to continue shutdown: "
	read junk
}


#
# let's see, do we want to halt or reboot the system?
#
if [ -f /etc/shutdown.lvl ]; then
  mode=`cat /etc/shutdown.lvl`
  /bin/rm /etc/shutdown.lvl

  case $mode in

    powerfail)
      /bin/sync
      /bin/sync
      /bin/echo "Power fail, system is safe now."
      #
      # Here you're supposed to call a program which asks the power fail
      # device about the power and returns when and if the line is OK again.
      #
      # Then we may return to multi user. For test purposes,
      # we just read the keyboard instead and when a newline is
      # typed, the power is OK and we take the system back to
      # multi user mode.
      #
      read a
      kill -9 $$
      ;;

    halt)
      /bin/sync
      /bin/sleep 5
      echo "System is safe now, switch power off."
      /bin/sync
      /bin/sleep 100000000 
      ;;

    reboot)
      /bin/sync
      /bin/sleep 5
      /bin/sync
      /etc/reboot
      ;;
  esac
fi

#
# single user mode, allow interrupts again
#
trap 1 2 3

# ----- THE FOLLOWING SECTION MAY BE CHANGED FOR SINGLE USER PROFILE -----

#
# profile for single user mode
#
export PATH=:/bin:/usr/bin:/etc
export TERM=ansipc
/bin/stty sane
