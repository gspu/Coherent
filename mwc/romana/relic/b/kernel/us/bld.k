#
# bld.k - make most of a kernel
#	omit HD device driver and KB device driver
#	so these can be linked in during installation
#
# $Log:	bld.k,v $
Revision 1.1  92/07/17  15:29:52  bin
Initial revision

# Revision 1.1  92/01/10  10:10:41  hal
# Initial revision


#
#	initialize variables
#
#LIBS="lib/tty.a lib/support.a"
UNDEF=""
PATCH=""
KERNEL=atkernel.o
RESULT=kernel321.o
DRIVERS="rm fl lp"

echo "Devices:   $DRIVERS"

for ARG in $DRIVERS
do
	if /bin/test -r confdrv/${ARG}
	then
		. confdrv/${ARG}
	else
		/bin/echo "'confdrv/${ARG}' does not exist"
		exit 1
	fi
done

# UNDEF was built on in confdrv/ scripts
UNDEF="${UNDEF} ${LIBS}"

#	make the proper root and pipe devices
#
PATCH="${PATCH} rootdev_=0 pipedev_=0"

set -ex
/bin/ld -r -x -o $RESULT ${KERNEL} ${UNDEF}
/conf/patch $RESULT ALLSIZE_=16384 NBUF_=32 NCLIST_=24 ${PATCH}
set -
echo $RESULT created
