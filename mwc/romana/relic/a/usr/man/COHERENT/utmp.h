

utmp.h                     Header File                     utmp.h




Login accounting information

#include <utmp.h>

/etc/utmp contains  a utmp entry for  every user currently logged
into the  COHERENT system.  The structure utmp  is defined in the
the header file utmp.h, as follows:


#define DIRSIZ  14
struct utmp {
        char    ut_line[8];     /* terminal name */
        char    ut_name[DIRSIZ];/* user name */
        time_t  ut_time;        /* time of login */
};


If either the user name or terminal name is cleared, the entry is
unused.  The element ut_line is  the name of the special file for
the user's terminal, and is normally found in the directory /dev.
ut_time gives the date and time the user logged into COHERENT.

The  file /usr/adm/wtmp  maintains  a record  of  all logins  and
logouts, and may be  summarized by the command ac.  The processes
login and init write  entries into the file wtmp; neither creates
the file,  so login  accounting is disabled  unless /usr/adm/wtmp
exists.

Entries in wtmp are identical to those in utmp.  A null string in
the  ut_name  field  indicates  a  logout.  The  following  three
special terminal names may be  found in wtmp.  When the system is
booted, init  writes a  ut_line entry of  `~'.  When the  time is
changed with the command date,  it writes an entry giving the old
date (`|')  and an entry giving the new  date (`}').  This allows
ac to adjust connect times appropriately.

***** Files *****

<utmp.h>
/etc/utmp
/usr/adm/wtmp

***** See Also *****

ac, date, file formats, header files, init, login, who











COHERENT Lexicon                                           Page 1


