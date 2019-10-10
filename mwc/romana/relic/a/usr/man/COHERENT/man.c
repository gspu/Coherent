

man                          Command                          man




Print online manual sections

mmaann [-ww] [_t_o_p_i_c ...]

man prints the  COHERENT lexicon entries for each specified topic
on the  standard output.  It uses scat to  display text (with the
-s  option to  suppress  blank lines).   With  no arguments,  man
prints a list of each available topic.

When used with the -w option, it prints the path name of the file
instead of printing the document itself.

If environmental variable PAGER is defined, man will ``pipe'' its
output through the  command specified in PAGER.  For example, the
.pprrooffiillee command line:


        export PAGER="exec /bin/scat -1"


would  invoke /bin/scat  with the command  line argument  -1 (the
digit one).

***** Manual-Page Control Files *****

The man  command uses two  control files when  processing manual-
page  requests.  File /usr/man/man.help  contains the  man's help
message.   This includes  a  list of  valid topics  and some  ex-
planatory  text.  The  second  control file,  /usr/man/man.index,
contains index entries for all manual pages on the system.  Lines
in this text file are of the form:


             _r_e_l_a_t_i_v_e-_p_a_t_h-_n_a_m_e _t_o_p_i_c


where  relative-path-name  gives  the  subdirectory and  file  in
/usr/man  that hold  the  manual-page entry,  and  topic gives  a
manual-page  topic  associated  with  this  file.   For  example,
entries


        COHERENT/ascii  ascii
        COHERENT/ascii  ASCII
        local/chess  chess


associate system  manual-page /usr/man/COHERENT/ascii with either
upper- or lower-case  spellings of topic ascii.  Similarly, rules
for   a    user-written   chess   game   are    found   in   file
/usr/man/local/chess and are retrieved using topic chess.





COHERENT Lexicon                                           Page 1




man                          Command                          man



***** Adding Manual-Page Entries *****

When writing  new manual-page entries for  COHERENT, we recommend
that you  place them in  subdirectories of /usr/man.   These sub-
directories should be uniquely named to avoid possible name-space
collisions.   A good  rule-of-thumb is  to name  the subdirectory
after  the application  with which it  is associated.   This also
allows them to  be updated easily, as all manual-pages associated
with a given application reside in a specific subdirectory.

When you  add manual-page entries to the  system, you should also
append a  list of topics to  /usr/man/man.help.  In addition, you
must append a line to the end of file /usr/man/man.index for each
newly added topic.

***** Files *****

/uussrr/mmaann/* -- Directories that hold manual pages

***** See Also *****

commands, help, install, PAGER, scat



































COHERENT Lexicon                                           Page 2


