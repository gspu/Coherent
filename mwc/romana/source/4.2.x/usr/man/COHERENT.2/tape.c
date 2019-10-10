ttaappee -- Command

Manipulate a tape device
ttaappee ccoommmmaanndd [_c_o_u_n_t] [_d_e_v_i_c_e]

The command ttaappee lets you manipulate  a tape device.  _d_e_v_i_c_e names the tape
device to manipulate.  If you name no _d_e_v_i_c_e on the command line, ttaappee uses
the  device TT_DDEEFFAAUULLTT;  header file  <ttaappee.hh> defines  this constant  to be
device /ddeevv/ttaappee. For a list of  tape devices, see the overview article for
ttaappee.

_c_o_m_m_a_n_d names the task that you want ttaappee to execute, as follows:

rreetteennssiioonn
     Retension  the tape.   This  rewinds the  tape, then  performs a  full
     forward wind, then another rewind.  The seek offset is set to zero.

rreewwiinndd
     Rewind the tape.  This command  positions the tape at the beginning of
     track 0.   It resets seek offset  (see sseeeekk and tteellll,  below) to zero.
     If tape is already rewound, this command has no effect.

sseeeekk _l_o_c_a_t_i_o_n
     This command  has the same  effect as if  the tape had  just been used
     with no-rewind-on-close,  leaving the tape  at byte _l_o_c_a_t_i_o_n.  No tape
     motion occurs at  the time of the command, but  the next read or write
     begins at byte _l_o_c_a_t_i_o_n on the tape.

ssttaattuuss
     Display various  parameters for the tape drive,  and for the cartridge
     being  used.   Not  every  tape  drive  support every  status  option.
     Unsupported features  appear as ``unavailable''.   The following gives
     an example of output from this command:

         Floppy Tape Status:
             Drive Configuration  = 0x90
                 500 Kbits/sec
                 Non-Extra-Length Tape
                 QIC-80 Mode.
             ROM Version  = 0x85
             Vendor ID  = 0x0146, Make=5, Model=6
             Tape Status Unavailable.
             Drive Status  = 0x65
                 drive ready or idle
                 cartridge present
                 cartridge referenced
                 at physical BOT
             Drive Error Status - No Error.

tteellll Display the byte offset that will  be in effect the next time the tape
     is read or written.

_S_e_e _A_l_s_o
ccoommmmaannddss, hhaaii, ttaappee
