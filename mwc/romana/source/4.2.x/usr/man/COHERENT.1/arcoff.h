aarrccooffff.hh -- Header File

COFF archive-file header
#include <coff/arcoff.h>

aarrccooffff.hh declares the structure aarr_hhddrr, which  is the header to a member of
an archive.  aarr_hhddrr is structured as follows:

struct ar_hdr
{
     char ar_name[16];        /* file member name - `/' terminated */
     char ar_date[12];        /* file member date - decimal */
     char ar_uid[6];          /* file member user id - decimal */
     char ar_gid[6];          /* file member group id - decimal */
     char ar_mode[8];         /* file member mode - octal */
     char ar_size[10];        /* file member size - decimal */
     char ar_fmag[2];         /* ARFMAG - string to end header */
};

The COFF common-archive format has the following structure:

    _______________________________________________
    |__________ARCHIVE_MAGIC_STRING_______________|
    |__________ARCHIVE_FILE_MEMBER_1______________|
    |                                             |
    |       Archive File Header "ar_hdr"          |
    |.............................................|
    |             Member Contents                 |
    |               1. External symbol directory  |
    |               2. Text file                  |
    |_____________________________________________|
    |________ARCHIVE_FILE_MEMBER_2________________|
    |                "ar_hdr"                     |
    |.............................................|
    |       Member Contents (.o or text file)     |
    |_____________________________________________|
    |       .               .               .     |
    |       .               .               .     |
    |       .               .               .     |
    |_____________________________________________|
    |________ARCHIVE_FILE_MEMBER_n________________|
    |               "ar_hdr"                      |
    |.............................................|
    |               Member Contents               |
    |_____________________________________________|

_S_e_e _A_l_s_o
aa_oouutt.hh,  aaoouutthhddrr.hh,  ffiillee  ffoorrmmaattss,  ffiilleehhddrr.hh, hheeaaddeerr  ffiilleess,  lliinneennuumm.hh,
rreelloocc.hh, ssccnnhhddrr.hh, ssttoorrccllaassss.hh, ssyymmss.hh, uuttyyppee.hh
Gircyc, G.R.: _U_n_d_e_r_s_t_a_n_d_i_n_g  _a_n_d _U_s_i_n_g _C_O_F_F.  Sebastopol, Calif, O'Reilly &
Associates, Inc., 1990.
