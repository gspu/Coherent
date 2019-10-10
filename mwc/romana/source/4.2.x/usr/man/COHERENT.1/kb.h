kkbb.hh -- Header File

Define keys for loadable keyboard driver
#iinncclluuddee <ssyyss/kkbb.hh>

The header  file kkbb.hh defines  macros and manifest constants  that are used
with nnkkbb, the user-configurable keyboard driver.  It is iinncclluuddeed with the C
programs that the  user can modify and compile to  remap her keyboard.  See
the Lexicon entries nnkkbb and kkeeyybbooaarrdd ttaabblleess for more information.

nnkkbb  is also  used with  COHERENT  system of  virtual consoles.   kkbb.hh sets
default definitions for function keys, as follows:

vvtt00 -- vvtt1155
     Switch to logical session 0 through 15, respectively.

ccoolloorr00 -- ccoolloorr1155
     Switch to color session 0 through 15, respectively.

mmoonnoo00 -- mmoonnoo1155
     Switch to monochrome session 0 through 15, respectively.

vvttnn  Switch to next higher-numbered open session.

vvttpp  Switch to next lower-numbered open session.

vvtttt  Toggle to most recently used open session

_S_e_e _A_l_s_o
hheeaaddeerr ffiilleess, kkeeyybbooaarrdd ttaabblleess, nnkkbb, vviirrttuuaall ccoonnssoollee
