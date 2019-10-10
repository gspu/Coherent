ssttddlliibb.hh -- Header File

Declare/define general functions
#iinncclluuddee <ssttddlliibb.hh>

ssttddlliibb.hh  is a  header  file that  is  defined in  the  ANSI Standard.   It
declares a  set of general utilities and defines  attending macros and data
types, as follows.

_T_y_p_e_s
ddiivv_tt.......Type of object returned by ddiivv
llddiivv_tt......Type of object returned by llddiivv

_M_a_n_i_f_e_s_t _C_o_n_s_t_a_n_t_s
EEXXIITT_FFAAIILLUURREEValue to indicate that program failed to execute properly
EEXXIITT_SSUUCCCCEESSSSValue to indicate that program executed properly
MMBB_CCUURR_MMAAXX..Largest size of multibyte character in current locale
MMBB_LLEENN_MMAAXX..Largest overall size of multibyte character in any locale
RRAANNDD_MMAAXX....Largest size of pseudo-random number

_F_u_n_c_t_i_o_n_s
aabboorrtt().....End program immediately
aabbss().......Compute the absolute value of an integer
aattooff()......Convert string to floating-point number
aattooii()......Convert string to integer
aattooll()......Convert string to long integer
bbsseeaarrcchh()...Search an array
ccaalllloocc()....Allocate dynamic memory
ddiivv().......Perform integer division
eexxiitt()......Terminate a program gracefully
ffrreeee()......De-allocate dynamic memory to free memory pool
ggeetteennvv()....Read environmental variable
llaabbss()......Compute the absolute value of a long integer
llddiivv()......Perform long integer division
mmaalllloocc()....Allocate dynamic memory
qqssoorrtt().....Sort an array
rraanndd()......Generate pseudo-random numbers
rreeaalllloocc()...Reallocate dynamic memory
ssrraanndd().....Seed the random-number generator
ssttrrttoodd()....Convert string to floating-point number
ssttrrttooll()....Convert string to long integer
ssttrrttoouull()...Convert string to unsigned long integer
ssyysstteemm()....Suspend a program and execute another

_S_e_e _A_l_s_o
hheeaaddeerr ffiilleess
ANSI Standard, section 7.10
