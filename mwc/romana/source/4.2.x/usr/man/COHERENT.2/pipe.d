ppiippee -- Definition

A _p_i_p_e  directs the output stream  of one program into  the input stream of
another program,  thus coupling the programs together.   With pipes, two or
more programs (or _f_i_l_t_e_r_s)  can  be  coupled  together to  perform  complex
transforms on streams of data.  For example, in the following command

    cat DATAFILE1 DATAFILE2 | sort | uniq -d

the filter  ccaatt opens two files  and prints their contents.   Its output is
piped to the filter ssoorrtt, which  sorts it.  The output of ssoorrtt is piped, in
turn, to the  filter uunniiqq, which (with the -dd  option) prints a single copy
of each  line that is duplicated  within the file.  Thus,  with this simple
set of  commands and pipes,  a user can  quickly print a list  of all lines
that appear in both files.

_S_e_e _A_l_s_o
ffiilltteerr, mmkkffiiffoo, nnaammeedd ppiippee, ppiippee(), UUssiinngg CCOOHHEERREENNTT
