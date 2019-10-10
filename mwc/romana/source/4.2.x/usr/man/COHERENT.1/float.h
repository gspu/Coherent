ffllooaatt.hh -- Header File

Define constants for floating-point numbers

The  header file  ffllooaatt.hh defines the  following manifest  constants, which
mark the  limits for computation  of floating-point numbers.   The prefixes
DDBBLL, FFLLTT, and LLDDBBLL refer, respective, to ddoouubbllee, ffllooaatt, and lloonngg ddoouubbllee:

DDBBLL_DDIIGG
     Number of decimal digits of precision.

DDBBLL_EEPPSSIILLOONN
     Smallest possible  floating-point number _x, such that  1.0 plus _x does
     not test equal to 1.0.

DDBBLL_MMAANNTT_DDIIGG
     Number of digits in the floating-point mantissa for base FFLLTT_RRAADDIIXX.

DDBBLL_MMAAXX
     Largest number that can be held by type ddoouubbllee.

DDBBLL_MMAAXX_EEXXPP
     Largest integer  such that the value of FFLLTT_RRAADDIIXX  raised to its power
     minus one is less than or equal to DDBBLL_MMAAXX.

DDBBLL_MMAAXX_1100_EEXXPP
     Largest integer  such that  ten raised  to its power  is less  than or
     equal to DDBBLL_MMAAXX.

DDBBLL_MMIINN
     Smallest number that can be held by type ddoouubbllee.

DDBBLL_MMIINN_EEXXPP
     Smallest integer such that the  value of FFLLTT_RRAADDIIXX raised to its power
     minus one is greater than or equal to DDBBLL_MMIINN.

DDBBLL_MMIINN_1100_EEXXPP
     Smallest integer such that ten raised  to its power is greater than or
     equal to DDBBLL_MMAAXX.

FFLLTT_DDIIGG
     Number of decimal digits of precision.

FFLLTT_EEPPSSIILLOONN
     Smallest floating-point  number _x, such that 1.0 plus  _x does not test
     equal to 1.0.

FFLLTT_MMAANNTT_DDIIGG
     Number of digits in the floating-point mantissa for base FFLLTT_RRAADDIIXX.

FFLLTT_MMAAXX
     Largest number that can be held by type ffllooaatt.

FFLLTT_MMAAXX_EEXXPP
     Largest integer  such that the value of FFLLTT_RRAADDIIXX  raised to its power
     minus one is less than or equal to FFLLTT_MMAAXX.

FFLLTT_MMAAXX_1100_EEXXPP
     Largest integer  such that  ten raised  to its power  is less  than or
     equal to FFLLTT_MMAAXX.

FFLLTT_MMIINN
     Smallest number that can be held by type ffllooaatt.

FFLLTT_MMIINN_EEXXPP
     Smallest integer such that the  value of FFLLTT_RRAADDIIXX raised to its power
     minus one is greater than or equal to FFLLTT_MMIINN.

FFLLTT_MMIINN_1100_EEXXPP
     Smallest integer such that ten raised  to its power is greater than or
     equal to FFLLTT_MMIINN.

FFLLTT_RRAADDIIXX
     Base  in  which  the  exponents  of  all  floating-point  numbers  are
     represented.

FFLLTT_RROOUUNNDDSS
     Manner  of rounding  used by  the  implementation.  The  ANSI Standard
     defines the rounding codes as follows:

    -11   Indeterminable, i.e., no strict rules apply
     00   Toward zero, i.e., truncation
     11   To nearest, i.e., rounds to nearest representable value
     22   Toward positive infinity, i.e., always rounds up
     33   Toward negative infinity, i.e., always rounds down

     COHERENT uses type-1 rounding.

LLDDBBLL_DDIIGG
     Number of decimal digits of precision.

LLDDBBLL_EEPPSSIILLOONN
     Smallest floating-point  number _x, such that 1.0 plus  _x does not test
     equal to 1.0.

LLDDBBLL_MMAANNTT_DDIIGG
     Number of digits in the floating-point mantissa for base FFLLTT_RRAADDIIXX.

LLDDBBLL_MMAAXX
     Largest number that can be held by type lloonngg ddoouubbllee.

LLDDBBLL_MMAAXX_EEXXPP
     Largest integer  such that the value of FFLLTT_RRAADDIIXX  raised to its power
     minus one is less than or equal to LLDDBBLL_MMAAXX.

LLDDBBLL_MMAAXX_1100_EEXXPP
     Largest integer  such that  ten raised  to its power  is less  than or
     equal to LLDDBBLL_MMAAXX.

LLDDBBLL_MMIINN
     Smallest  number that  can be  held by  type lloonngg  ddoouubbllee. Must  be no
     greater than 1E-37.

LLDDBBLL_MMIINN_EEXXPP
     Smallest integer such that the  value of FFLLTT_RRAADDIIXX raised to its power
     minus one is greater than or equal to LLDDBBLL_MMIINN.

LLDDBBLL_MMIINN_1100_EEXXPP
     Smallest integer such that ten raised  to its power is greater than or
     equal to LLDDBBLL_MMIINN.

_S_e_e _A_l_s_o
ddoouubbllee, ffllooaatt, hheeaaddeerr ffiilleess
ANSI Standard, section 5.2.4.2.2

_N_o_t_e_s
COHERENT's C compiler does not yet implement type lloonngg ddoouubbllee.
