# (-lgl
#	Coherent 386 release 4.2
#	Copyright (c) 1982, 1994 by Mark Williams Company.
#	All rights reserved. May not be copied without permission.
#	For copying permission and licensing info, write licensing@mwc.com
# -lgl)
# divvy.sh - divvy up a directory tree among several volumes
#
# Create lists of files in ./divvy/prefix_{1|2|...}

# Functions.

usage () {
	echo "Usage: $0 source_dir blocks_per_volume prefix"
	exit 1
}

# Main script.

# Check arguments.

if [ $# -ne 3 ]; then
	usage
else
	SOURCE_DIR=$1
	BLOCKS_PER_VOL=$2
	PREFIX=$3
fi

if [ ! -d $SOURCE_DIR ]; then
	echo "Can't access source directory $SOURCE_DIR"
	exit 1
fi

if [ $BLOCKS_PER_VOL -lt 100 ]; then
	echo "Suspicious value $BLOCKS_PER_VOL for blocks per volume."
	echo "Expect integer >= 100."
	exit 1
fi

if [ $BLOCKS_PER_VOL -gt 5000 ]; then
	echo "Suspicious value $BLOCKS_PER_VOL for blocks per volume."
	echo "Expect integer <= 5000."
	exit 1
fi

# See that needed subdirectory "divvy" of adm is present.

if [ ! -d divvy ];then
	echo "Can't access directory \"divvy\""
	exit 1
fi

# Clean up in case a previous divvy run used the same prefix.

rm -f divvy/${PREFIX}_* $PREFIX.srt

# Make list of files in the source tree, sorted by decreasing size.

echo "Sorting files in $SOURCE_DIR by size."
HERE=$(pwd)
cd $SOURCE_DIR
ls -s $(find . -type f | sed -e "s/^..//") | sort -rn > $HERE/$PREFIX.srt
cd $HERE

echo \
"Dividing $SOURCE_DIR, $BLOCKS_PER_VOL blocks per volume, " \
"prefix \"$PREFIX\""

# For each file F
#   For each volume V
#     If V has room for F
#	Assign F to V
#       Break from loop
#     Fi
#   Done
#   If file wasn't placed
#     Start a new volume and place it there
# Done

NEXT_VOL_NUM=1
VOLS=
cat $HERE/$PREFIX.srt | while read F_SIZE F_NAME
do
	# Now have file with name F_NAME, size (in blocks) F_SIZE.
	# echo "SIZE=$F_SIZE NAME=$F_NAME"

	# File F is currently assigned to no volume.
	F_VOL=

	# Is F too big to fit on a single volume?
	if [ $F_SIZE -gt $BLOCKS_PER_VOL ];then
		echo "$0 ERROR: File $F_NAME has size $F_SIZE, "\
		"too big for volume size $BLOCKS_PER_VOL."
		exit 1
	fi

	for V in $VOLS; do

		# Does file F fit on volume V?

		# NEW_SIZE is space in use on V plus size of F
		NEW_SIZE=$(eval expr '$'${V}_size + $F_SIZE)

		if [ $NEW_SIZE -le $BLOCKS_PER_VOL ]
		then
			# Yes, there is room for F on V.

			# Update number of blocks used on volume V.
			eval ${V}_size=$NEW_SIZE

			# Remember home volume for F.
			F_VOL=$V

			# F is placed, stop looking at volumes.
			break
		fi

	done

	# If no extant volume had room for F, start a new volume.
	if [ -z "$F_VOL" ];then
		V="${PREFIX}_${NEXT_VOL_NUM}"
		VOLS="$VOLS $V"
		NEXT_VOL_NUM=$(expr $NEXT_VOL_NUM + 1)

		# Initialize number of blocks used on volume V.
		eval ${V}_size=$F_SIZE

		# Remember home volume for F.
		F_VOL=$V

		echo $F_NAME > divvy/$F_VOL
	else
		echo $F_NAME >> divvy/$F_VOL
	fi

	echo $F_NAME $F_SIZE $F_VOL
done

# Report volume use.

echo 
echo "Volume\tBlocks Used."
echo -- "------\t------------"

for V in $VOLS; do
	eval echo \"$V\\t'$'${V}_size\"
done
