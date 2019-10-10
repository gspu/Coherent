export PATH=/bin:$PATH

###############
# require_number
###############

require_number () {
	if is_numeric $1; then
		return 0
	else
		echo "Please enter a number or hit <Enter> for the default value."
		return 1
	fi
}

###############
# tune
#
# Given tunable parameter and description string,
#   prompt for new value to the parameter
#   change parameter setting, if needed, using idtune
###############

tune () {
	get_tunable $1 DFLT
	read_input "$2" VAL $DFLT require_number
	if is_numeric $VAL;then
		if [ $VAL -ne $DFLT ]; then
			$CONF_DIR/bin/idtune -f $1 $VAL || return 1
		fi
	else
		echo "Please enter a number, or <Enter> for default value."
		return 1
	fi
	return 0
}

###############
# get_tunable
#
# Get current setting of a tunable parameter into a shell variable, or die
###############

get_tunable () {
	P=$1
	set "$2" $($CONF_DIR/bin/idtune -p $1)

	is_empty $3 && {
		echo "aha/mkdev: Can't get current setting of parameter $P" 2>&1
		exit 100
	}

	eval $1=$3
}


###############
# is_enabled
#
# Return 0 if a device is enabled in sdevice, else return 1 or exit on failure
###############

is_enabled() {
	case $($CONF_DIR/bin/idenable -p $1) in
	$1*Y)
		return 0
		;;
	$1*N)
		return 1
		;;
	*)
		echo "$1/mkdev: can't tell if $1 device is enabled"
		exit 1
		;;
	esac
}

###############
# set_enable_status
#
# Allow changing enable/disable status of device $1; exit 0 if device disabled.
###############

set_enable_status () {
	while : ; do
		if is_enabled $1 ; then
			__dev_used=y
			echo "\nDevice is enabled."
		else
			__dev_used=
			echo "\nDevice is not enabled."
		fi

		read_input "Change enable/disable status" __answer "n" require_yes_or_no
		is_yes $__answer || break

		# Change enable/disable status of the driver.
		if is_empty $__dev_used;then
			$CONF_DIR/bin/idenable -e $1
		else
			$CONF_DIR/bin/idenable -d $1
		fi
	done
	is_empty $__dev_used && exit 0
}

