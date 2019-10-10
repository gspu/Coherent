

write                        Command                        write




Conduct interactive conversation with another user

wwrriittee _u_s_e_r [ _t_t_y ]

The COHERENT system provides several commands that allow users to
communicate with each other.  write allows two logged-in users to
have an extended, interactive conversation.

write initiates a conversation with user.  If tty is given, write
looks for the user on that  terminal; this is useful if a user is
marked as  being logged in  on more than  one device.  Otherwise,
write  holds the  conversation with  the  first instance  of user
found on any tty.

If found, write notifies  user that you are beginning a conversa-
tion with  him.  All subsequent  lines typed into  write are for-
warded to  the user's terminal, except  lines beginning with `!',
which  are sent  to the  shell sh.  Typing  end of  file (usually
<ctrl-D>) terminates write  and sends user the message ``EOT'' to
tell him that communication has ended.

Two users typing lines to write  at about the same time can cause
extreme confusion,  so users should agree on  a protocol to limit
when each  is typing.  The following  protocol is suggested.  One
user initiates a write to another, and waits until the other user
replies  before beginning.   The first user  then types  until he
wishes  a  reply and  suffixes  ``o'' (over)  to  indicate he  is
through.  The other user  does the same, and the conversation al-
ternates until one user  wishes to terminate it.  This user types
``oo'' (over  and out).  The other user replies  in the same way,
indicating he  too is finished.  Finally each  of the users leave
write by typing end-of-file (usually <ctrl-D>).

Any user may deny others  the permission to write to his terminal
by using the command mesg.

***** Files *****

/eettcc/uuttmmpp
/dev/*

***** See Also *****

commands, mail, mesg, msg, sh, wall, who

***** Notes *****

You should use write only for extended conversations.  Use msg to
send brief  communications to a logged in user,  and mail to com-
municate with a user  not currently logged in.  wall broadcasts a
message to all logged in users.





COHERENT Lexicon                                           Page 1


