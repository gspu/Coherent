greet:
	@echo To compile all example programs, type
	@echo "\tmake all"
	@echo To compile just abs.c, type
	@echo "\tmake abs"
	@echo To make backup copies of all example files, type
	@echo "\tbasename.sh"

Example: Example.o
abs: abs.o
access: access.o
acos: acos.o
	cc -f acos.o -lm
argv: argv.o
array: array.o
asctime: asctime.o
atoi: atoi.o
atol: atol.o
buffer: buffer.o
byteordering: byteordering.o
calloc: calloc.o
ceil: ceil.o
	cc -f ceil.o -lm
cosh: cosh.o
	cc -f cosh.o -lm
ctime: ctime.o
ctype: ctype.o
curses: curses.o
	cc curses.o -lcurses -lterm
__DATE__: __DATE__.o
envp: envp.o
execve: execve.o
exp: exp.o
	cc -f exp.o -lm
fdopen: fdopen.o
ferror: ferror.o
fgetc: fgetc.o
fgets: fgets.o
fileno: fileno.o
fopen: fopen.o
fputc: fputc.o
freopen: freopen.o
frexp: frexp.o
	cc -f frexp.o -lm
fscanf: fscanf.o
fseek: fseek.o
getc: getc.o 
getenv: getenv.o
gets: gets.o
goto: goto.o
j0: j0.o
	cc -f j0.o -lm
localtime: localtime.o
malloc: malloc.o
modf: modf.o
	cc -f modf.o
open: open.o
preproc: preproc.o
	cc -f preproc.o -lm
modulus: modulus.o
printf: printf.o
	cc -f printf.o
putc: putc.o
rand: rand.o
rindex: rindex.o
scanf: scanf.o
sizeof: sizeof.o
sscanf: sscanf.o
stringfun: stringfun.o
strncpy: strncpy.o
swab: swab.o
system: system.o
tolower: tolower.o
unlink: unlink.o

all:	curses       buffer	      exp	     getc	    putc\
	Example      byteordering     fdopen	     getenv	    rand\
	__DATE__     calloc	      ferror	     gets	    rindex\
	abs	     ceil	      fgetc	     goto	    scanf\
	access       cosh	      fgets	     j0	            sizeof\
	acos	     ctime	      fileno	     localtime      sscanf\
	all	     ctype	      fopen	     malloc	    stringfun\
	argv	     		      fputc	     modf	    strncpy\
	array	     freopen	      modulus	     swab	    asctime\
        frexp	     open	      system	     atoi	    envp\
        fscanf	     preproc	      tolower	     atol	    execve\
        fseek	     printf	      unlink 	     
				>all_done

clean:	
	rm   -f      buffer	      exp	     getc	    putc\
	Example      byteordering     fdopen	     getenv	    rand\
	__DATE__     calloc	      ferror	     gets	    rindex\
	abs	     ceil	      fgetc	     goto	    scanf\
	access       cosh	      fgets	     j0	            sizeof\
	acos	     ctime	      fileno	     localtime      sscanf\
	all	     ctype	      fopen	     malloc	    stringfun\
	argv	     		      fputc	     modf	    strncpy\
	array	     freopen	      modulus	     swab	    asctime\
        frexp	     open	      system	     atoi	    envp\
        fscanf	     preproc	      tolower	     atol	    execve\
        fseek	     printf	      unlink 	     *.o	    tmpfile\
	all_done     curses
			echo all clean
