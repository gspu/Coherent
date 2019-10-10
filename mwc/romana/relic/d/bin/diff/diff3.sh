: /bin/diff3 4/3/91
: Usage: diff3 [ -ex3 ] file1 file2 file3
: Run two diffs, then merge their results.

FLAGS=
PID=$$
TMP=/tmp/d3${PID}
TMPA=${TMP}a
TMPB=${TMP}b

case "$1" in
-* )
	FLAGS=$1
	shift
esac

case $# in
3)
	;;
*)
	echo Usage: diff3 [-ex3] file1 file2 file3
	exit 2
esac

trap "rm $TMPA $TMPB; exit 2" 1 2 3 5

diff -d $1 $3 >$TMPA

case $? in
[01])
	diff -d $2 $3 >$TMPB
	;;
*)
	rm $TMPA
	exit 2
esac

case $? in
[01])
	/usr/lib/diff3 $FLAGS $TMP
esac

STATUS=$?
rm $TMPA $TMPB
exit $STATUS

: end of /bin/diff3
