
set -xv
:
: 'Diff3 - run two diffs, then merge their'
: 'results'
:
unique=$$

case "$1" in
-* )
	flag=$1
	shift
esac

case $# in
3)
	f1mark=$1;f2mark=$3;;
5)
	f1mark=$4;f2mark=$5;;
*)
	echo Usage: diff3 [-ex3EX] file1 file2 file3 [m1 m2]
	exit 2
esac

trap 'rm /tmp/d3${unique}[ab]; exit 2' 1 2 3 5

diff -d -n $1 $3 >/tmp/d3${unique}a

case $? in
[01])
	diff -d -n $2 $3 >/tmp/d3${unique}b
	;;

*)
	rm /tmp/d3${unique}[ab]
	exit 2
esac

case $? in
[01])
	/usr/lib/diff3 $flag /tmp/d3${unique} $f1mark $f2mark
:	diff3 $flag /tmp/d3${unique} $f1mark $f2mark
esac

stat=$?

rm /tmp/d3${unique}[ab]
exit $stat
