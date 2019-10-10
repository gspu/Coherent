ccaassee -- C Keyword

Introduce entry in switch statement

The C keyword ccaassee is a label within a sswwiittcchh statement.  For example:

    while ((int = getchar()) != EOF)
        switch (foo) {
        case 'q':
        case 'Q':
            exit(0);
        case ' ':
            n++;
        default:
            break;
        }

ccaassee  labels  each of  the  three possibilities  recognized  by the  sswwiittcchh
statement:  a space,  `q',  and `Q'.   The  statements that  follow a  ccaassee
statement behave as if they were enclosed within braces.

Note that a ccaassee statement is  simply a label: it sets a point to which the
sswwiittcchh statement  jumps, and execution  continues from that  point.  Once a
sswwiittcchh statement jumps to the point marked by a given ccaassee label, execution
continues until an eexxiitt, bbrreeaakk, or  rreettuurrnn is read, or the closing brace of
the sswwiittcchh statement is encountered.

_S_e_e _A_l_s_o
bbrreeaakk, CC kkeeyywwoorrddss, sswwiittcchh
ANSI Standard, section 6.6.4.2
