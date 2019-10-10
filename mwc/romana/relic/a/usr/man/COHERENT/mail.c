

mail                         Command                         mail




Computer mail

mmaaiill [-mmppqqrrvv] [-ff _f_i_l_e] [_u_s_e_r ...]

mmaaiill allows  you to exchange electronic  mail with other COHERENT
system users,  either on your own system or  on other systems via
UUCP.  Depending  upon its form, this command  can be used either
to send mail to other users  or to read the mail that other users
have sent to you.

***** Sending Mail *****

If you name one or more _u_s_e_rs, mmaaiill assumes that you wish to send
a mail message to each _u_s_e_r. mmaaiill first prints the prompt


        Subject:


on the screen, requesting that you give the message a title.

mmaaiill then  reads what you type on the  standard input.  A message
is  terminated by  <ccttrrll-DD>,  by a  line that  contains only  the
character `.', or by a line that contains only the character `?'.
Ending with a question mark prompts mmaaiill to feed the message into
an editor for further editing.   The editor used is the one named
in  the environmental  variable EEDDIITTOORR. If  this variable  is not
defined, mmaaiill uses eedd.

If  you have  defined environmental variable  AASSKKCCCC to  YYEESS, mmaaiill
asks you, after  a message is ended, for a  list of users to whom
you wish to send a copy of the message.

Finally, mmaaiill prepends the  date and the sender's name, and sends
the result  to each _u_s_e_r named  either on the command  line or on
the carbon-copy list with the rrmmaaiill command.

Each _u_s_e_r  who has received mail is greeted  by the message ``You
have  mail.''  when  she  logs  in.   mmaaiill normally  changes  the
contents of  the mailbox  as the  user works with  them; however,
mmaaiill has options that allow the contents of the mailbox to remain
unchanged if the user desires.

***** Reading Mail *****

If no _u_s_e_r is named on  its command line, mmaaiill reads and displays
the user's  mail, message by message.   If environmental variable
PPAAGGEERR  is defined,  mmaaiill will ``pipe''  each message  through the
command it names.  For example, the .pprrooffiillee command line:







COHERENT Lexicon                                           Page 1




mail                         Command                         mail



        export PAGER="exec /bin/scat -1"


invokes  /bbiinn/ssccaatt for  each mail  message with  the command-line
argument -11 (the digit one).

While  reading  mail,  the user  can  use  any  of the  following
commands to  save, delete, or  send each message  to another user
interactively.

dd    Delete the current message and print the next message.

mm [_u_s_e_r ...]
     Mail  the  current  message  to  each _u_s_e_r  given  (default:
     yourself).

pp    Print the current message again.

qq    Quit, and update mailbox file to reflect changes.

rr    Reverse the direction in which the mailbox is being scanned.

ss [_f_i_l_e ...]
     Save the current mail  message with the usual header in each
     _f_i_l_e (default: $HHOOMMEE/mmbbooxx).

tt [_u_s_e_r ...]
     Send a  message read from the  standard input, terminated by
     an end-of-file character or by a line containing only `.' or
     `?', to each _u_s_e_r (default: yourself).

ww [_f_i_l_e ...]
     Write the  current message without the  usual header in each
     _f_i_l_e (default: $HHOOMMEE/mmbbooxx).

xx    Exit without updating the mailbox file.

<nneewwlliinnee>
     Print the next message.

-    Print the previous message.

EEOOFF  Quit, updating mailbox; same as qq.

?    Print a summary of available commands.

!_c_o_m_m_a_n_d
     Pass _c_o_m_m_a_n_d to the shell for execution.

The  following  command  line  options  control the  sending  and
reading of mail.

-ff _f_i_l_e
     Read   mail  from   _f_i_l_e  instead   of  from   the  default,
     /uussrr/ssppooooll/mmaaiill/_u_s_e_r.


COHERENT Lexicon                                           Page 2




mail                         Command                         mail




-mm   Send a message to the terminal  of _u_s_e_r if he is logged into
     the system when mail is sent.

-pp   Print all mail without interaction.

-qq   Quit without changing  the mailbox if an interrupt character
     is typed.   Normally, an interrupt  character stops printing
     of the current message.

-rr   Reverse  the  order of  printing  messages.  Normally,  mmaaiill
     prints messages in the order in which they were received.

-vv   Verbose mode.  Show  the version number of the mmaaiill program,
     and display expanded aliases.

If you wish, you can  create a signature file, .ssiigg.mmaaiill, in your
home directory.  mmaaiill  appends the contents of the signature file
to the  end of every  mail message you  send, as a  signature.  A
signature  can be  your system's path  name (for  uuuuccpp messages),
your telephone number, an amusing _b_o_n _m_o_t, or what you will.

***** Files *****

$HHOOMMEE/ddeeaadd.lleetttteerr -- Message that mmaaiill could not send
$HHOOMMEE/mmbbooxx -- Default saved mail
$HHOOMMEE/.ssiigg.mmaaiill -- Signature file
/ttmmpp/mmaaiill* -- Temporary and lock files
/uussrr/ssppooooll/mmaaiill -- Mailbox directory, filed by user name

***** See Also *****

aalliiaasseess,  AASSKKCCCC, ccoommmmaannddss,  EEDDIITTOORR, mmkkffnnaammeess,  mmssgg,  nnppttxx, PPAAGGEERR,
ppaatthhss, uuuuxx























COHERENT Lexicon                                           Page 3


