mmss -- Device Driver


/ddeevv/mmoouussee is a low-level interface to the traditional Microsoft bus mouse.
It does not  currently support the Microsoft InPort series  of mice.  It is
assigned major device 10, and is accessed as a character-special device.

The following iiooccttll routines provide access to the mouse:

    #include <sys/ms.h>
    struct msparms parm;
    struct mspos   mick;
    struct msbuts  buts;
    struct mspos   pos;
    int st;

    ioctl( fd, MS_SETUP,    &parm );
    ioctl( fd, MS_SETCRS,   &pos  );
    ioctl( fd, MS_GETCRS,   &pos  );
    ioctl( fd, MS_READBTNS, &buts );
    ioctl( fd, MS_READSTAT, &st   );
    ioctl( fd, MS_SETMICK,  &mick );
    ioctl( fd, MS_GETMICK,  &mick );

The iiooccttll call MMSS_SSEETTUUPP defines the initial setup for the mouse.  The field
aacccceell_tt  gives the  incremental movement  threshold at  which the  speed of
movement  will double.   The fields  hh_ccmmiinn and  hh_ccmmaaxx give  the allowable
range  of  horizontal movement.   The  fields vv_ccmmiinn  and  vv_ccmmaaxx give  the
allowable range  of vertical movement.  The fields  hh_mmpprr and vv_mmpprr specify
multipliers to be applied to movement.  A movement multipler of zero or one
provides single-tick resolution.

The iiooccttll call MMSS_SSEETTCCRRSS changes  the active position of the mouse, whereas
the call MMSS_GGEETTCCRRSS retrieves the mouse's current position.

The iiooccttll  call MMSS_RREEAADDBBTTNNSS retrieves the status of  the mouse buttons.  It
returns  the positions  at  which buttons  were pressed  and released,  and
clears the button status.

The iiooccttll call  MMSS_RREEAADDSSTTAATT identifies recently occurring mouse events.  If
the MMSS_SS_MMOOVVEE bit is set, the mouse has been moved and the new position can
be  obtained  by  the  iiooccttll  call  MMSS_GGEETTCCRRSS. The  bits  MMSS_SS_LL_PPRREESSSS  and
MMSS_SS_LL_RREELLEEAASSEE  indicate  that  the  left  button has  been,  respectively,
pressed or  released.  Likewise,  the bits MMSS_SS_RR_PPRREESSSS  and MMSS_SS_RR_RREELLEEAASSEE
indicate that the right button has been, respectively, pressed or released.
The position at  which a button was pressed or  released can be obtained by
the iiooccttll call MMSS_RREEAADDBBTTNNSS.

Finally, the iiooccttll call MMSS_SSEETTMMIICCKK changes the mouse-movement multiplers.

_F_i_l_e_s
/ddeevv/mmoouussee -- Character-special file
<ssyyss/mmss.hh> -- Include file

_S_e_e _A_l_s_o
ddeevviiccee ddrriivveerrss

_N_o_t_e_s
All mouse  support uses the same /uussrr/iinncclluuddee file.   However, each type of
mouse requires its own driver.
