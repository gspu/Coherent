#!/bin/sh
# readbb.sh
# read the bounding-box parameters from an EPS image.  Use it because the
# GNU program "psbb" is far too finicky about what it calls an EPS file.
# By Fred Butzen, 10/16/1994

lx=0
ly=0
ux=0
uy=0

if [ $# -eq 0 ]; then
	echo Usage: readbox.sh filename.eps
	exit 1
fi

grep BoundingBox $1 | read JUNK lx ly ux uy ; echo .BB $lx $ly $ux $uy

# end of readbb.sh
