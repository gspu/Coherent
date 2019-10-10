#! /bin/sh
#
# @(#)mkfnames.sh	2.5 (smail) 9/15/87
#
if test $# = 0
then
	sed 's/\(.*\):.*:.*:.*:\(.*\):.*:.*/\1	\2/' /etc/passwd
else
	cat $*
fi	|	# at this point, we have a list of login\tFull Name pairs
nptx	|
lcasep	|
sort -u +0 -1
