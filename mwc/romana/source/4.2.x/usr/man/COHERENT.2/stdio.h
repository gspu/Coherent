ssttddiioo.hh -- Header File

Declarations and definitions for I/O

ssttddiioo.hh is  a header file that defines manifest  constants used in standard
I/O, prototypes  the STDIO functions,  and defines numerous  I/O macros, as
follows:

_T_y_p_e_s
FFIILLEE    Descriptor of file used by STDIO routines ssttddeerrrrStandard      error
device (by default, the screen) ssttddiinnStandard input device (by default, the
keyboard) ssttddoouutt    Standard output device (by default, the screen)

_M_a_n_i_f_e_s_t _C_o_n_s_t_a_n_t_s

BBUUFFSSIIZZ.........Default buffer size
EEOOFF............End of file
FFIILLEENNAAMMEE_MMAAXX...Maximum length of a file name
FFOOPPEENN_MMAAXX......Maximum number of of open files
LL_cctteerrmmiidd......Length of cctteerrmmiidd()
LL_ttmmppnnaamm.......Length of a temporary file name
PP_ttmmppddiirr.......Default directory for temporary files
TTMMPP_MMAAXX........Maximum number of temporary file names

_F_u_n_c_t_i_o_n_s _a_n_d _M_a_c_r_o_s

cclleeaarreerrrr().....Present status stream
ffcclloossee().......Close a file stream
ffddooppeenn().......Open a file stream for I/O
ffeeooff().........Discover a file stream's status
ffeerrrroorr().......Discover a file stream's status
fffflluusshh().......Flush an output buffer
ffggeettcc()........Get a character
ffggeettppooss()......Read the file-position indicator
ffggeettss()........Get a string
ffggeettww()........Get a word
ffiilleennoo().......Get a file descriptor from a FFIILLEE structure
ffooppeenn()........Open a file stream
ffpprriinnttff()......Format and print to a file stream
ffppuuttcc()........Output a character
ffppuuttss()........Output a string
ffppuuttww()........Output a word
ffrreeaadd()........Read a file stream
ffrreeooppeenn()......Open a file stream
ffssccaannff().......Format and read from a file stream
ffsseeeekk()........Seek in a file stream
ffsseettppooss()......Set the file-position indicator
fftteellll()........Return file pointer position
ffwwrriittee().......Write to a file stream
ggeettcc().........Get a character
ggeettcchhaarr()......Get a character
ggeettss().........Get a string
ggeettww().........Get a word
ppcclloossee().......Close a pipe
ppooppeenn()........Open a pipe
pprriinnttff().......Print a formatted string
ppuuttcc().........Output a character
ppuuttcchhaarr()......Output a character
ppuuttss().........Output a string
ppuuttww().........Output a word
rreewwiinndd().......Reset a file pointer
ssccaannff()........Format and input from standard input
sseettbbuuff().......Set alternative file-stream buffer
sseettvvbbuuff()......Set alternative file-stream buffer
sspprriinnttff()......Format and print to a string
ssssccaannff().......Format and read from a string
ttmmppffiillee()......Create a temporary file
uunnggeettcc().......Return character to file stream
vvffpprriinnttff().....Format and print to a file stream
vvpprriinnttff()......Print a formatted string
vvsspprriinnttff().....Format and print to a string

_S_e_e _A_l_s_o
hheeaaddeerr ffiillee, lliibbcc, SSTTDDIIOO
ANSI Standard, section 7.9

_N_o_t_e_s
COHERENT  release 4.2  has  rewritten its  version  of ssttddiioo.hh  so that  it
conforms to the ANSI Standard.  For this reason, program that use STDIO and
are compiled  under COHERENT release 4.2 (or  subsequent releases) will not
run correctly under versions of COHERENT prior to release 4.2.
