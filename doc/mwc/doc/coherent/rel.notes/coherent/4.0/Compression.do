
From norm  Thu Jul  1 19:58:44 1993 remote from mwc
Received: by lepanto.mwc.com (smail2.5.3-coh) id AA03578; 1 Jul 93 19:58:44 GMT (Thu)
Received: by mwc.com (smail2.5.3-coh) id AA26667; 1 Jul 93 14:22:46 
Message-Id: <9307011422.AA26660.V3.4@mwc.com>
Date: 1 Jul 93 14:22:43
From: norm@mwc.com (Norman Bartek)
To: tech rs
Cc: fred
Subject: compressed distribution

I finished making changes to the scripts which build the COHERENT release.
The scripts now handle compression of directory trees during the time that
the distribution gets split up.  Prior to compression, the new release took
seven 3.5" disks (probably six if you pack things by hand).  With compression,
the release takes five disks, with some room left over for things to come.

I'll hack /etc/install to correctly handle these releases iff called from
build.  All the compressed archives are stored in /compressed on the floppies.
During the install, /compressed will be supressed and the extraction will
take place direcly off the floppies.  This will save both time and temporary
disk space.

  -- norm --                                    norm@mwc.com, uunet!mwc!norm
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
Quote for the day:
    "The rise of the Virtual Corporation causes the virtual employee to come
     into existence.  To form the nuclei of centers of excellence, companies
     will need people with either a broad set of skills that can be flexibly
     used or who are subject matter experts."
	       -- The Needs of the Virtual Employee, UNIXVISION, Summer 1993
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


