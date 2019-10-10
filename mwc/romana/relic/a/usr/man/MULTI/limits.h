

limits.h                   Header File                   limits.h




Define numerical limits

#include <limits.h>

The header  file lliimmiittss.hh defines  macros that set  the numerical
limits  for  the translation  environment.   It  is described  in
sections 2.2.4.2 and 4.1.4 of the ANSI Standard.

The following  table gives the  macros defined in  lliimmiittss.hh. Each
value  given  is   the  macro's  minimum  maximum:  a  conforming
implementation of C must  meet these limits, and may exceed them.

CCHHAARR_BBIITT
     Number of bits in a cchhaarr. This must be at least eight.

CCHHAARR_MMAAXX
     Largest value  representable in an  object of type  cchhaarr. If
     the implementation  defines a cchhaarr to be  signed, then it is
     equal to the value  of the macro SSCCHHAARR_MMAAXX; otherwise, it is
     equal to the value of the macro UUCCHHAARR_MMAAXX.

CCHHAARR_MMIINN
     Smallest value  representable in an object  of type cchhaarr. If
     the implementation  defines a cchhaarr to be  signed, then it is
     equal to the value  of the macro SSCCHHAARR_MMIINN; otherwise, it is
     zero.

IINNTT_MMAAXX
     Largest  value representable  in an object  of type  iinntt; it
     must be at least 32,767 (0x7FFF).

IINNTT_MMIINN
     Smallest value  representable in an  object of type  iinntt; it
     must be at most -32,767 (0x8000).

LLOONNGG_MMAAXX
     Largest value  representable in an object  of type lloonngg iinntt;
     it must be at least 2,147,483,647 (0x7FFFFFFFL).

LLOONNGG_MMIINN
     Smallest value representable  in an object of type lloonngg iinntt;
     it must be at most -2,147,483,647 (0x80000000L).

MMBB_LLEENN_MMAAXX
     Largest number of  bytes in any multibyte character, for any
     locale; it must be at least one.

SSCCHHAARR_MMAAXX
     Largest  value representable  in  an object  of type  ssiiggnneedd
     cchhaarr; it must be at least 127.

SSCCHHAARR_MMIINN
     Smallest  value representable  in an  object of  type ssiiggnneedd
     cchhaarr; it must be at most -127.


COHERENT Lexicon                                           Page 1




limits.h                   Header File                   limits.h




SSHHRRTT_MMAAXX
     Largest value representable  in an object of type sshhoorrtt iinntt;
     it must be at least 32,767 ((sshhoorrtt)0x7FFF).

SSHHRRTT_MMIINN
     Smallest value representable in an object of type sshhoorrtt iinntt;
     it must be at most -32,767 ((sshhoorrtt)0x8000).

UUCCHHAARR_MMAAXX
     Largest value  representable in  an object of  type uunnssiiggnneedd
     cchhaarr; it must be at least 255.

UUIINNTT_MMAAXX
     Largest value  representable in  an object of  type uunnssiiggnneedd
     iinntt; it must be at least 65,535 ((uunnssiiggnneedd iinntt)0xFFFF).

UULLOONNGG_MMAAXX
     Largest value  representable in  an object of  type uunnssiiggnneedd
     lloonngg  iinntt;  it must  be  at  least 4,294,967,295  ((uunnssiiggnneedd
     lloonngg)0xFFFFFFFFL).

UUSSHHRRTT_MMAAXX
     Largest value  representable in  an object of  type uunnssiiggnneedd
     sshhoorrtt   iinntt;  it   must  be   at  least   65,535  ((uunnssiiggnneedd
     sshhoorrtt)0xFFFF).

***** See Also *****

hheeaaddeerr ffiilleess



























COHERENT Lexicon                                           Page 2


