# c/n1/i8086/Makefile 4/25/91
# Execute $(TABGEN) to build new code tables in current directory.
# Expects $(TABGEN) and $(TFLAGS) to be set accordingly.

ALL=\
	prefac.f \
	aadd.t \
	aand.t \
	add.t \
	adiv.t \
	aft.t \
	amul.t \
	and.t \
	aor.t \
	arem.t \
	ashl.t \
	ashr.t \
	assign.t \
	asub.t \
	axor.t \
	bef.t \
	div.t \
	leaves.t \
	mul.t \
	neg.t \
	not.t \
	or.t \
	relop.t \
	rem.t \
	shl.t \
	shr.t \
	sub.t \
	xor.t

macros.c patern.c: $(ALL)
	$(TABGEN) $(TFLAGS) $<

# end of c/n1/i8086/Makefile
