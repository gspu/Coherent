Michelle,

Thought you might find this helpful.  Enjoy!

  # Addressing & Networks by Donnalyn Frey and Rick Adams.  In addition to
  # providing detailed information about how to use email, it tells how to
  # send mail to and from over 180 networks around the world.
  #
  # INTRODUCTION
  #
  # This file documents methods of sending mail from one network to another.
  # It represents the aggregate knowledge of the readers of comp.mail.misc
  # and many contributors elsewhere.  If you know of any corrections or
  # additions to this file, please follow the instructions in the section
  # entitled 'HOW TO FORMAT INFORMATION FOR SUBMISSION' and then mail the
  # information to me: Scott A. Yanoff <yanoff@csd4.csd.uwm.edu>.
  #
  # HOW TO USE THIS GUIDE
  #
  # If you just want to browse the guide manually for information, this is what
  # you need to know.  The guide is organized as a list of entries.  Each entry
  # tells you how to get from one network to another.  Here is what a typical
  # entry might look like:
  #
  #   #FROM: mynet
  #   #TO: yournet
  #   #RECIPIENT: youraddress
  #   #CONTACT: contactaddress
  #   #INSTR: send to 'youraddress@thegateway'
  #
  # This means that to send mail FROM a network called 'mynet' TO a
  # RECIPIENT address 'youraddress' on a network called 'yournet', you
  # should follow the INSTRUCTIONS shown and address your mail to
  # 'youraddress@thegateway'.
  #
  # Names and descriptions of the possible FROM and TO fields:
  #
  #N: aol           ; America Online; America Online, Inc.; commercial;
  #N: alternex      ; AlterNex; IBASE; non-profit (Brazil);
  #N: applelink     ; AppleLink; Apple Computer, Inc.; commercial;
  #N: arcom         ; X.400; ?; ?;
  #N: att           ; AT&T Mail; AT&T; commercial;
  #N: bitnet        ; BITNET; none; academic;
  #N: bix           ; Byte Information eXchange; Byte magazine; commercial;
  #N: bmug          ; BMUG ; Berkeley Macintosh Users Group; in-house;
  #N: calvacom      ; Calvacom;
  #- Reseau & Communication Informatique (RCI-Calvacom); commercial;
  #N: chasque       ; Chasque; Instituto del Tercer Mundo; non-profit (Uraguay)
  #N: comlink       ; ComLink; ? ; non-profit (Germany);
  #N: compuserve    ; CompuServe; CompuServe Inc.; commercial;
  #N: connect       ; Connect Professional Information Network; ?; commercial;
  #N: easylink      ; Easylink; AT&T; commercial;
  #N: easynet       ; Easynet; DEC; in-house;
  #N: econet        ; EcoNet; Institute for Global Communications; non-profit;
  #N: ecuanex       ; EcuaNex; ? ; non-profit (Ecuador);
  #N: envoy         ; Envoy-100; Telecom Canada; commercial; X.400
  #N: fax           ; Facsimile document transmission; none; none;
  #N: fidonet       ; FidoNet; none; bbs;
  #N: genie         ; GEnie; GE Information Services; commercial;
  #N: geonet        ; GeoNet Mailbox Systems;
  #- Geonet Mailbox Services GmbH/Systems Inc.; commercial;
  #N: glasnet       ; GlasNet; ? ; non-profit (Russia);
  #N: gold-400      ; GNS Gold 400; British Telecom; commercial; X.400
  #N: goldgate      ; GoldGate Telcom Gold; Net-Tel Computer Systems; ?;
  #N: greennet      ; GreenNet; Soft Solutions Ltd; commercial (England);
  #N: gsfcmail      ; GSFCmail; NASA/Goddard Space Flight Center; in-house;
  #N: ibm           ; VNET; IBM; in-house;
  #N: ibmmail       ; ?; IBM; commercial?;
  #N: internet      ; Internet; none; academic;
  #N: keylink       ; KeyLink; Telecom Australia; commercial; X.400
  #N: mailnet       ; X.400; ?; ?;
  #N: mausnet       ; Mausnet; Mausnet; non-profit;
  #N: mci           ; MCIMail; MCI; commercial;
  #N: nasamail      ; NASAMail; NASA; in-house;
  #N: nicarao       ; Nicarao; CRIES; non-profit (Nicaragua);
  #N: nordnet       ; NordNet; ? ; non-profit (Sweden);
  #N: nsi           ; NASA Science Internet; NASA; government;
  #-  Dual-protocol: instructions given here pertain only to NSI-DECnet addresses
  #-  (NSI-TCP/IP addresses should be treated as shown for 'internet')
  #N: omnet         ; OMNET; OMNET; commercial;
  #N: peacenet      ; PeaceNet/EcoNet; Institute for Global Communications;
  #- non-profit;
  #N: pegasus       ; Pegasus; Pegasus Networks; non-profit (Australia);
  #N: prodigy       ; PRODIGY; commercial; ?;
  #N: sinet         ; Schlumberger Information NETwork; ?; ?;
  #N: sprintmail    ; SprintMail; Sprint; commercial; formerly Telemail
  #N: thenet        ; Texas Higher Education Network; University of Texas;
  #- academic ;
  #N: web           ; The Web; Nirv Community Resource Centre; non-profit(Canada)
  #N: wwivnet       ; WWIVnet; WWIVnet; non-profit;

  # The 'RECIPIENT' record gives an example of an address on the
  # destination network, to make it clear in subsequent lines what text
  # requires subsitution.
  #
  # The 'CONTACT' record gives an address for inquiries concerning the
  # gateway, expressed as an address reachable from the source (#FROM:) network.
  # Presumably, if you can't get the gateway to work at all, then knowing
  # an unreachable address on another network will not be of great help.
  #
  # The 'INSTR' records, of which there may be several, give verbal
  # instructions to a user of the source network to let them send mail
  # to a user on the destination network.  Text that needs to be typed
  # will appear in double quotes, with C-style escapes if necessary.  If
  # the instructions consist simply of mailing to a certain address, this
  # will be indicated by the words 'send to' followed by a quoted address.
  # If there are alternative addresses, they will be marked 'or to' instead.
  #
  # HOW TO FORMAT INFORMATION FOR SUBMISSION
  #
  # Here is what I really want in the way of information.  If you are adding
  # a new network to the list, tell me what its official name is (pay attention
  # to capitalization), what the name of its responsible organization is, and
  # what kind of a network it is (academic, commercial, government, in-house
  # or non-profit).  If this isn't clear, look at the examples above.
  # For each connection, give me an entry that looks something like:
  #
  # #FROM: foonet
  # #TO: barnet
  # #RECIPIENT: baraddress
  # #CONTACT: contactaddress
  # #INSTR: send to 'baraddress@thegateway'
  #
  # Note that 'contactaddress' must be an address expressed in foonet's native
  # format, and not that of barnet, since if a user is having trouble accessing
  # barnet, giving him/her an address on that net to contact for help is not
  # productive.  If there is no contact/postmaster address, please tell me.
  # If there are more complicated instructions, use additional #INSTR: lines.
  #
  # Once you've got all the information together, send it to me in an e-mail
  # message with the words 'INMG update' in the Subject: line.  You can in
  # general expect an answer back from me within a week.

  #FROM: aol
  #TO: applelink
  #RECIPIENT: user
  #CONTACT: Internet
  #INSTR: send to 'user@applelink'

  #FROM: aol
  #TO: compuserve
  #RECIPIENT: 71234,567
  #CONTACT: Internet
  #INSTR: send to '71234.567@cis'

  #FROM: aol
  #TO: genie
  #RECIPIENT: user
  #CONTACT: Internet
  #INSTR: send to 'user@genie'

  #FROM: aol
  #TO: internet
  #RECIPIENT: user@domain
  #CONTACT: Internet
  #INSTR: send to 'user@domain'

  #FROM: applelink
  #TO: bitnet
  #RECIPIENT: user@site
  #INSTR: send to 'user@site.bitnet@internet#'

  #FROM: applelink
  #TO: internet
  #RECIPIENT: user@domain
  #INSTR: send to 'user@domain@internet#' (address must be < 35 characters)

  #FROM: arcom
  #TO: internet
  #RECIPIENT: fred@Domain.dd.uu.us
  #INSTR: send to C=CH,A=ARCOM,P=SWITCH,ORG=us,OU1=uu,OU2=dd,OU3=Domain,S=fred

  #FROM: att
  #TO: bitnet
  #RECIPIENT: user@site
  #INSTR: send to 'internet!site.bitnet!user'

  #FROM: att
  #TO: internet
  #RECIPIENT: user@domain
  #INSTR: send to 'internet!domain!user'

  #FROM: bitnet
  #TO: internet
  #RECIPIENT: user@domain
  #INSTR: Methods for sending mail from Bitnet to the Internet vary depending on
  #-      what mail software is running at the Bitnet site in question.  In the
  #-      best case, users should simply be able to send mail to 'user@domain'.
  #-      If this doesn't work, try 'user%domain@gateway' where 'gateway' is a
  #-      Bitnet-Internet gateway site nearby.  Finally, if neither of these
  #-      works, you may have to try hand-coding an SMTP envelope for your mail.

  #FROM: calvacom
  #TO: fax
  #RECIPIENT: (1) 41 08 11 99
  #INSTR: send to 'TLC/41081199'
  #INSTR: use standard phone numbers as calling from Paris, France.
  #FROM: calvacom
  #TO: internet
  #RECIPIENT: user@domain
  #CONTACT: postmaster@calvacom.fr
  #INSTR: send to 'EM/user@domain'
  #FROM: calvacom
  #TO: telex
  #RECIPIENT: telex_number
  #CONTACT: 634000 RCINF
  #INSTR: send to 'TLX/telex_number'
  #FROM: calvacom
  #TO: X400
  #RECIPIENT: G=John, S=Smith, O=ORG, P=PRMD, A=ADMD, C=CA
  #CONTACT: S=ADM1/A=ATLAS/P=CALVACOM/C=FR
  #INSTR: send to 'X400/G=John/S=Smith/O=ORG/P=PRMD/A=ADMD/C=CA'
  #INSTR: Calvacom professionnal account needed.
  #FROM: calvacom
  #TO: mercurylink/easylink
  #RECIPIENT: number
  #CONTACT: 19000600
  #INSTR: send to 'ML/number'
  #INSTR: Calvacom professionnal account needed.

  #FROM: compuserve
  #TO: fax
  #RECIPIENT: +1 415 555 1212
  #INSTR: send to '>FAX 14155551212'
  #INSTR: not transitive - message must originate from a CompuServe user
  #INSTR: for calls outside the NANP, use '011' as the international prefix

  #FROM: compuserve
  #TO: internet
  #RECIPIENT: user@domain
  #INSTR: send to 'INTERNET:user@domain' (only from CompuServe users)

  #FROM: compuserve
  #TO: mci
  #RECIPIENT: 123-4567
  #INSTR: send to '>MCIMAIL:123-4567' (only from CompuServe users)

  #FROM: connect
  #TO: internet
  #RECIPIENT: user@domain
  #INSTR: send to 'DASN'
  #-      and set the first line of message: '"user@domain"@DASN'

  #FROM: easylink
  #TO: calvacom
  #RECIPIENT: JS10 (John Smith)
  #INSTR: send to '19000600 /ATTN JS10 SMITH+'

  #FROM: easynet
  #TO: bitnet
  #RECIPIENT: user@site
  #CONTACT: DECWRL::ADMIN
  #INSTR: send to 'nm%DECWRL::"user@site.bitnet"' (from VMS using NMAIL)
  #INSTR: send to 'user@site.bitnet' (from Ultrix)
  #INSTR:   or to 'user%site.bitnet@decwrl.dec.com' (from Ultrix via IP)
  #INSTR:   or to 'DECWRL::"user@site.bitnet"' (from Ultrix via DECN)

  #FROM: easynet
  #TO: fidonet
  #RECIPIENT: john smith at 1:2/3.4
  #CONTACT: DECWRL::ADMIN
  #INSTR: send to 'nm%DECWRL::"john.smith@p4.f3.n2.z1.fidonet.org"'
  #-      (from VMS using NMAIL)
  #INSTR: send to 'john.smith@p4.f3.n2.z1.fidonet.org'
  #-      (from Ultrix)
  #INSTR: or to '"john.smith%p4.f3.n2.z1.fidonet.org"@decwrl.dec.com'
  #-      (from Ultrix via IP)
  #INSTR: or to 'DECWRL::"john.smith@p4.f3.n2.z1.fidonet.org"'
  #-      (from Ultrix via DECN)

  #FROM: easynet
  #TO: internet
  #RECIPIENT: user@domain
  #CONTACT: DECWRL::ADMIN
  #INSTR: send to 'nm%DECWRL::"user@domain"' (from VMS using NMAIL)
  #INSTR: send to 'user@domain' (from Ultrix)
  #INSTR:   or to 'user%domain@decwrl.dec.com' (from Ultrix via IP)
  #INSTR:   or to 'DECWRL::"user@domain"' (from Ultrix via DECN)
  #INSTR:   or to 'user@domain @Internet' (using ALL-IN-1)

  #FROM: envoy
  #TO: internet
  #RECIPIENT: user@domain
  #CONTACT: ICS.TEST or ICS.BOARD
  #INSTR: send to '[RFC-822="user(a)domain"]INTERNET/TELEMAIL/US'
  #INSTR: for special characters, use @=(a), !=(b), _=(u),
  #INSTR:          any=(three octal digits)

  #FROM: fax
  #TO: calvacom
  #RECIPIENT: JS10 (John Smith)
  #INSTR: send to '+33 1 41 08 11 99'
  #INSTR: and write on every pages 'ATTN JS10 SMITH'
  #INSTR: fax are manually processed.

  #FROM: fidonet
  #TO: internet
  #RECIPIENT: user@machine.site.domain
  #INSTR: convert to 'user@machine.site.domain ON 1:1/31

  #FROM: fidonet
  #TO: wwivnet
  #RECIPIENT: number@node
  #CONTACT: Kevin C. ON 1:100/215
  #INSTR: convert to '#number @node ON 1:100/215'
  #INSTR: WWIVgate; LOW TRAFFIC SITE, USE SPARINGLY..  Gateway is modem-based,
  #-      they absorb cost of long distance connects to pick-up and deliver.
  #-      Keep messages under 10K, use infrequently, do NOT use mail-lists or
  #-      file/list-server commands.

  #FROM: genie
  #TO: internet
  #RECIPIENT: user@domain
  #CONTACT: postmaster@genie.geis.com
  #INSTR: send to user@domain@INET#

  #FROM: geonet
  #TO: internet
  #RECIPIENT: user@domain
  #INSTR: send to 'DASN'
  #INSTR: set subject line to 'user@domain!subject'

  #FROM: gold-400
  #TO: internet
  #RECIPIENT: user@host
  #INSTR: send to '/DD.RFC-822=user(a)host/O=uknet/PRMD=uk.ac/ADMD=gold 400/C=GB/'
  #INSTR: for special characters, use @=(a), %=(p), !=(b), "=(q)

  #FROM: gsfcmail
  #TO: internet
  #RECIPIENT: user@domain
  #CONTACT: cust.svc
  #INSTR: send to '(SITE:SMTPMAIL,ID:<user(a)domain>)'
  #INSTR: or to '(C:USA,A:TELEMAIL,P:SMTPMAIL,ID:<user(a)domain>)'
  #INSTR: or send to 'POSTMAN'
  #-      and set the first line of message to 'To: user@domain'
  #INSTR: Help is also available by phoning +1 800 858 9947.

  #FROM: gsfcmail
  #TO: nsi
  #RECIPIENT: host::user
  #CONTACT: cust.svc
  #INSTR: send to '(SITE:SMTPMAIL,ID:<user(a)host.DN.NASA.GOV>)'
  #INSTR: or to '(C:USA,A:TELEMAIL,P:SMTPMAIL,ID:<user(a)host.DN.NASA.GOV>)'
  #INSTR: or send to 'POSTMAN'
  #-      and set the first line of message to 'To: user@host.DN.NASA.GOV'

  #FROM: ibmmail
  #TO: ibmmail (internet)
  #RECIPIENT: "userid@domain"
  #INSTR: imbed the following lines in messages sent from IBMMAIL and address
  #- the message to IBMMAIL(INTERNET)
  #- /INTERNET
  #- /TO userid1@domain
  #- /CC userid2@domain
  #- /REPORT
  #- /END

  #FROM: internet
  #TO: alternex
  #RECIPIENT: user
  #CONTACT: suporte@ax.apc.org
  #CONTACT: +55 (21) 286-0348
  #INSTR: send to 'user@ax.apc.org'

  #FROM: internet
  #TO: aol
  #RECIPIENT: A User
  #CONTACT: postmaster@aol.com
  #INSTR: send to auser@aol.com (all lower-case, remove spaces)
  #INSTR: msgs are truncated to 32K (8K for PCs), all characters except newline
  #-      & printable ASCII characters are mapped to spaces, users are limited to
  #-      75 pieces of Internet mail in their mailbox at a time.

  #FROM: internet
  #TO: applelink
  #RECIPIENT: user
  #INSTR: send to 'user@applelink.apple.com'

  #FROM: internet
  #TO: arcom
  #RECIPIENT: (G:John, I:Q., S:Smith, OU:ORG_UNIT, O:Org, P:PRMD, A:ADMD, C:CA)
  #INSTR: send to:
  #-  /G=John/I=Q/S=Smith/OU=ORG_UNIT/O=Org/P=PRMD/A=ADMD/C=CA@chx400.switch.ch

  #FROM: internet
  #TO: att
  #RECIPIENT: user
  #INSTR: send to 'user@attmail.com'

  #FROM: internet
  #TO: bitnet
  #RECIPIENT: user@site
  #INSTR: send to 'user%site.bitnet@gateway' where 'gateway' is a gateway host
  #-      that is on both the internet and bitnet.  Some examples of gateways
  #-      are: cunyvm.cuny.edu mitvma.mit.edu.  Check first to see what local
  #-      policies are concerning inter-network forwarding.

  #FROM: internet
  #TO: bix
  #RECIPIENT: user
  #INSTR: send to 'user@bix.com'

  #FROM: internet
  #TO: bmug
  #RECIPIENT: John Smith or John J. Smith
  #CONTACT: Email the director: Steve_Costa@bmug.org or phone (510)849-BMUG
  #INSTR: send to 'John.Smith@bmug.org'  or 'John_J_Smith@bmug.org'

  #FROM: internet
  #TO: calvacom
  #RECIPIENT: JS10 (John Smith)
  #INSTR: send to 'js10@calvacom.fr'
  #INSTR: msgs are limited to 100 000 chars each.

  #FROM: internet
  #TO: chasque
  #RECIPIENT: user
  #CONTACT: apoyo@chasque.apc.org
  #CONTACT: +598 (2) 496192
  #INSTR: send to 'user@chasque.apc.org'

  #FROM: internet
  #TO: comlink
  #RECIPIENT: user
  #CONTACT: support@oln.comlink.apc.org
  #CONTACT: +49 (511) 350-3081
  #INSTR: send to 'user@oln.comlink.apc.org'

  #FROM: internet
  #TO: compuserve
  #RECIPIENT: 71234,567
  #INSTR: send to '71234.567@CompuServe.com'
  #INSTR:   Ordinary Compuserve account IDs are pairs of octal numbers

  #FROM: internet
  #TO: compuserve
  #RECIPIENT: organization:department:user
  #INSTR: send to 'user@department.organization.compuserve.com'
  #INSTR:  This syntax is for use with members of organizations which have a
  #-       private CompuServe mail area.  'department' may not always be present.

  #FROM: internet
  #TO: connect
  #RECIPIENT: NAME
  #INSTR: send to 'NAME@connectinc.com'

  #FROM: internet
  #TO: easylink
  #RECIPIENT: user mail number 1234567
  #INSTR: send to: 1234567@eln.attmail.com

  #FROM: internet
  #TO: easynet
  #RECIPIENT: HOST::USER
  #CONTACT: admin@decwrl.dec.com
  #INSTR: send to 'user@host.enet.dec.com'
  #INSTR:   or to 'user%host.enet@decwrl.dec.com'

  #FROM: internet
  #TO: easynet
  #RECIPIENT: John Smith @ABC
  #CONTACT: admin@decwrl.dec.com
  #INSTR: send to 'John.Smith@ABC.MTS.DEC.COM'
  #INSTR:   this syntax is for sending mail to ALL-IN-1 users

  #FROM: internet
  #TO: econet
  #RECIPIENT: user
  #CONTACT: support@igc.apc.org
  #CONTACT: +1 (415) 442-0220
  #INSTR: send to 'user@igc.apc.org

  #FROM: internet
  #TO: envoy
  #RECIPIENT: John Smith (ID=userid)
  #INSTR: send to 'uunet.uu.net!att!attmail!mhs!envoy!userid'

  #FROM: internet
  #TO: ecuanex
  #RECIPIENT: user
  #CONTACT: intercom@ecuanex.apc.org
  #CONTACT: +593 (2) 505 074
  #INSTR: send to 'user@ecuanex.apc.org'

  #FROM: internet
  #TO: envoy
  #RECIPIENT: John Smith (ID=userid)
  #CONTACT: /C=CA/ADMD=TELECOM.CANADA/ID=ICS.TEST/S=TEST_GROUP/@nasamail.nasa.gov
  #INSTR: send to
  #INSTR: '/C=CA/ADMD=TELECOM.CANADA/DD.ID=userid/PN=John_Smith/@Sprint.COM'

  #FROM: internet
  #TO: fidonet
  #RECIPIENT: john smith at 1:2/3.4
  #INSTR: send to 'john.smith@p4.f3.n2.z1.fidonet.org'

  #FROM: internet
  #TO: genie
  #RECIPIENT: user
  #CONTACT: postmaster@genie.geis.com
  #INSTR: send to user@genie.geis.com

  #FROM: internet
  #TO: geonet
  #RECIPIENT: user at host
  #INSTR: send to 'user:host@map.das.net'
  #INSTR:   or to 'user@host.geomail.org' (known to work for geo2)
  #INSTR: known hosts: geo1 (Europe), geo2 (UK), geo4 (USA)

  #FROM: internet
  #TO: glasnet
  #RECIPIENT: user
  #CONTACT: support@glas.apc.org
  #CONTACT: +7 (095) 217-6182
  #INSTR: send to 'user@glas.apc.org'

  #FROM: internet
  #TO: gold-400
  #RECIPIENT: (G:John, I:Q, S:Smith, OU: org_unit, O:organization, PRMD:prmd)
  #INSTR: send to 'john.q.smith@org_unit.org.prmd.gold-400.gb'
  #INSTR:   or to
  #- '"/G=John/I=Q/S=Smith/OU=org_unit/O=org/PRMD=prmd/ADMD=gold 400/C=GB/"
  #- @mhs-relay.ac.uk'

  #FROM: internet
  #TO: goldgate
  #RECIPIENT: 10087:CQQ061
  #INSTR: send to '10087.CQQ061@goldgate.ac.uk'
  #INSTR:   or to '/G=10087/S=CQQ061/P=uk.ac/O=GoldGate/C=GB/'

  #FROM: internet
  #TO: greennet
  #RECIPIENT: user
  #CONTACT: support@gn.apc.org  or +44 (71) 608-3040
  #INSTR: user@gn.apc.org    (or user@gn.uucp if mailing from JANET)

  #FROM: internet
  #TO: gsfcmail
  #RECIPIENT: user
  #CONTACT: naic@nasa.gov
  #INSTR: send to 'user@gsfcmail.nasa.gov'
  #INSTR:   or to '/PN=user/ADMD=TELEMAIL/PRMD=GSFC/O=GSFCMAIL/C=US/
  #-        @x400.msfc.nasa.gov'

  #FROM: internet
  #TO: ibm
  #RECIPIENT: user@vmnode.tertiary_domain (syntax?)
  #CONTACT: nic@vnet.ibm.com
  #INSTR: send to 'user@vmnode.tertiary_domain.ibm.com'
  #INSTR: To look up a user's mailbox name, mail to nic@vnet.ibm.com with
  #- the line 'WHOIS name' in the message body.

  #FROM: internet
  #TO: ibmmail
  #RECIPIENT: ccsssuuu@IBMMAIL.COM
  #CONTACT: IBMMAIL(ccsssuuu)
  #INSTR: send to ccsssuuu@ibmmail.com
  #- cc=country code, sss=company site, uuu=unique number (alphanumeric base 35
  #- <36?> number) together this form the "userid" for ibmmail.

  #FROM: internet
  #TO: keylink
  #RECIPIENT: (G:John, I:Q, S:Smith, O:MyOrg, A:Telememo, C:au)
  #CONTACT: aarnet@aarnet.edu.au
  #INSTR: send to John.Q.Smith@MyOrg.telememo.au
  #INSTR: for keylink Private Mail Domains such as
  #INSTR:   (G:John, S:Smith, O:MyDept, P:AusGov, A:Telememo, C:au)
  #INSTR:   send to John.Smith@MyDept.AusGov.telememo.au

  #FROM: internet
  #TO: mausnet
  #RECIPIENT: hans schmidt @ box
  #CONTACT: sysop@k2.maus.de
  #INSTR: send to 'hans_schmidt@box.maus.de'

  #FROM: internet
  #TO: mci
  #RECIPIENT: John Smith (123-4567)
  #INSTR: send to '1234567@mcimail.com'
  #INSTR: or to 'JSmith@mcimail.com' (if 'JSmith' is unique)
  #INSTR: or to 'John_Smith@mcimail.com' (if 'John Smith' is unique - note the
  #-      underscore!)
  #INSTR: or to 'John_Smith/1234567@mcimail.com' (if 'John Smith' is NOT unique)

  #FROM: internet
  #TO: nasamail
  #RECIPIENT: user
  #CONTACT: naic@nasa.gov
  #INSTR: send to 'user@nasamail.nasa.gov'
  #INSTR: Help is available by phoning +1 205 544 1771 or +1 800 858 9947.
  #INSTR: Soon to be: user@center.nasa.gov  where "center" is where they work.

  #FROM: internet
  #TO: nicarao
  #RECIPIENT: user
  #CONTACT: support@nicarao.apc.org
  #CONTACT: +505 (2) 26228
  #INSTR: send to 'user@nicarao.apc.org'

  #FROM: internet
  #TO: nordnet
  #RECIPIENT: user
  #CONTACT: support@pns.apc.org
  #CONTACT: +46 (8) 600-0331
  #INSTR: send to 'user@pns.apc.org'

  #FROM: internet
  #TO: nsi
  #RECIPIENT: host::user
  #CONTACT: naic@nasa.gov
  #INSTR: send to 'user@host.dnet.nasa.gov'
  #INSTR:   or to 'user%host.dnet@ames.arc.nasa.gov'
  #INSTR:   or to 'user%host.dnet@east.gsfc.nasa.gov'
  #INSTR: Help is also available by phoning +1 800 858 9947.

  #FROM: internet
  #TO: omnet
  #RECIPIENT: user
  #CONTACT: naic@nasa.gov
  #INSTR: send to 'user@omnet.nasa.gov'
  #INSTR:   or to 'user/omnet@omnet.nasa.gov' (?)
  #INSTR:   or to '/DD.UN=user/O=OMN/ADMD=TELEMAIL/C=US/@Sprint.COM'
  #INSTR: Help is available by phoning +1 800 858 9947

  #FROM: internet
  #TO: peacenet
  #RECIPIENT: user
  #CONTACT: support@igc.org
  #INSTR: send to 'user@igc.apc.org'

  #FROM: internet
  #TO: pegasus
  #RECIPIENT: user
  #CONTACT: support@peg.apc.org
  #CONTACT: +61 (7) 2571111
  #INSTR: send to 'user@peg.apc.org'

  #FROM: internet
  #TO: prodigy
  #RECIPIENT: user
  #CONTACT: postmaster@prodigy.com
  #INSTR: send to 'user@prodigy.com'     For example: abcd12a@prodigy.com
  #- Please note that this service is still currently being tested!

  #FROM: internet
  #TO: sinet
  #RECIPIENT: node::user or node1::node::user
  #INSTR: send to 'user@node.SINet.SLB.COM'
  #INSTR:   or to 'user%node@node1.SINet.SLB.COM'

  #FROM: internet
  #TO: sprintmail
  #RECIPIENT: John Smith at SomeOrganization
  #INSTR: send to
  #-  '/G=John/S=Smith/O=SomeOrganization/ADMD=TELEMAIL/C=US/@Sprint.COM'
  # Help is available by phoning +1 800 827 4685

  #FROM: internet
  #TO: thenet
  #RECIPIENT: user@host
  #INSTR: send to 'user%host.decnet@utadnx.cc.utexas.edu'

  #FROM: internet
  #TO: web
  #RECIPIENT: user
  #CONTACT: spider@web.apc.org
  #CONTACT: +1 (416) 596-0212
  #INSTR: send to 'user@web.apc.org

  #FROM: internet
  #TO: wwivnet
  #RECIPIENT: number@node
  #CONTACT: faq-request@tfsquad.mn.org or bryen@tfsquad.mn.org
  #INSTR: convert to 'number-node@wwiv.tfsquad.mn.org'
  #INSTR: WWCPgate; LOW TRAFFIC SITE, USE SPARINGLY..  Gateway is modem-based,
  #-      they absorb cost of long distance connects to pick-up and deliver.
  #-      Keep messages under 10K, use infrequently, do NOT use mail-lists or
  #-      file/list-server commands.

  #FROM: keylink
  #TO: internet
  #RECIPIENT: John Smith <user@domain>
  #CONTACT: (G:CUSTOMER, S:SERVICE, O:CUST.SERVICE, P:telememo, C:au)
  #INSTR: send to '(C:au, A:telememo, P:oz.au, "RFC-822":"John Smith
  #-  <user(a)domain>")'
  #INSTR: special characters must be mapped: @->(a), %->(p), !->(b), "->(q)

  #FROM: mausnet
  #TO: internet
  #RECIPIENT: user@domain
  #CONTACT: sysop@k2
  #INSTR: send to 'user@domain'

  #FROM: mci
  #TO: compuserve
  #RECIPIENT: John Smith (71234,567)
  #CONTACT: 267-1163 (MCI Help)
  #INSTR: at the 'To:' prompt type 'John Smith (EMS)'
  #INSTR: at the 'EMS:' prompt type 'compuserve'
  #INSTR: at the 'Mbx:' prompt type '71234,567'

  #FROM: mci
  #TO: internet
  #RECIPIENT: John Smith <user@domain>
  #CONTACT: 267-1163 (MCI Help)
  #INSTR: at the 'To:' prompt type 'John Smith (EMS)'
  #INSTR: at the 'EMS:' prompt type 'INTERNET'
  #INSTR: at the 'Mbx:' prompt type 'user@domain'

  #FROM: nasamail
  #TO: internet
  #RECIPIENT: user@domain
  #CONTACT: admin
  #INSTR: send to '(site:smtpmail,id:<user(a)domain>)'
  #INSTR: Help is also available by phoning +1 205 544 1771 and at 'admin/nasa'.

  #FROM: nasamail
  #TO: nsi
  #RECIPIENT: host::user
  #CONTACT: admin
  #INSTR: send to '(site:smtpmail,id:<user(a)host.DN.NASA.GOV>)'
  #INSTR: Help is also available by phoning +1 205 544 1771 and at 'admin/nasa'.

  #FROM: nsi
  #TO: gsfcmail
  #RECIPIENT: user
  #CONTACT: naic@nasa.gov
  #INSTR: send to 'east::"user@gsfcmail.nasa.gov"'
  #INSTR: or to 'east::"/PN=user/ADMD=TELEMAIL/PRMD=GSFC/O=GSFCMAIL/C=US/
  #-      @x400.msfc.nasa.gov'
  #INSTR: Help is also available by phoning +1 800 858 9947.

  #FROM: nsi
  #TO: internet
  #RECIPIENT: user@domain
  #CONTACT: east::"naic@nasa.gov"
  #INSTR: send to 'east::"user@domain"'
  #INSTR: or to 'dftnic::"user@domain"'
  #INSTR: or to 'nssdca::in%"user@domain"'
  #INSTR: or to 'jpllsi::"user@domain"'
  #INSTR: Help is also available by phoning +1 800 858 9947.

  #FROM: nsi
  #TO: omnet
  #RECIPIENT: user
  #CONTACT: omnet.service
  #INSTR: send to 'east::"user@omnet.nasa.gov"'
  #INSTR: Help also available by phoning +1 617 244 4333 (OMN customers only)

  #FROM: nsi
  #TO: sprintmail
  #RECIPIENT: John Smith at SomeOrganization
  #CONTACT: east::"naic@nasa.gov"
  #INSTR: send to
  #- '/G=John/S=Smith/O=SomeOrganization/ADMD=TELEMAIL/C=US/@Sprint.COM'
  #INSTR: Help is also available by phoning +1 800 858 9947.

  #FROM: omnet
  #TO: internet
  #RECIPIENT: user@domain
  #CONTACT: omnet.service
  #INSTR: Enter 'compose manual' at the command prompt.  Choose the Internet
  #-      address option from the menu that appears.  Note that this gateway
  #-      service charges based on the number of 1000-character blocks sent.
  #INSTR: Help also available by phoning +1 617 244 4333 (OMN customers only).

  #FROM: sinet
  #TO: internet
  #RECIPIENT: user@domain
  #INSTR: send to 'M_MAILNOW::M_INTERNET::"user@domain"'
  #INSTR:   or to 'M_MAILNOW::M_INTERNET::domain::user'

  #FROM: sprintmail
  #TO: internet
  #RECIPIENT: user@domain
  #INSTR: send to '(C:USA,A:TELEMAIL,P:INTERNET,"RFC-822":<user(a)domain>) DEL'
  #INSTR: Help available within the United States by phoning +1 800 336 0437 and
  #-      pressing '2' on a TouchTone phone.

  #FROM: sprintmail
  #TO: nsi
  #RECIPIENT: host::user
  #INSTR: send to
  #-  '(C:USA,A:TELEMAIL,P:INTERNET,"RFC-822":<user(a)host.DNET.NASA.GOV>) DEL'
  #INSTR: Help available within the United States by phoning +1 800 336 0437 and
  #-      pressing '2' on a TouchTone phone.

  #FROM: telex
  #TO: calvacom
  #RECIPIENT: JS10 (John Smith)
  #INSTR: send to '634000 (RCINF) ; ATTN JS10 SMITH'

  #FROM: thenet
  #TO: internet
  #RECIPIENT: user@domain
  #INSTR: send to 'UTADNX::WINS%" user@domain "'

  #FROM: wwivnet
  #TO: fidonet
  #RECIPIENT: First Last ON zone:node/fnet
  #CONTACT: 1@3469
  #INSTR: convert to 'First Last ON zone:node/fnet @656'
  #INSTR: WWIVgate; LOW TRAFFIC SITE, USE SPARINGLY..  Gateway is modem-based,
  #-      they absorb cost of long distance connects to pick-up and deliver.
  #-      Keep messages under 10K, use infrequently, do NOT use mail-lists or
  #-      file/list-servers commands.

  #FROM: wwivnet
  #TO: internet
  #RECIPIENT: user@machine.site.domain
  #CONTACT: faq-request@9702 or 1@9702
  #INSTR: convert to 'user#machine.site.domain@506'
  #-      If 'user' begins with digits, begin address with a quote.
  #INSTR: WWCPgate; LOW TRAFFIC SITE, USE SPARINGLY..  Gateway is modem-based,
  #-      they absorb cost of long distance connects to pick-up and deliver.
  #-      Keep messages under 10K, use infrequently, do NOT use mail-lists or
  #-      file/list-server commands.

  #FROM: X400
  #TO: calvacom
  #RECIPIENT: JS10 (John Smith)
  #INSTR: send to 'S=JS10/A=ATLAS/P=CALVACOM/C=FR'
  --
   _/ _ ! _         @           Milwaukee, WI - A Great Place On a Great Lake
  !  _! !! ! !_  ~~  @ ~  ~~
  ! ! ! !! ! ! !~~__=||_~ ~~~ Computing Services Division - MAINFRAME CONSULTING
  ! ! ! _! ! ~~~ ~____/  ~~~  yanoff@csd4.csd.uwm.edu   yanoff@point.cs.uwm.edu


  The Great IP Guide


                 -=-=- The Research Internet (#15) -=-=-
                               19-July-1993

  The count of countries which are IP-connected now stands at 60, an
  increase of 3 over the 57 listed in table#14 on 25-June-1993:

                                FJ (Fiji)
                has been added to the list for Australasia

                            LI (Liechtenstein)
                  has been added to the list for Europe

                              GL (Greenland)
               has been added to the list for North America

  Continent       Countries <1>:                                   #
  ---------       --------------                                   -

  Africa          TN, ZA                                           2

  Antarctica      AQ <2>                                           1

  Asia            HK, IL, IN, JP, KR, KW, MY, SG, TH, TR,
                  TW                                              11

  Australasia     AU, FJ <3>, NZ                                   3

  Europe          AT, BE, BG, CH, CY, CZ, DE, DK, EE, ES,
                  FI, FR, GR, HR, HU, IE, IS, IT, LI, LU,
                  LV, NL, NO, PL, PT, RO, SE, SI, SK, SU,
                  UA <4>, UK                                      32
  North America   CA, CR, GL <5>, MX, US                           5

  South America   AR, BR, CL, EC, PR, VE                           6
                                                                  --
                                                          total=  60

                            =-=-= Notes =-=-=

  <1> IP connectivity to these 60 countries has been confirmed with ping
          plus telnet to port 25 (SMTP).

  <2> Some hosts listed in the GOV domain are physically located in AQ.

  <3> The SOA for FJ is physically located on the island of Fiji.

  <4> Some hosts listed in the UA domain, and physically located in the
          Ukrainian republic, are reachable from European IP networks.

  <5> Some hosts listed in the GOV domain are physically located in GL.

                   =-=-= Anticipated Connectivity =-=-=

  The following country appears to be likely to connect soon:

          EG (Egypt)
  --

    Donald C. Wells           Associate Scientist       dwells@nrao.edu
    National Radio Astronomy Observatory                +1-804-296-0277
    520 Edgemont Road                              Fax= +1-804-296-0278
    Charlottesville, Virginia 22903-2475 USA         78:31.1W, 38:02.2N

From uunet!mwc!alien!udo  Fri May  6 18:15:03 1994 remote from mwc
Received: by lepanto.chi.il.us (smail2.5.3-coh) id AA09668; 6 May 94 18:15:03 GMT (Fri)
Received: by mwc.com (smail2.5.3-coh) id AA23188; 6 May 94 17:26:57 GMT (Fri)
Received: from uucp3.uu.net by relay1.UU.NET with SMTP 
	(5.61/UUNET-internet-primary) id AAwosy17016; Fri, 6 May 94 13:10:16 -0400
Received: from mwc.UUCP by uucp3.uu.net with UUCP/RMAIL
        ; Fri, 6 May 1994 13:10:16 -0400
Received: by mwc.com (smail2.5.3-coh) id AA22983; 6 May 94 16:55:42 GMT (Fri)
Received: by alien.mwc.com (smail2.5.3-coh) id AA00268; 6 May 94 11:57:48 CDT (Fri)
Subject: Re: networking
To: fred@lepanto.mwc.com (Fred Butzen)
Date: Fri, 6 May 1994 11:57:44 +0100 (CDT)
From: "Udo Munk" <udo@alien.mwc.com>
Cc: louis@mwc.com, hal@mwc.com, chris@mwc.com, nigel@mwc.com, steve@mwc.com,
        rs@mwc.com, stephen@mwc.com, addison@mwc.com, jay@mwc.com
In-Reply-To: <9405041153.AA609.V1.5@lepanto.chi.il.us> from "Fred Butzen" at May 4, 94 11:53:56 am
X-Mailer: ELM [version 2.4 PL23]
Content-Type: text
Content-Length: 1347      
Message-Id: <9405061157.AA00238@alien.mwc.com>
Status: RO

> I have just spoken with Jeff Moskow concerning networking.  At present,
> the product will be configured as follows:
> 
> - The product will support the following daemons:
> 	ftp, telnet, rlogin, inet, rexec, and rsh

No routed? I want to use one of my systems as a gateway!

> - The product will support the following clients:
> 	ftp, finger, hostname, ping, rsh, rlogin, telnet, and rcp

No route? Where is ifconfig?
How does the application interface look like? Am I able to compile
gopher, WWW and such things and get them working?

> - The networking is being written using STREAMS and STREAMS cloning,
>   and (of course) uses the DDK/DKI interface to the kernel.
> 
> - It will use a sockets driver that will be linked into the kernel.
>   This will involve using yet another sockets library (hopefully,
>   the last one).

If the sockets are implemented as a device driver this for sure is not
the latest version, what we want are SV.4 compatible sockets, that
means a socket emulation sitting on top of TLI. We have to be binary
compatible to SCO too somehow to get the commercial available X/Motif
applications working. I don't think that all vendors are willing to
distribute their applications in form of object files, which are
linked on the target system then.
-- 
Udo Munk  udo@mwc.com
Mark Williams Company
Technical Development


