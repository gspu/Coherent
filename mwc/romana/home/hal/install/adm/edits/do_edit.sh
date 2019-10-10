# Make a system file from a template.
# Insert
#	# <filename>
#	# <date
# at start of file, then make all substitutions requested.
#
# Usage
#	do_edit [old=new]... filename label
# "filename" is the name, with correct path, of the file to edit.
# "label" is a string, usually the path on destination bits, that will
# appear at the start of the edited file.
#
# Edits the file in place.

EDIT_CMD_FILE=/tmp/ed.$$
> $EDIT_CMD_FILE

for Arg
do
	case $Arg in
	*=*)
		LHS=${Arg%%=*}		# Strip longest suffix.
		RHS=${Arg#${LHS}=}	# Strip shortest prefix.

		# Escape slashes in LHS and RHS
		LHS=$(echo $LHS | sed -e 's/\//\\\//g')
		RHS=$(echo $RHS | sed -e 's/\//\\\//g')

		echo '1,$s'"/$LHS/$RHS/g" >> $EDIT_CMD_FILE
		shift
		;;
	esac
done

if [ $# -ne 2 ]; then
	echo "Usage: $0 [old=new]... file_to_edit" >&2
	rm -f $EDIT_CMD_FILE
	exit 1
fi

FILE_TO_EDIT=$1
LABEL=$2

if [ ! -f $FILE_TO_EDIT ];then
	echo "$0: can't open source file $FILE_TO_EDIT" >&2
	rm -f $EDIT_CMD_FILE
	exit 1
fi

DATE=$(date)

cat <<- _EOF >> $EDIT_CMD_FILE
1i
# ${LABEL}
# ${DATE}
.
_EOF
echo "wq" >> $EDIT_CMD_FILE

/bin/ed - +v $FILE_TO_EDIT < $EDIT_CMD_FILE
rm -f $EDIT_CMD_FILE

exit 0
