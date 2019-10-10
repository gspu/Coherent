# dos/Makefile 6/26/92

CFLAGS= -A 
DESTDIR=/interim/bin/
DOSBIN386=/bin386/bin
DOSBIN=/interim/bin

#-o $(DOSOBJ)/$@ 
#CFLAGS=-DDEBUG 

OBJ=\
	copyright.o\
	dos0.o\
	dos1.o\
	dos2.o\
	dos3.o\
	dos4.o
#OBJ=\
#	$(DOSOBJ)/copyright.o\
#	$(DOSOBJ)/dos0.o\
#	$(DOSOBJ)/dos1.o\
#	$(DOSOBJ)/dos2.o\
#	$(DOSOBJ)/dos3.o\
#	$(DOSOBJ)/dos4.o
HDR=\
	bpb.h\
	dos0.h\
	dos1.h\
	mdir.h
SRC=\
	copyright.c\
	dos0.c\
	dos1.c\
	dos2.c\
	dos3.c\
	dos4.c\


# Primary targets.
dos386:	$(OBJ)
	$(CC) -i -o $(DOSBIN386)/dos $(OBJ)
	ln -f $(DOSBIN386)/dos $(DOSBIN386)/dosdir
	ln -f $(DOSBIN386)/dos $(DOSBIN386)/dosls	
	ln -f $(DOSBIN386)/dos $(DOSBIN386)/dosmkdir
	ln -f $(DOSBIN386)/dos $(DOSBIN386)/doscp
	ln -f $(DOSBIN386)/dos $(DOSBIN386)/doscpdir
	ln -f $(DOSBIN386)/dos $(DOSBIN386)/dosdel
	ln -f $(DOSBIN386)/dos $(DOSBIN386)/doscat
	ln -f $(DOSBIN386)/dos $(DOSBIN386)/dosrm
	ln -f $(DOSBIN386)/dos $(DOSBIN386)/dosrmdir
	ln -f $(DOSBIN386)/dos $(DOSBIN386)/dosformat
	ln -f $(DOSBIN386)/dos $(DOSBIN386)/doslabel
dos:	$(OBJ)
	$(CC) -i -o $(DESTDIR)dos $(OBJ)
	ln -f $(DOSBIN)/dos $(DOSBIN)/dosdir
	ln -f $(DOSBIN)/dos $(DOSBIN)/dosls	
	ln -f $(DOSBIN)/dos $(DOSBIN)/dosmkdir
	ln -f $(DOSBIN)/dos $(DOSBIN)/doscp
	ln -f $(DOSBIN)/dos $(DOSBIN)/doscpdir
	ln -f $(DOSBIN)/dos $(DOSBIN)/dosdel
	ln -f $(DOSBIN)/dos $(DOSBIN)/doscat
	ln -f $(DOSBIN)/dos $(DOSBIN)/dosrm
	ln -f $(DOSBIN)/dos $(DOSBIN)/dosrmdir
	ln -f $(DOSBIN)/dos $(DOSBIN)/dosformat
	ln -f $(DOSBIN)/dos $(DOSBIN)/doslabel

# Secondary targets.
$(DOSOBJ)/copyright.o:	copyright.c
	cc -A -c -o $@ copyright.c

$(DOSOBJ)/dos0.o:	dos0.c $(HDR)
	cc -A -c -o $@ dos0.c

$(DOSOBJ)/dos1.o:	dos1.c $(HDR)
	cc -A -c -o $@ dos1.c

$(DOSOBJ)/dos2.o:	dos2.c $(HDR)
	cc -A -c -o $@ dos2.c

$(DOSOBJ)/dos3.o:	dos3.c $(HDR)
	cc -A -c -o $@ dos3.c

$(DOSOBJ)/dos4.o:	dos4.c $(HDR)
	cc -A -c -o $@ dos4.c
