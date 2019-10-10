uuttmmpp.hh -- Header File

Login accounting information
#iinncclluuddee <uuttmmpp.hh>

File  /eettcc/uuttmmpp contains  an entry  of type uuttmmpp  for every  user currently
logged  into your  COHERENT  system.  Header  file uuttmmpp.hh  defines uuttmmpp  as
follows:

#define DIRSIZ  14
struct utmp {
        char    ut_line[8];     /* terminal name */
        char    ut_name[DIRSIZ];/* user name */
        time_t  ut_time;        /* time of login */
};

If either the  user name or terminal name is  cleared, the entry is unused.
The  element  uutt_lliinnee is  the  name  of the  special  file  for the  user's
terminal, and  is normally found  in the directory /ddeevv.  uutt_ttiimmee gives the
date and time the user logged into COHERENT.

File  /uussrr/aaddmm/wwttmmpp maintains  a  record of  all logins  and logouts.   The
command aacc reads this file and prints a summary of its information for you.
The processes  llooggiinn and iinniitt write entries into  wwttmmpp; neither creates the
file, so login accounting is disabled unless /uussrr/aaddmm/wwttmmpp exists.

Entries in  wwttmmpp are  identical to  those in uuttmmpp.  A null string  in field
uutt_nnaammee indicates a logout.  The following three special terminal names may
be found in wwttmmpp. When the system is booted, iinniitt writes a uutt_lliinnee entry of
`~'.  When  the time is changed  with the command ddaattee,  it writes an entry
giving the  old date (`|')  and an entry  giving the new  date (`}').  This
allows aacc to adjust connect times appropriately.

_F_i_l_e_s
/eettcc/uuttmmpp
/uussrr/aaddmm/wwttmmpp

_S_e_e _A_l_s_o
aacc, ddaattee, ffiillee ffoorrmmaattss, hheeaaddeerr ffiilleess, iinniitt, llooggiinn, wwhhoo
