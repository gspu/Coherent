#!sh.940902


# doperm.sh
#
# apply MWC standard permissions to a list of files.
#
# Usage:
#   doperm.sh [ -sv ] [ -P permlist ] file...

# Parse command options.

PERMLIST=/source/Permlist.10
CHECKONLY=y
VERBOSE=n
while [ $# -gt 0 ]; do
	case $1 in
	-v)
		VERBOSE=y
		;;
	-s)
		echo "Setting permissions..."
		CHECKONLY=n
		;;
	-P)
		shift
		PERMLIST=$1
		;;
	-*)
		echo "Usage : $0 [ -s ] [ -P permlist ] file..." >&2
		exit 1
		;;
	*)
		break;
		;;
	esac
	shift
done

# Make sure we have a valid permlist.

test -f $PERMLIST || {
	echo "$0 : Can't open permlist file $PERMLIST" >&2
	exit 1
}

# For each file name supplied, either check its permissions (-c) or set them.

EXIT_STATUS=0
for FILE; do

	# Get required perms for the file as specified in PERMLIST.

	GREPPED=$(grep -n "	$FILE\$" $PERMLIST)
	LNUM=${GREPPED%%:*}
	RIGHTPERMS=${GREPPED#*:}

	test -n "$RIGHTPERMS" || {
		echo "$0 : no entry in $PERMLIST for $FILE" >&2
		continue
	}

	if [ $CHECKONLY = 'y' ];then

		# Get current perms.

		ACTUALPERMS=$(getperm $FILE)

		test -n "$ACTUALPERMS" || {
			echo "$0 : can't getperm $FILE" >&2
			exit 1
		}

		# See if required perms match current perms.

		if [ "$RIGHTPERMS" != "$ACTUALPERMS" ];then
			echo "correct...$RIGHTPERMS"
			echo "*ACTUAL*..$ACTUALPERMS"
			EXIT_STATUS=2
		elif [ $VERBOSE = 'y' ];then
			echo "correct...$ACTUALPERMS"
		fi
	else
		# No checking, just set the perms to required values.

		if eval .perm $PERMLIST $LNUM; then
			if [ $VERBOSE = 'y' ];then
				echo "set.......$RIGHTPERMS"
			fi
		else
			echo "FAILED:...$RIGHTPERMS ($?)"
			EXIT_STATUS=3
		fi
	fi
done

exit $EXIT_STATUS
