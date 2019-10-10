# c/common/Makefile 4/25/91

COM=\
	bget.$X\
	bput.$X\
	dget.$X\
	diag.$X\
	dput.$X\
	getvar.$X\
	iget.$X\
	iput.$X\
	lget.$X\
	lput.$X\
	makvar.$X\
	milnam.$X\
	mionam.$X\
	newlab.$X\
	newseg.$X\
	nput.$X\
	segnam.$X\
	sget.$X\
	sput.$X\
	talloc.$X\
	tcpy.$X\
	unbget.$X
COMMCH=\
	geno.$X\
	mdlnam.$X\
	mdonam.$X\
	regnam.$X\
	tyname.$X

# Primary targets.
lib.a:		$(COM) $(COMMCH)
	ar rus lib.a $(COM) $(COMMCH)
lib.olb:	$(COM) $(COMMCH)
	lb c lib.olb
	lb r lib.olb $(COM) $(COMMCH)
lib.lib:	$(COM) $(COMMCH)
	mwlib c lib $(COM) $(COMMCH)

# Machine independent.
bget.$X:	bget.c
bput.$X:	bput.c
dget.$X:	dget.c
diag.$X:	diag.c
dput.$X:	dput.c
getvar.$X:	getvar.c
iget.$X:	iget.c
iput.$X:	iput.c
lget.$X:	lget.c
lput.$X:	lput.c
makvar.$X:	makvar.c
milnam.$X:	milnam.c
mionam.$X:	mionam.c
newlab.$X:	newlab.c
newseg.$X:	newseg.c
nput.$X:	nput.c
segnam.$X:	segnam.c
sget.$X:	sget.c
sput.$X:	sput.c
talloc.$X:	talloc.c
tcpy.$X:	tcpy.c
unbget.$X:	unbget.c

# Machine dependent.
geno.$X:	geno.c
mdlnam.$X:	mdlnam.c
mdonam.$X:	mdonam.c
regnam.$X:	regnam.c
tyname.$X:	tyname.c

# end of c/common/Makefile
