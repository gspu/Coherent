
From easix.gun.de!umunk!udo  Sun Jun 12 21:23:57 1994 remote from mwc
Received: by lepanto.chi.il.us (smail2.5.3-coh) id AA00121; 12 Jun 94 21:23:57 GMT (Sun)
Received: by mwc.com (smail2.5.3-coh) id AA09283; 12 Jun 94 20:26:27 GMT (Sun)
Received: from easix.gun.de by relay2.UU.NET with SMTP 
	(rama) id QQwtzz03784; Sun, 12 Jun 1994 15:56:18 -0400
Received: by easix.gun.de (Smail3.1.28.1)
	  from umunk with uucp
	  id <m0qCwXi-00014mA>; Sun, 12 Jun 94 20:54 CET
Received: by umunk.uucp (Smail3.1.28.1 #2)
	id m0qCxBt-0001AWC; Sun, 12 Jun 94 21:35 +0100
Message-Id: <m0qCxBt-0001AWC@umunk.uucp>
Subject: release notes
To: fred@mwc.com
Date: Sun, 12 Jun 1994 21:35:34 +0000 (+0100)
From: Udo Munk <udo@umunk.GUN.de>
Organization: U. Munk's private BBS
X-Mailer: ELM [version 2.4 PL21]
MIME-Version: 1.0
Content-Type: text/plain; charset=US-ASCII
Content-Transfer-Encoding: 7bit
Content-Length: 4727      
Status: RO

Fred,

distribution disks for GNU tools and GNU emacs are on the way, here
some informations for the release notes:

GNU tools:
---------------------------------------------------------------------------
Install the disks with:
/etc/install -c GNUtools devive no_of_disks
There are two 3,5" disks and three 5,25" disks. Please note the -c
option, it's neccessary, I figured out how to get install uncompressing
tar archives directely from floppy disks without copy them first.
The distribution needs ~6MB on the hard disk. All binaries are compiled
with the latest startup code and the latest gcc or the MWC cc, so
many programs will run without a NDP emulation in the kernel. But probably
some binaries, like perl, includes floating point instructions and the
NPD emulation must be linked into the kernel to run them on CPU's
without NDP. The whole stuff goes into /u1/gnu, like GCC too and the
manual pages go into /usr/man/GNU. There are two files left,
/usr/man/gnu.index.new and /u1/gnu/info/dir.new, which must be renamed
or merged with the existing files, the install script doesn't try to
do that, it could destroy the existing files.
The GNU copyleft is installed under /u1/gnu, users should read it. For
many programs like pearl, make, awk printed manuals are available from
the FSF, users should contact them and buy the manuals from them, to
support further development of free software.
Because of a bug in ksh one should add the following line to /etc/profile:
export LASTERROR=_, or GNU make will core dump when used from the ksh.
There is a program "locate" in the distribution, it does something similar
than qfind, but it's much better because it uses real regular expressions
for searching. Before it can be used a database must be build like as for
qfind too. The command for that is /u1/gnu/lib/locate/updatedb, it's a
good idea to run it frequently from a crontab, like the sample entry for
qfind in the root crontab too.
To activate the binaries the same action as for GNU C is required,
/u1/gnu/bin must go into the PATH variable. Here much more warnings
are neccessary than for GCC, the output of many programs is *much*
different and more UNIX like than from the MWC commands. Install scripts
for third party vendor stuff don't expect this output and installation
will fail. So before something else is installed users should unmount
the partition /u1/gnu or remove /u1/gnu/bin from the PATH variable.
In no case users should replace MWC binaries in /bin or /usr/bin with
the GNU binaries, one always uses $PATH to select which binaries one
want to run!!!!!
As with GCC the GNU info command is included, there should be some hints
to learn using it and read the available hypertext documentation.
---------------------------------------------------------------------------

GNU emacs:
---------------------------------------------------------------------------
Install the disks with
/etc/install -c GNUemacs device 2
There are two 3,5" and two 5,25" disks. The distribution needs 7MB free
space on the hard disk. The whole stuff goes into directory /u1/gnu.
Users can mount a partition to /u1/gnu before the install, as with the
other stuff too. It's neccessary to add /u1/gnu/bin to $PATH as for the
other stuff too. Users should remove the link /usr/bin/emacs (linked
to /usr/bin/me) and they should remove the man page emacs. Microemacs
is *far* away from beeing emacs, that's the same nonsense than the link
of csh to the Bourne Shell. IMHO this should be corrected in the
Coherent distribution too.
GNU emacs also includes a info mode to read texinfo hypertext. Unfortunately
GNU emacs uses an own directory for that /u1/gnu/lib/emacs/info. I don't
know if users prefer emacs or the info command to read the stuff. I would
suggest to move the files in ~/lib/emacs/info into /u1/gnu/info and merge
the both dir files there. Then one links all files in /u1/gnu/info to
/u1/gnu/lib/emacs/info and one can use both programs to read the docs.
Users should take a look at /u1/gnu/lib/emacs/lisp/default.el, this is
a default customization of emacs for Coherent, which makes the beast a
little bit more usefull. Then there is lisp code under
/u1/gnu/lib/emacs/lisp/term for terminal support (ansipc, vt100, other DEC
terminals and xterm). This stuff should be used as example to to configure
emacs for a new terminal.
The FSF offers a printed manual for emcas, users should conntact them and
buy it from them, to support further creation of free software.
---------------------------------------------------------------------------
Of course all the stuff is Copyright by FSF.

I really really hope I didn't forget something essential...
-- 
Udo Munk  udo@mwc.com or udo@umunk.GUN.de, CIS: 100021,2515


