aatt -- Command

Execute commands at given time
aatt [ -vv ] [ -cc _c_o_m_m_a_n_d ] _t_i_m_e [ [ _d_a_y ] _w_e_e_k ] [ _f_i_l_e ]
aatt [ -vv ] [ -cc _c_o_m_m_a_n_d ] _t_i_m_e _m_o_n_t_h _d_a_y [ _f_i_l_e ]

aatt executes commands at a given time in the future.

If the  -cc option is  used, aatt executes  the following _c_o_m_m_a_n_d.  If _f_i_l_e is
named, aatt  reads the commands from  it.  If neither is  given, aatt reads the
standard input for commands.

If _t_i_m_e is a one-digit or  two-digit number, aatt interprets it as specifying
an hour.  If  _t_i_m_e is a three-digit or four-digit  number, aatt interprets it
as specifying an  hour and minutes.  If _t_i_m_e is  followed by aa, pp, nn, or mm,
aatt assumes  AAMM, PPMM, nnoooonn, or mmiiddnniigghhtt,  respectively; otherwise, it assumes
that _t_i_m_e indicates a 24-hour clock.  Note that you should _n_o_t type a colon
`:' in the time string.

For example, the command

    at -c "time | msg henry" 1450

set the ttiimmee  command to be executed at 2:50  PM, and pipe ttiimmee's output to
the mmssgg  command, which will  pass it to  the terminal of  user hheennrryy.  The
argument to the -cc option had  to be enclosed in quotation marks because it
contains spaces and special characters; if this were not done, aatt would not
be able  to tell when  the argument ended,  and so would  generate an error
message.  If you wish to pass  information to a user's terminal with the aatt
command, you must tell aatt to whom to send the information.  The command

    at 250p commandfile

sets the  file ccoommmmaannddffiillee to be  read and executed at 2:50  PM.  It is _n_o_t
necessary to use the file's full path name.  Also, if the suffix pp were not
appended to the time, the file would be set to be read at 2:50 AM.

The time set in aatt's command line is _n_o_t the exact time that the command is
executed.   Rather,  the  daemon  ccrroonn  wakes  up the  file  /uussrr/lliibb/aattrruunn
periodically  to see  if any  commands have been  scheduled commands  to be
executed at  or before that  time.  The frequency with  which ccrroonn executes
aattrruunn  determines the  ``granularity''  of aatt  execution times;  it may  be
changed by editing the file /uussrr/lliibb/ccrroonnttaabb. For example, the entry

    0,5,10,15,20,25,30,35,40,45,50,55 * * * *   /usr/lib/atrun

sets  /uussrr/lliibb/aattrruunn  to be  executed  every five  minutes.   Thus, the  aatt
command that is  set, for example, to 2:53 PM  will actually be executed at
2:55  PM.  aattrruunn  executes specified  commands when  it discovers  that the
given time is past; therefore, aatt  commands are executed even if the system
is down at the specified time or if the system's time is changed.

The  aatt command  has two  forms, as  shown above.  In  the first  form, the
option _d_a_y names  a day of the week (lower  case, spelled out).  If wweeeekk is
specified, aatt  interprets the given _t_i_m_e  and _d_a_y as meaning  that time and
day the following week.  For example, the command

    at -c "time | msg henry" 1450 friday week

executes ttiimmee and sends its output to hheennrryy's terminal one week from Friday
at 2:50 PM.

In the second  form given above, _m_o_n_t_h specifies a  month name (lower case,
spelled out) and the number _d_a_y specifies a day of the month.  For example,
the command

    at 1450 july 4 commandfile

set the file ccoommmmaannddffiillee to be read at 2:50 PM on July 4.

If  the -vv flag  is given,  aatt prints  the time when  the commands  will be
executed, giving  you enough information  to plan for the  execution of the
command.  For  example, if it is  now August 13, 1990, at  2:30 PM, and you
type the command

    at -v -c "/usr/games/fortune | msg henry" 1435

aatt will reply:

    Tue Aug 13 14:35:00

indicating  that  the  command will  be  executed  five  minutes from  now.
However, if you type

    at -v -c "/usr/games/fortune | msg henry" 1435 august 10

aatt will reply

    Sun Aug 10 14:35:00 1991

which indicates  that on Sunday,  August 10 of  next year, at  2:35 PM, the
COHERENT system will print a ffoorrttuunnee onto your terminal.

Should you create such a long-distance aatt file by accident, you can correct
the error  by simply deleting the  file that encodes it  from the directory
/uussrr/ssppooooll/aatt.  The  file will be  named after the  time that it  is set to
execute, plus  a unique two-character suffix, should  more than one command
be scheduled to run at the  same time.  For example, the file for the above
command would be named 99110088110011443355.aaaa.

Finally, note that the current working directory, exported shell variables,
file  creation mask,  user id,  and group  id are  restored when  the given
command is executed.

_F_i_l_e_s
/bbiinn/ppwwdd -- To find current directory
/uussrr/lliibb/aattrruunn -- Execute scheduled commands
/uussrr/ssppooooll/aatt -- Scheduled activity directory
/uussrr/ssppooooll/aatt/ _y_y_m_m_d_d_h_h_m_m._x_x -- Commands scheduled at given time

_S_e_e _A_l_s_o
aatt, ccoommmmaannddss, ccrroonn
