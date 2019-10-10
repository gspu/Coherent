# c/.profile 4/25/91
# Assumes $SRC defines C compiler root source directory.

export SRCPATH=$SRC
export MCH=i8086
export CFLAGS="-O -I$SRC/h -I$SRC/h/$MCH"
export CLIB=../common/lib.a
export OUTPUT=outcoh.o
export TABGEN=../coh/tabgen
export TFLAGS="-s -d ONLYSMALL"
export X=o

# end of c/.profile
