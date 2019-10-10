all: lp lpstat lpsched hpfilter
	@touch lpstat lp lpsched
	@rm -fr lpshut;  ln lp lpshut
	@rm -fr reprint; ln lp reprint
	@rm -fr cancel;  ln lpstat cancel
	@rm -fr chreq;   ln lpstat chreq
	@rm -fr route;   ln lpstat route
	@rm -fr lpadmin; ln lpstat lpadmin
	@sync
	@echo "Done\007"

spooler.o: header.h spooler.c
	@echo spooler
	@cc -c spooler.c

lpsched: header.h lpsched.c spooler.o
	@echo lpsched
	@cc lpsched.c spooler.o -lterm
	@strip lpsched

lp: header.h lp.c spooler.o
	@echo lp
	@cc lp.c spooler.o -lterm
	@strip lp

lpstat: header.h lpstat.c spooler.o
	@echo lpstat
	@cc lpstat.c spooler.o -lterm
	@strip lpstat
	@chmod u+s lpstat

hpfilter: hpfilter.c
	@echo hpfilter
	@cc hpfilter.c
	@strip hpfilter
	@cp hpfilter /usr/spool/mlp/backend/hpfilter
