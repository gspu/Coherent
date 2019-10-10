# obj/mcc/.profile 6/26/92
# for monolithic compiler with memory tempfiles

export AR=ar
export CC=cc
export SRC=/src386/bin/c		# source directory
obj=`pwd`			# object directory
. $SRC/.prof.386		# execute .profile for Makefiles
bin=$obj/bin			# set shell variables for directories
c=$SRC
coh=$c/coh
common=$c/common
commoni=$c/common/i386
h=$c/h
hi=$c/h/i386
n0=$c/n0
n0i=$c/n0/i386
n1=$c/n1
n1i=$c/n1/i386
export tables=$c/n1/i386/tables
n2=$c/n2
n2i=$c/n2/i386
n3=$c/n3
n3i=$c/n3/i386
V=D23002008100			# variant for explicit cc phase execution
export mcc=$obj/bin/mcc

ed - $hi/mch.h <$c/mch.mem.eds
ed - $h/host.h <$c/host.m.eds
echo Type \"make mcc\" to make.

# end of obj/c386/.profile
