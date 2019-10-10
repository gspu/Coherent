# Master makefile

CONFDIR=.
PREFIX=.
MANIFEST=$(PREFIX)/conf
MAKEFILE=Makefile
GCC=gcc
#GCCFLAGS=-O2 $(CFLAGS) # -Wall -ansi -pedantic
GCCFLAGS=$(CFLAGS) # -Wall -ansi -pedantic
GLDFLAGS=-nostdlib /lib/crts0.o
GLIBFLAGS=/lib/libc.a

MAKEARGS="CC=$(CC)" "CFLAGS=$(CFLAGS)" "LDFLAGS=$(LDFLAGS)" \
	"LIBFLAGS=$(LIBFLAGS)" "CONFDIR=$(CONFDIR)"
EXTRA_ARGS=	MTUNE_FILE=$(MTUNE_FILE) STUNE_FILE=$(STUNE_FILE) \
		MDEV_FILE=$(MDEV_FILE) SDEV_FILE=$(SDEV_FILE) \
		TEMPL_FILE=$(TEMPL_FILE) KEEP_FILE=$(KEEP_FILE) \
		FORCE_KERNEL=$(FORCE_KERNEL)
BIN=bin

DRIVERS=$(CONFDIR)/drvbld.mak

everything:	all $(DRIVERS) configure

all clean setup depend:
	@+j=$$(pwd); for i in */src/[Mm]akefile; do \
		case $$i in \
		[!*]*)	cd $${i%/*} ; echo $$(pwd); \
			$(MAKE) $(MAKEARGS) $(EXTRA_ARGS) \
				PREFIX=$(MANIFEST)/$${i%%/*} $@ || exit 1; \
			cd $$j; ;; \
		esac \
	done

gcc:
	@+exec $(MAKE) $(EXTRA_ARGS) \
		"CC=$(GCC)" "CFLAGS=$(GCCFLAGS)" "LDFLAGS=$(GLDFLAGS)" \
		"LIBFLAGS=$(GLIBFLAGS)" "CONFDIR=$(CONFDIR)" -f $(MAKEFILE)

# The only difference between this and the action above is Hal's desire
# to see the system walk over the directories.

manifest distrib:
	@j=$$(pwd); for i in */src/[Mm]akefile; do \
		case $$i in \
		[!*]*)	cd $${i%/*} ; \
			$(MAKE) $(MAKEARGS) $(EXTRA_ARGS) \
				PREFIX=$(MANIFEST)/$${i%%/*} $@ || exit 1; \
			cd $$j; ;; \
		esac \
	done

# Update binaries of drivers and the 'devadm' program
# As a detail, we check to see that a device for major number 4 is installed,
# otherwise the kernel will be built without a console and will fail to be
# very useful.

$(DRIVERS):	$(MDEV_FILE) $(SDEV_FILE) \
		$(TEMPL_FILE) $(BIN)/idcheck \
		$(BIN)/devadm
	@$(BIN)/idcheck -i $(CONFDIR) -B -l 4 -u 4; if [ $$? -gt 0 -a $$? -lt 100 ]; \
		then exit 0; \
		else echo "No console device configured"; exit 1;\
		fi
	CONFPATH=$$(pwd) $(BIN)/devadm -X $(MDEV_FILE) -Y $(SDEV_FILE) \
				-Z $(TEMPL_FILE) -d -I $(CONFDIR)

configure:
	export CONFPATH=$$(pwd); cd $(CONFDIR); \
		$(MAKE) $(MAKEARGS) $(EXTRA_ARGS) \
			K386LIB=$(K386LIB) TARGET=$(TARGET) -f drvbld.mak

# Set up additional dependencies for top-level actions, and then define the
# additional actions.

clean:	conf_clean
setup:	conf_setup
manifest: conf_manifest
distrib: conf_distrib

conf_clean:
	rm -f drvbld.mak conf.[ch] obj/*

conf_setup:
	[ -d obj ] || mkdir obj
	[ -d lib ] || mkdir lib

conf_manifest:
	@for i in Makefile; do \
		echo $(MANIFEST)/$$i; \
	done

conf_distrib:
	@for i in Makefile bin/id* bin/devadm bin/cohtune bin/ndptype \
		  bin/bports bin/kbdselect bin/conf_bits \
		  bin/cxtype bin/conf_lib.sh lib/k*; do \
		echo $(MANIFEST)/$$i; \
	done

