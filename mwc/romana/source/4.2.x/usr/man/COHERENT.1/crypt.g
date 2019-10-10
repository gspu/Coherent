ccrryypptt() -- General Function (libc)

Encryption using rotor algorithm
cchhaarr *ccrryypptt(_k_e_y, _e_x_t_r_a); cchhaarr *_k_e_y, *_e_x_t_r_a;

ccrryypptt() implements  a version of  rotor encryption.  It  produces encrypted
passwords that  are verified by comparing the  encrypted clear text against
an original encryption.

_k_e_y  is an  ASCII string  that contains  the user's  password.  _e_x_t_r_a  is a
string of  two additional characters, stored in the  password file with the
encrypted  password.   Each character  must  come from  an  alphabet of  64
symbols, which  consists of the upper-case  and lower-case letters, digits,
the period `.', and the slash `/'.

ccrryypptt()  returns a  string built from  the 64-character  alphabet described
above; the  first two characters  returned are the _e_x_t_r_a  argument, and the
rest contain the encrypted password.

_S_e_e _A_l_s_o
lliibbcc
