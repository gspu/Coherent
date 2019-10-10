mmaann -- Command

Print Lexicon entries
mmaann [-ww] [_t_o_p_i_c ...]

mmaann prints  the COHERENT  Lexicon entries for  each specified _t_o_p_i_c  on the
standard  output.  It  uses ssccaatt  to display  text (with  the -ss  option to
suppress  blank lines).   With  no arguments,  mmaann  prints a  list of  each
available _t_o_p_i_c.

When used with  the -ww option, it prints the  path name of the file instead
of printing the document itself.

If environmental  variable PPAAGGEERR is  defined, mmaann pipes  its output through
the command specified in PPAAGGEERR. For example, the .pprrooffiillee command line:

    export PAGER=more

invokes mmoorree.

_I_n_d_e_x _F_i_l_e_s
mmaann read index files to locate the files that hold the manual page you have
requested.  It  assumes that every file /uussrr/mmaann/*.iinnddeexx  is an index file,
opens it, and searches it for the page you have requested.

Prior  to release  4.2, an  index file  consisted of  entries that  had the
format:

    _r_e_l_a_t_i_v_e-_p_a_t_h-_n_a_m_e _a_r_t_i_c_l_e-_n_a_m_e

where _r_e_l_a_t_i_v_e-_p_a_t_h-_n_a_m_e  gives the subdirectory and  file in /uussrr/mmaann that
hold the  manual-page entry, _a_r_t_i_c_l_e__n_a_m_e gives the name  of the article as
it  appears in  the Lexicon.   Beginning with release  4.2, mmaann  uses index
entries of the form:

    _r_e_l_a_t_i_v_e-_p_a_t_h-_n_a_m_e<ttaabb>_a_r_t_i_c_l_e__n_a_m_e<ttaabb>_d_e_s_c_r_i_p_t_i_o_n

_d_e_s_c_r_i_p_t_i_o_n gives a brief summary of the article.  Fields must be separated
by one <ttaabb> character.  For example, entries

     COHERENT/bc    bc        Interactive calculator with arbitrary precision
     LOCAL/chess    chess     Interactive chess program

associate manual-page file  /uussrr/mmaann/CCOOHHEERREENNTT/bbcc with the Lexicon entry for
the command  bbcc. Likewise, rules for the user-written  chess game cchheessss are
found in file /uussrr/mmaann/LLOOCCAALL/cchheessss.

mmaann can  read index entries prepared  in either the ``old''  or the ``new''
form.   We encourage  you to  use the  new form,  because this  format also
allows the index entries to be read by the command aapprrooppooss.

_A_d_d_i_n_g _M_a_n_u_a_l-_P_a_g_e _E_n_t_r_i_e_s
When writing  new manual-page entries  for COHERENT, we  recommend that you
place them  into a  subdirectory of  /uussrr/mmaann. This subdirectory  should be
uniquely named  to avoid  possible name-space  collisions.  A good  rule of
thumb is  to name the subdirectory  after the application with  which it is
associated.  This  also allows  them to be  updated easily, as  all manual-
pages   associated  with   a  given  application   reside  in   a  specific
subdirectory.

You should also add a uniquely  named index file to directory /uussrr/mmaann that
identifies each  of the newly  added manual pages.  This  index file should
use the  ``new'' format  described above;  and its name  should end  in the
string .iinnddeexx.

_F_i_l_e_s
/uussrr/mmaann/* -- Directories  that hold manual pages /uussrr/mmaann/*.iinnddeexx -- Index
files

_S_e_e _A_l_s_o
aapprrooppooss, ccoommmmaannddss, hheellpp, iinnssttaallll, PPAAGGEERR, UUssiinngg CCOOHHEERREENNTT

_N_o_t_e_s
The manual pages that are included with your release of the COHERENT system
may  include  entries  that have  been  corrected  and  updated since  your
COHERENT manual was printed.  If  there is a discrepancy between an on-line
manual page and the printed COHERENT manual, you should assume that the on-
line manual page is correct.
