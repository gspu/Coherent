

at                           Command                           at




Execute commands at given time

aatt [ -vv ] [ -cc _c_o_m_m_a_n_d ] _t_i_m_e [ [ _d_a_y ] wweeeekk ] [ _f_i_l_e ]
aatt [ -vv ] [ -cc _c_o_m_m_a_n_d ] _t_i_m_e _m_o_n_t_h _d_a_y [ _f_i_l_e ]

at executes commands at a given time in the future.

If the -c option is  used, aatt executes the following command.  If
file  is named,  at reads  the commands from  it.  If  neither is
given, at reads the standard input for commands.

If time  is a one-digit or two-digit number,  aatt interprets it as
specifying an hour.  If  time is a three-digit or four-digit num-
ber, aatt interprets it as specifying an hour and minutes.  If _t_i_m_e
is followed  by aa, pp, nn,  or mm, at assumes AAMM,  PPMM, nnoooonn, or mmiidd-
nniigghhtt, respectively; otherwise,  it assumes that time indicates a
24-hour clock.

For example, the command


        at -c "time | msg henry" 1450


set the ttiimmee  command to be executed at 2:50  PM, and pipe ttiimmee's
output to the mmssgg command, which  will pass it to the terminal of
user  hheennrryy.  Note  that  argument to  the  -cc option  had to  be
enclosed  in  quotation  marks  because  it contains  spaces  and
special characters;  if this were not done, aatt  would not be able
to tell  when the argument ended, and so  would generate an error
message.  Also note that if you wish pass information to a user's
terminal with  the aatt command, you  must tell aatt to  whom to send
the information.  The command


        at 250p commandfile


will set the file ccoommmmaannddffiillee to be read and executed at 2:50 PM.
Note that it  is _n_o_t necessary to use the  file's full path name.
Also, if  the suffix pp  were not appended  to the time,  the file
would be set to be read at 2:50 AM.

The time set in aatt's command  line is _n_o_t the exact time that the
command is  executed.  Rather, the daemon cron  wakes up the file
/usr/lib/atrun  periodically to  see  if any  commands have  been
scheduled commands  to be executed  at or before  that time.  The
frequency   with  which  cron   executes  atrun   determines  the
``granularity''  of at  execution  times; it  may  be changed  by
editing the file /usr/lib/crontab.  For example, the entry






COHERENT Lexicon                                           Page 1




at                           Command                           at



        0,5,10,15,20,25,30,35,40,45,50,55 * * * *   /usr/lib/atrun


sets /uussrr/lliibb/aattrruunn to be executed every five minutes.  Thus, the
aatt command that is set, for  example, to 2:53 PM will actually be
executed at  2:55 PM.  atrun executes  specified commands when it
discovers that the given time is past; therefore, at commands are
executed even if  the system is down at the  specified time or if
the system's time is changed.

The aatt command has two forms, as shown above.  In the first form,
the option day names a day of the week (lower case, spelled out).
If week  is specified,  at interprets the  given time and  day as
meaning that  time and day the following  week.  For example, the
command


        at -c "time | msg henry" 1450 friday week


executes ttiimmee  and sends its output to  hheennrryy's terminal one week
from Friday at 2:50 PM.

In  the second  form given  above, month  specifies a  month name
(lower case,  spelled out) and the number day  specifies a day of
the month.  For example, the command


        at 1450 july 4 commandfile


set the file ccoommmmaannddffiillee to be read at 2:50 PM on July 4.

If the  -v flag is  given, at prints  the time when  the commands
will be  executed, giving you enough information  to plan for the
execution of  the command.  For example, if it  is now August 13,
1990, at 2:30 PM, and you type the command


        at -v -c "/usr/games/fortune | msg henry" 1435


aatt will reply:


        Tue Aug 13 14:35:00


indicating that  the command will  be executed five  minutes from
now.  However, if you type







COHERENT Lexicon                                           Page 2




at                           Command                           at



        at -v -c "/usr/games/fortune | msg henry" 1435 august 10


aatt will reply


        Sun Aug 10 14:35:00 1991


which indicates  that on Sunday, August 10 of  next year, at 2:35
PM, the COHERENT system will print a ffoorrttuunnee onto your terminal.

Should you  create such a long-distance aatt  file by accident, you
can correct the error by simply deleting the file that encodes it
from the  directory /uussrr/ssppooooll/aatt.  The file  will be named after
the time  that it is set to execute,  plus a unique two-character
suffix, should  more than one command be scheduled  to run at the
same time.  For example, the  file for the above command would be
named 99110088110011443355.aaaa.

Finally, note that  the current working directory, exported shell
variables, file creation mask, user id, and group id are restored
when the given command is executed.

***** Files *****

/bbiinn/ppwwdd -- To find current directory
/uussrr/lliibb/aattrruunn -- Execute scheduled commands
/uussrr/ssppooooll/aatt -- Scheduled activity directory
/uussrr/ssppooooll/aatt/ _y_y_m_m_d_d_h_h_m_m._x_x -- Commands scheduled at given time

***** See Also *****

at, commands, cron























COHERENT Lexicon                                           Page 3


