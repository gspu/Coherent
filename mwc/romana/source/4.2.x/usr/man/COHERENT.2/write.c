wwrriittee -- Command

Converse with another user
wwrriittee _u_s_e_r [ _t_t_y ]

The  COHERENT  system   provides  several  commands  that  allow  users  to
communicate with  each other.  wwrriittee allows two logged-in  users to have an
extended, interactive conversation.

wwrriittee initiates a conversation with _u_s_e_r.  If _t_t_y is given, wwrriittee looks for
the _u_s_e_r  on that  terminal; this is  useful if a  user is marked  as being
logged in on more than one device.  Otherwise, wwrriittee holds the conversation
with the first instance of _u_s_e_r found on any tty.

If found,  wwrriittee notifies _u_s_e_r  that you are beginning  a conversation with
him.  All  subsequent lines  typed into wwrriittee  are forwarded to  the _u_s_e_r's
terminal, except lines beginning with `!',  which are sent to the shell sshh.
Typing end  of file (usually <ccttrrll-DD>) terminates wwrriittee  and sends _u_s_e_r the
message ``EOT'' to tell him that communication has ended.

Two users  typing lines to wwrriittee  at about the same  time can cause extreme
confusion,  so users  should agree  on  a protocol  to limit  when each  is
typing.  The  following protocol is suggested.  One  user initiates a wwrriittee
to another,  and waits until the other user  replies before beginning.  The
first user then types until he  wishes a reply and suffixes ``o'' (over) to
indicate he is through.  The other user does the same, and the conversation
alternates until  one user wishes to terminate it.   This user types ``oo''
(over and out).  The other user  replies in the same way, indicating he too
is finished.   Finally each of the users leave  wwrriittee by typing end-of-file
(usually <ccttrrll-DD>).

Any user may  deny others the permission to wwrriittee  to his terminal by using
the command mmeessgg.

_F_i_l_e_s
/eettcc/uuttmmpp
/ddeevv/*

_S_e_e _A_l_s_o
ccoommmmaannddss, mmaaiill, mmeessgg, mmssgg, sshh, wwaallll, wwhhoo

_N_o_t_e_s
You  should use  wwrriittee only  for extended conversations.   Use mmssgg  to send
brief communications  to a logged in  user, and mmaaiill to  communicate with a
user not currently  logged in.  wwaallll broadcasts a message  to all logged in
users.
