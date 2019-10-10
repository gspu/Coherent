# basename.sh - Shell script illustrating use of "basename".
# Fri Oct 29 12:21:25 1993 CST

# "basename" is pretty much obsolete if you have pattern matching.

# This is a little script that makes copies of all .c files,
# replacing the ".c" suffix with ".backup".

#
# Example using basename.
#

# for i in `ls *.c`
# do
# 	cp $i `basename $i .c`.backup
# done

#
# Alternate method using shell pattern matching.
#

for i in *.c
do
	cp $i ${i%.c}.backup
done
