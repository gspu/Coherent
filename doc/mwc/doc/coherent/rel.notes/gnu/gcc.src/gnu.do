
From uunet.uu.net!umunk!udo  Thu Jul 14 21:29:26 1994 remote from mwc
Received: by lepanto.mwc.com (smail2.5.3-coh) id AA05726; 14 Jul 94 21:29:26 GMT (Thu)
Received: by mwc.com (smail2.5.3-coh) id AA11067; 14 Jul 94 20:39:32 GMT (Thu)
Received: from nanolon.gun.de by relay1.UU.NET with SMTP 
	(relay) id QQwyog05486; Thu, 14 Jul 1994 16:34:41 -0400
Received: from umunk.UUCP (umunk@localhost) by nanolon.gun.de (8.6.8.1/8.6.6) with UUCP id WAA08460; Thu, 14 Jul 1994 22:31:13 +0200
Received: by umunk.uucp (Smail3.1.28.1 #2)
	id m0qOQlI-0001YJC; Thu, 14 Jul 94 13:23 +0100
Message-Id: <m0qOQlI-0001YJC@umunk.uucp>
Subject: release notes for gnu sources
To: fred@mwc.com
Date: Thu, 14 Jul 1994 13:23:31 +0000 (+0100)
From: Udo Munk <udo@umunk.GUN.de>
Cc: john@mwc.com
Organization: U. Munk's private BBS
X-Mailer: ELM [version 2.4 PL21]
MIME-Version: 1.0
Content-Type: text/plain; charset=US-ASCII
Content-Transfer-Encoding: 7bit
Content-Length: 7073      
Status: RO

Fred, John,

here are the release notes for the GNU development source distributions
I have so far. I don't know if this informations are complete enough,
but I configured and compiled the programs without theses informations,
I had the included documentation only of course. If you have problems
please let me know, so that we can improve this release notes.

----- cut here -----
------------------------------------------------------------------------------
Common for all GNU source distributions:
------------------------------------------------------------------------------

To compile the GNU development tool sources the GNU C compiler 2.5.6 at
least is required. The GNU tools also should be installed, really necessary
are GNU awk, GNU sed GNU make and GNU bash, else the sources will not compile
without problems. GNU bash should be installed as /bin/sh for compiling
the sources, if one want's to configure the packages it's a must. Also
/bin/sh must be configured as login shell, so that GNU bash is used
and not the korn shell. All packages already are configured ready to
compile under Coherent 4.2 (r09 and later), for compilation under
Coherent it's not necessary to run configure first.
All source distributions install into directory /u1/gnu/src, so it's
possible to have /u1 as a seperate partition to hold all this GNU stuff.
The binaries on the binary distribution disks are compiled with -O2,
optimized without debugging informations and the symbol table is striped.
If one want a binary which can be debugged, the command to compile a
package is: "make CC=gcc CFLAGS=-g". In the description for every
package the command for building optimized binaries is explained.
Printed manuals for the programs are available from the FSF, and the
full documentation also is included in all source distributions.
Reading the available documentation is a must, building compilers,
assemblers, debuggers and so on is *not* trivial.

------------------------------------------------------------------------------
GNU compiler source distribution:
------------------------------------------------------------------------------

The command to install the GNU compiler sources is:
/etc/install -c COHgcc device 5

After installation the whole source tree is in directory:
/u1/gnu/src/gcc-2.5.6

Installation of the source distribution requires 23MB free disk space, for
compiling the stuff 35-50MB (including the 23MB used by the sources) are
needed, that depends on how many stages of the compiler one wants to build.
It's not necessary to build the stage 3 compiler as described in the
README file, the compilers from the GNU development tools binary distribution
already are tested stage 3 compilers.

The command to compile the C compilers is:
make CC=gcc CFLAGS=-O2

The command to compile the Objective-C runtime library is:
make objc-runtime CC=gcc CFLAGS=-O

WARNING: Don't compile Objective-C programs with the compiler option -O2,
use -O, the optimizer for Objective-C has a known bug and the programs
will core dump.

The command to install the binaries and librarys and support files at the
right place is:
make install

The command to configure the C compilers for Coherent 4.2 is:
configure --target=i386-coh --with-stabs --prefix=/u1/gnu

The configure procedure builds modified header files in the include
directory.  All this header files other than in sub-directory objc aren't
needed, because the Coherent header files are GNU C compatible. It's
strongly recommended to remove this header files befor building the
compiler, so that the Coherent header files are used!!!

------------------------------------------------------------------------------
GNU assembler source distribution:
------------------------------------------------------------------------------

The command to install the GNU assembler sources is:
/etc/install -c COHgas device 2

After installation the whole source tree is in /u1/gnu/src/gas-2.2

The GNU assembler sources needs 8MB free disk space, to compile
the package 11MB free disk space (including the 8MB needed for the
sources) are required.

The command to compile the GNU assembler is:
make CC=gcc CFLAGS=-O2

The command to install the package is:
make install

The command to configure the GNU assembler for Coherent 4.2 is:
configure --target=i386-coh --prefix=/u1/gnu

------------------------------------------------------------------------------
GNU binutils source distribution:
------------------------------------------------------------------------------

The command to install the GNU binutils sources is:
/etc/install -c COHbin device 2

After installation the whole source tree is in /u1/gnu/src/binutils-2.4

The GNU binutils sources needs 8MB free disk space, to compile
the package 12MB free disk space (including the 8MB for the sources)
are required.

The command to compile the GNU binutils is:
make CC=gcc CFLAGS=-O2

The command to install the package is:
make install

This installs the binaries with the usual names (ld, ar, nm...) in the
directory /u1/gnu/i386-coh/bin and creates linkes to files in directory
/u1/gnu/bin which begins with i386-coh-. This kind of installation isn't
what we want, remove the links in /u1/gnu/bin/i386-coh* and move the
binaries with the right names from /u1/gnu/i386-coh/bin to /u1/gnu/bin.
Do *NOT* install the GNU strip command, it doesn't work correct!!!

The command to configure the GNU binutils for Coherent 4.2 is:
configure --target=i386-coh --with-stabs --prefix=/u1/gnu

------------------------------------------------------------------------------
GNU debugger source distribution:
------------------------------------------------------------------------------

The command to install the GNU debugger sources is:
/etc/install -c COHgdb device 3

After installation the whole source tree is in /u1/gnu/src/gdb-4.9

The GNU debugger sources needs 12MB free disk space, to compile
the package 15MB free disk space (including the 12MB needed for the
sources) are required.

The command to compile the GNU debugger is:
make CC=gcc CFLAGS=-O2

The command to install the package is:
make install

The command to configure the GNU debugger for Coherent 4.2 is:
configure --target=i386-coh --prefix=/u1/gnu

------------------------------------------------------------------------------
GNU C++ runtime library source distribution:
------------------------------------------------------------------------------

The command to install the GNU C++ library sources is:
/etc/install -c COHlibg device 2

After installation the whole source tree is in /u1/gnu/src/libg++-2.5.3

The GNU C++ library sources needs 6MB free disk space, to compile
the package 9MB free disk space (including the 6MB needed for the
sources) are required.

The command to compile the GNU C++ library is:
make CC=gcc CFLAGS=-O2

The command to install the package is:
make install

The command to configure the GNU C++ library for Coherent 4.2 is:
configure --target=i386-coh --prefix=/u1/gnu
----- cut here -----
-- 
Udo Munk  udo@mwc.com or udo@umunk.GUN.de, CIS: 100021,2515


