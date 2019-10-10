
From siss81.sis.rpslmc.edu!fb  Fri May  6 18:14:53 1994 remote from mwc
Received: by lepanto.chi.il.us (smail2.5.3-coh) id AA09654; 6 May 94 18:14:53 GMT (Fri)
Received: by mwc.com (smail2.5.3-coh) id AA22715; 6 May 94 16:08:01 GMT (Fri)
Received: from siss81.sis.rpslmc.edu by relay2.UU.NET with SMTP 
	(5.61/UUNET-internet-primary) id AAwosq23181; Fri, 6 May 94 11:05:29 -0400
Received: by siss81.sis.rpslmc.edu id AA20228
  (5.67a/IDA-1.5 for fb); Fri, 6 May 1994 10:05:16 -0500
Date: Fri, 6 May 1994 10:05:16 -0500
From: fb@siss81.sis.rpslmc.edu (Fred Butzen)
Message-Id: <199405061505.AA20228@siss81.sis.rpslmc.edu>
Apparently-To: fb@siss81.sis.rpslmc.edu
Status: RO



     FFFFIIIINNNNGGGGEEEERRRR((((llll))))                      PPPPTTTTXXXX                      FFFFIIIINNNNGGGGEEEERRRR((((llll))))



     NNNNAAAAMMMMEEEE
          finger - user information lookup program

     SSSSYYYYNNNNOOOOPPPPSSSSIIIISSSS
          ffffiiiinnnnggggeeeerrrr [ options ] name ...

     DDDDEEEESSSSCCCCRRRRIIIIPPPPTTTTIIIIOOOONNNN
          By default ffffiiiinnnnggggeeeerrrr lists the login name, full name, terminal
          name, and write status (producing an asterisk (*) before the
          terminal name if write permission is denied), idle time,
          login time, and office location and phone number (if known)
          for each current UNIX user.  (Idle time is minutes if a
          single integer is given, hours and minutes with a colon (:),
          or days and hours if d is given.)

          A longer format is used when a list of people's names is
          given.  (Account names as well as first and last names of
          users are accepted.)  This format is multiline, and includes
          all the information described above as well as the user's
          home directory, login shell, and login universe, any plan
          the user has placed in the file ._p_l_a_n in the home directory,
          and the project on which the user is working from the file
          ._p_r_o_j_e_c_t, also in the home directory.

     OOOOPPPPTTTTIIIIOOOONNNNSSSS
          ----mmmm   Match arguments only on user name.

          ----llll   Force long output format.

          ----pppp   Suppress printing of the ._p_l_a_n files.

          ----ssss   Force short output format.

          ----wwww   Wide output format.

          ----qqqq   Give quick list of users providing login name, tty
               name, and login time only.

          ----iiii   Give quick list with idle times.

          ----bbbb   Use default options.

          ----ffff   Suppress printing of headers on all options.

          ----hhhh   List project information from ~user/.project.

          The default options are ----bbbb, ----ffff, ----hhhh, ----mmmm, ----pppp, ----qqqq, ----wwww.

     FFFFIIIILLLLEEEESSSS
          /etc/utmp           who file
          /etc/passwd         for users names, offices, ...
          /usr/adm/lastlog    last login times



     Page 1                                          (printed 3/27/93)






     FFFFIIIINNNNGGGGEEEERRRR((((llll))))                      PPPPTTTTXXXX                      FFFFIIIINNNNGGGGEEEERRRR((((llll))))



     SSSSEEEEEEEE AAAALLLLSSSSOOOO
          who(1)

     AAAAUUUUTTTTHHHHOOOORRRR
          Earl T. Cohen; ported to PTX by Dilip Ratnam.

     BBBBUUUUGGGGSSSS
          Only the first line of the ._p_r_o_j_e_c_t file is printed.















































     Page 2                                          (printed 3/27/93)





