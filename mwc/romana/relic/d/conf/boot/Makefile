# boot/Makefile 8 Nov 1991
# COHERENT boot strap Makefile.

DESTDIR=/interim/conf/
TMP=boot.o
ALL=\
	boot \
	boot.at \
	boot.atx \
	boot.f9a \
	boot.f9d \
	boot.fha \
	boot.fqa \
	boot.fva \
	mboot

# Primary target.
all:	$(ALL)
	: Done.

# Install this code
install:
	Write me.  Put chmogs and cps here.

# Secondary targets.
boot: boot.at
	ln -f $(DESTDIR)boot.at $(DESTDIR)$@

# Hard disk secondary bootstrap.
boot.at:	boot.m
	cc -c -DNSPT=9 -DNTRK=40 -o $(TMP) $<
	dd if=$(TMP) of=$(DESTDIR)$@ ibs=1 skip=44 count=512
	rm $(TMP)

# Hard disk master boot block.
boot.atx:	mboot.s
	cc -c -o $(TMP) $<
	dd if=$(TMP) of=$(DESTDIR)$@ ibs=1 skip=44 count=512
	rm $(TMP)

# 720 block 9 sector per track floppy, alternating heads.
boot.f9a:	boot.m
	cc -c -DNHD=2 -DNSPT=9 -DNTRK=80 -o $(TMP) $<
	dd if=$(TMP) of=$(DESTDIR)$@ ibs=1 skip=44 count=512
	rm $(TMP)

# 720 block 9 sector per track floppy, not alternating heads.
boot.f9d:	boot.m
	cc -c -DNSPT=9 -DNTRK=80 -o $(TMP) $<
	dd if=$(TMP) of=$(DESTDIR)$@ ibs=1 skip=44 count=512
	rm $(TMP)

# 2400 block 15 sector per track 5.25" floppy, alternating heads.
boot.fha:	boot.m
	cc -c -DNHD=2 -DNSPT=15 -DNTRK=80 -o $(TMP) $<
	dd if=$(TMP) of=$(DESTDIR)$@ ibs=1 skip=44 count=512
	rm $(TMP)

# 1440 block 9 sector per track 3.5" floppy, alternating heads.
boot.fqa:	boot.m
	cc -c -DNHD=2 -DNSPT=9 -DNTRK=80 -o $(TMP) $<
	dd if=$(TMP) of=$(DESTDIR)$@ ibs=1 skip=44 count=512
	rm $(TMP)

# 2880 block 18 sector per track 3.5" HD floppy, alternating heads.
boot.fva:	boot.m
	cc -c -DNHD=2 -DNSPT=18 -DNTRK=80 -o $(TMP) $<
	dd if=$(TMP) of=$(DESTDIR)$@ ibs=1 skip=44 count=512
	rm $(TMP)

# Master boot block.
mboot:	boot.atx
	ln -f $(DESTDIR)boot.atx $(DESTDIR)$@

# Patchable bootstrap.
# The patchable secondary boot does NOT work!  More work needs doing.
# The od is to check that the signature lands at the end.
# If pboot changes, the code which patches it from /etc/fdisk must change too.
pboot:	boot.m
	cc -c -DPBOOT -DNHD=9 -DNSPT=26 -DNTRK=1024 -DWPCC=0xFFFF -DCNTRL=8 -o $(TMP) $<
	dd if=$(TMP) of=$(DESTDIR)$@ ibs=1 skip=44 count=512
	od -b $(DESTDIR)pboot | tail -1
	rm $(TMP)

# end of boot/Makefile
