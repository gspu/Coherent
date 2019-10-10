# Master makefile

all: drivers drvbld.mak configure

drivers:
	make -f mkdev.mak
	j=`pwd`;for i in `cat drvlist`; do cd $$i; make; cd $$j; done

drvbld.mak: mdevice sdevice template.mak
	mkdev -d

configure:
	make -f drvbld.mak

