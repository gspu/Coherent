ccoonnttiinnuuee -- C Keyword

Force next iteration of a loop

ccoonnttiinnuuee  forces the  next iteration  of  a ffoorr,  wwhhiillee, or  ddoo loop.   For
example,

    while ((foo = getchar()) != EOF) {
        if ((foo < 'a') || (foo > 'z'))
            continue;
        ...     /* do something */
    }

forces the wwhhiillee loop to throw away everything except lower-case alphabetic
characters.

_S_e_e _A_l_s_o
CC kkeeyywwoorrddss, ffoorr, wwhhiillee
ANSI Standard, section 6.6.6.2
