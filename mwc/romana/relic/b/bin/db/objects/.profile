# db/obj/.profile 4/1/93

SRC=/src386/bin/db
export SRCPATH=$SRC:$SRC/i386
echo "Type \"make -e\" to make."

# Use MWC cc.
export CC=cc
export CFLAGS="-VNDP -I$SRC -I$SRC/i386 -D__KERNEL__=1"

# Use gcc.
# export CC=gcc
# export CFLAGS="-O2 -I$SRC -I$SRC/i386 -D__KERNEL__=1 -DPROTO_OK"

# end of db/obj/.profile
