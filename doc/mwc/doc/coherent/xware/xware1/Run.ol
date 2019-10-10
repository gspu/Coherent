# Run
#
# Compile the release notes for Xware 1: Window Managers package
#

TROFF="troff -p -Z6 -rRN1 -rNS1 -rNI1"
OUTPUT=/tmp/doc/rel.notes
V=/v/doc/coherent

rm $OUTPUT/*

# do the manual pages
$TROFF -mcohlex $V/manual/fonts.r $V/manual/top.r lx/olwm lx/olvwm lx/olvwmrc \
	2> xware.x > $OUTPUT/olvwm.out
