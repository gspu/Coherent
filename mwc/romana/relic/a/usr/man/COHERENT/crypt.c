

crypt                        Command                        crypt




Encrypt/decrypt text

ccrryypptt [_p_a_s_s_w_o_r_d]

The command crypt  encrypts data.  It emulates a rotor-encryption
machine,  such as  the Enigma  or  Hagelin C-48  cipher machines.
Unlike these  machines, crypt  uses only  one rotor, with  a 256-
character alphabet and a keying sequence of period 2^32.

crypt  reads text  from standard input  and writes  the encrypted
text to standard output.  password is used to construct the model
of the machine and to  start the keying sequence.  If no password
is given,  crypt prompts for a password on  the terminal and dis-
ables echo while it is being typed in.  The password may be up to
ten characters  long, but must not be  empty; all characters past
the first ten are ignored.  All characters are legal, although it
may not  be possible  to input  certain characters from  the ter-
minal.

crypt uses the  same password for both encryption and decryption.
For example, the commands


        crypt COHERENT <file1 >file2
        crypt COHERENT <file2 >file3


leave file3 identical to file1.

Encrypted files produced by ed with  its -x option may be read by
crypt, and  vice versa, as  ed uses crypt to  perform its encryp-
tion.

Security of a cryptosystem depends on several factors:

11. Brute-force attempts to break the system should be infeasible.
   Passwords should  be at  least five characters  long; although
   the construction of  the machine model from the password takes
   a substantial fraction of a second, it is still plausible that
   encrypted files  could be  read by  a brute-force search  of a
   portion of  the password space  (say, all passwords  less than
   four characters long).

22. Cryptanalysis  of the basic  encryption scheme should  be very
   hard.   Analysis of  rotor machines is  understood, but  it is
   difficult and in most cases probably not worth the trouble.

33. Passwords must be  kept secret.  crypt erases password as soon
   as it  can, to avoid  the possibility that it  could appear in
   the output of ps.

44.  Privileged  access  to the  system  must  be guarded.   Under
   COHERENT,  the security  of crypt  can be  no better  than the
   security governing access to superuser status, because the su-


COHERENT Lexicon                                           Page 1




crypt                        Command                        crypt



   peruser can do practically anything.  This is probably crypt's
   most vulnerable point.

***** Files *****

/ddeevv/ttttyy -- Typed passwords

***** See Also *****

commands
Kahn D: _T_h_e _C_o_d_e _B_r_e_a_k_e_r_s.  New York, Macmillan, 1967.
Morris R: The  Hagelin cipher machine (M-209).  _C_r_y_p_t_o_l_o_g_i_a, July
1978.












































COHERENT Lexicon                                           Page 2


