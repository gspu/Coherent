Hi Fred,

	I read over the manual you sent. It's great!
It came at an opportune time for me. I've been putting
a lot of time into Cohulip and I've wondered about
some of the priorities I ought to pursue. The manual
helped me get closer to a user perspective.

	I found an area where I believe I may have
propogated some confusion. It has to do with the term
'echo'. There is a telnet option for 'echo', there
is an echo server and there is an echo icmp message.

	First, I'll try to clarify the concepts, then
I'll point to some references in the manual.

	The echo server in cohulip is not the mechanism
that responds to 'ping'. In fact it doesn't deal with
ping in any way. The echo server, as its name suggests,
receives data and sends that data right back to the sender.
This service is used for testing network facilites and
application programs. On the other hand, ping generates
icmp "echo request" datagrams and the remote host that
is being 'pinged' replies with 'echo reply' icmp datagrams.
There is not 'port number' associated with icmp (unlike
tcp or udp).

	I have just completed a ping client that
runs through sockets. With icmp, all users who have
an icmp socket open receive all icmp datagrams. This means
that if I'm ping host a and someone else is pinging host
b, both of us will see each others ping reply messages.
Unfortunately, I did not understand this myself until
I ported the bsd ping client and got really familiar
with the icmp specifications as well as the echo server.

	So, (i hope these page numbers are usable pointers)
pg 21, ping doesn't go through udp, it goes to the icmp
handler which is a part of the IP module. In your diagram,
we have a wonderful picture of pinging through udp port 7
(the echo service). I don't really know how important
this is to the typical user. The nice thing about that
diagram is that clearly illustrates the high degree
of complexity that occurs in networking.

	About the only thing in cohulip that actually
_uses_ the udp code is the DNS client on udp port 53.
There isn't an echo client.. but you can telnet to port
7 and type stuff that will repeated back. 

	On page 25, you discuss 'special addresses' with
the notation that address 0 is used by applications that
don't know who they are. While this is true, the main use
of an address zero 

	An application that wishes to connect to
a remote machine, but does not know its own address
can make a function call to open tcp with a local
address of 0.0.0.0. However, the 0.0.0.0 will get
changed into the appropriate local address before
anything is emited onto the networks.

	Page 3 , 2nd paragraph contains what appears
to be a typo. The questionable phrase is:

..."but to plug set up networking on your COHERENT and
use networking to its best advantage......."

	So, in the 83 pages, there is obviously a great
deal of work that you have done. It is of very high quality,
Fred, many light years better than the ka9q dox. I'm impressed
and inspired.

---------

	In reading the manual, it became more evident that
I need to concentrate on easing the user burdens with slip.
Today, I've been adding some router features that should
enhance cohulip's abilities to handle multiple networks,
though I don't intend to make a real gateway out of it.
In particular, I'm adding code that allows each interface
to have its own IP address, broadcast address and netmask.
There are also some flags that need to be handled so that
I can port a 'rip' protocol support. This will allow cohulip
to function as a 'mulithomed' host and as a gateway in the
most common small networks. It will also enable the cohulip
owner to offer dialup slip login services (an internet
provider). 

--------

	At any rate, how do you want to go about
distributing this work? The manual is non-trivial,
and clearly looks good in hard-copy.

--------

	I've been doing some tech support on cohulip.
It is interesting (to me) that it is going into environments
I would not have thought of. There is lots of dos<->coh
networking going on... more than I would have expected.
I thoughit it was going to be mainly net-surfers trying to
snag files from far away places. Today, I helped out
a meteorologist at a university who wants to collect
doppler radar data on coherent and distribute it via
cohulip to end users. Another guy, in England, wants
to run a mail hub in a business. He'd never seen the
.gtz file extension, due to his dos background, and
didn't know exactly how to go about using ftp to get
files from raven. These things seem typical. I get
so tied up in bits and bytes that I totally forget
who it is who must use this stuff.

	Recently, I got a phone call from a guy
in Florida who had written a login program that works
with the arcnet driver. He wanted to send me a copy,
so I said great, why not email it. Well, he doesn't
have any email services.. So he sent it by snail
mail. hmmmm... I could be unaware of a whole world of
people. 

Does MWC keep any sort of profiles on 'typical' COHERENT 
users? I wonder, when I post messages to comp.os.coherent, 
what proportion of people I am _not_ reaching?


-- Randy





From mwc!gateway!wimsey.com!rwsyss!rw Mon Aug 29 01:27:26 1994
Return-Path: <mwc!gateway!wimsey.com!rwsyss!rw>
Received: from mwc by lepanto.mwc.com with uucp
	(Smail3.1.28.1 #3) id m0qf0Bi-0001CCC; Mon, 29 Aug 94 01:27 CDT
Received: by mwc.com (smail2.5.3-coh) id AA19293; 29 Aug 94 05:07:34 GMT (Mon)
Received: from rci.ripco.com by gateway.mwc.com with smtp
	(Smail3.1.28.1 #23) id m0qey8L-00026jC; Sun, 28 Aug 94 23:15 CDT
Received: from wolfe.wimsey.com by rci.ripco.com with smtp
	(Smail3.1.28.1 #9) id m0qevbG-000JosB; Sun, 28 Aug 94 20:33 CDT
Received: from rwsyss by wolfe.wimsey.com with uucp
	(Smail3.1.28.1 #9) id m0qevcr-0007XAC; Sun, 28 Aug 94 18:35 PDT
Received: by rwsys.wimsey.bc.ca (Smail3.1.28.1 #1)
	id m0qevW0-0000yTC; Sun, 28 Aug 94 18:28 PDT
Message-Id: <m0qevW0-0000yTC@rwsys.wimsey.bc.ca>
Date: Sun, 28 Aug 94 18:28 PDT
From: rw@rwsys.wimsey.bc.ca (RW)
To: fred@lepanto.mwc.com
Subject: Re: manual
Status: RO

>RW writes:
>> Hi Fred,
>[snip]
>> 	At any rate, how do you want to go about
>> distributing this work? The manual is non-trivial,
>> and clearly looks good in hard-copy.
>
>I need to clean up the points you raised above (in the snipped text), then
>build a new edition.  The $64K question is how long the current, user-level
>version of cohulip is going to be extant.  Your plans for cohulip
>obviate about two thirds of the manual.
>
hmmm... I really don't mean to become 'wishy-washy' on you
Fred, but there is a way to proceed that might be better than 
what I had planned. It would be good if Coherent users could
capitalize on what you have written.

There is the possibility that I could leave the "net>"
interface on cohulip and simply add the sockets as an "additional"
feature. This would minimize the documention obsolecence because
you have both the "net>" interface and the sockets interface 
documented. Then, the documentation process would become one of adding
documentation for new applications (such as smail) rather than 
re-writing the exiting stuff. There are new socket capabilities
as well, but they are not going to require a big amount of
documentation.

The user could have it either way; they could run net> pretty
much as it now stands, or they could invoke it as a daemon or
background task and run stuff through the sockets. This would 
be great for firewalls, because you could run 2 separate copies 
of cohulip, and you could also run all the sockets stuff, like 
smail and Xwindows over the network.

I envision 3 levels of networking, beyond uucp. First, there is a 
"personal slip" sort of facility, It is characterized by being easy 
to deal with. At the next level, there is "workstation" grade networking 
that allows a person to tie his or her computer into a LAN or WAN and 
run applications that benefit the whole group, such as news, mail 
hub or Xwindows stuff.  The third level is what I call "professional" 
grade networking. This system can server as a router, a high speed 
file or database server in a large network enviroment. It is full 
featured and complex to master.

My feeling is that cohulip 2.1 falls into the personal catagory. 
The next release that I contemplate will provide for the
workstation abilities. The proposal I submitted to Dmitry
was for a "hi-test" professional edition. That idea is that
by using lots of inexpensive Russian labor, a very fine
implementation could be produced for a tiny fraction of 
what it would normally cost. Since Dmitry is carrying the ball
on this one, I do not know what final form the proposal will 
take if it is (or was) submitted to Bob.

At any rate, what I'm getting at is that cohulip 3 can be
backward compatible with cohulip 2.1 as far as the net> is
concerned, thus combining the personal level networking with
the workstation level networking. I'd just have to leave the
net> interface on the package.

I didn't plan to pull the tcp/ip protocol stacks into the kernel
in the next release. In fact, there might be some advantages
to leaving the protocol stacks as a user process permanently.
Without going into excesses of detail, the reason to pull
cohulip into the kernel is to compete with those who claim
to be able to saturate an ethernet. Since no-one is actually
going to _want_ their ethernet filled with traffic jams, such
competition is really for bragging rights only. I can provide
all of the functionality of a kernel based tcp/ip (and more)
without moving the process into the kernel. I am also aware
of some relatively inexpensive optimizations that ought to
make Cohulip as fast as, or faster than any of the current
STREAMS on intel based tcp/ip implementations. The point is, 
that a competitive workstation level implementation can be 
obtained by having cohulip on a user level with its present 
interface, plus a sockets interface.

>With regard to distribution, that's an interesting question.  I did write
>it on company time; so even though I didn't throw a copyright page onto it,
>the manual really is Bob's property.  One arrangement which
>might be equitable is that nroff'd, screen-displayable bits could distributed
>with the package, and you could *sell* printed copies of the manual, to help
>recoup some of your costs.  I don't know what the tax situation is in Canada,
>that might be more trouble than it's worth.
>
>Please let me know what you think would be fair, and I'll discuss it with
>Bob on Monday.
>
Let me explain my thinking.

1. The next release that I make of TCP/IP will have a price tag,
though I am unsure of how it should be priced. I need to
support my efforts financially. Whether I can sell the project
lock, stock and barrel to a distributor or whether I have to
start retailing remains to be seen. I'd prefer to stay out
of retailing and concentrate on programming if I can find a
reasonable way to do that.

2. If Dmirty's proposal suceeds before I release
my present work, I will almost certainly stop working on
Cohulip. It is also possible that other contract work can
draw me away from this cohulip.3 version.

3. I am 4 days behind in my schedule. I had a target of Oct.1
for starting Beta tests on my new version. But, having gone
through supporting this initial release of cohulip, I can 
see some advantages in revising my schedule to add some of 
the most requested functionality.

4. It is important to everybody who has an interest in Coherent,
to move quickly toward an adequate TCP/IP networking facility.
My vision might be distorted by the fact that I make this observation
based on the contents of Comp.os.coherent on usenet, but there is
other evidence as well. Coherent needs a more complete tcp/ip A.S.A.P.
There seems to be some delay in MWC's effort to put TCP/IP together.


----

If you think it reasonable, run this past Bob:

1. I suggest that a low priced distribution be made by MWC of
cohulip 2.1 with a manual and on a Coherent installation floppy.
I think it would be fair if I received $2 per copy for such a
distibution, as it will probably increase my support work load.
If MWC simply gives away such a distribution or tosses it into
the kernel package at no additional cost, then it would not
be fair for me to ask for $ so long as I benefit from increased
visibility in the market.

2. A way to advance this to the next step beyond that, would be for
me to also offer cohulip 3.0 for exclusive distributorship by
MWC. This would include the sockets and device drivers for
3com 509 ethernet and western digital 80x3 style cards. The
socket system connects with cohulip to provide the tcp/ip
facilities on a multi user basis (with the security that that
brings). The socket library is compatible with Coherent's
exiting library, but is extended to provide for networking.
This would provide the functionality for a reasonably complete
implementation of tcp/ip. Even nfs can be built when the appopriate
virtual file systems code is availble in the kernel. However,
I could leave the net> interface intact to simplify learning
and managing by novices. A network distribtuion kit could then 
be made by MWC, similar to the Xwindows package without having 
to totally rewrite the documentation. It could include the
smtp capable mail facilities, a network news system (nntp,
cnews and tin), ftp, telnet, the 'r' commands such as rlogin
and rt (remote tape), rcp (remote copy).. etc. 

This could be licensed by me to MWC on a basis like this: 
$10 per copy if MWC does all the support, $15 if I support 
the MWC support team, but don't directly support customers, 
and $25 if I do the customer support. This should allow Bob 
to put out a network disk-set for less than $100 and make a 
good profit with very little risk or cash up front. This 
would involve having you perform any additional documentation 
at Bob's expense, as well as Bob doing the marketing and sales. 
Such an arrangement would allow me the incentive (and resources)
to continue to improve cohulip's marketability; Bob could
make a profit on the distribution without having to gamble
and Coherent users would have real networking. 

If an agreement to do this could be reached right away, I can 
offer a probable delivery of an alpha between Oct.15 and Nov. 1, 
and a firm date of Nov. 15.  I'd expect to see at least 2 week of 
beta testing.  Therefore, it would be available to market for 
the December Christmas Season.

I can easily provide a more detailed description if Bob seems
interested, and of course, if _you_ think this is a reasonable 
way to proceed.

--Randy




From mwc!gateway!wimsey.com!rwsyss!rw Tue Aug 30 17:26:08 1994
Return-Path: <mwc!gateway!wimsey.com!rwsyss!rw>
Received: from mwc by lepanto.mwc.com with uucp
	(Smail3.1.28.1 #3) id m0qfbd1-0001STC; Tue, 30 Aug 94 17:26 CDT
Received: by mwc.com (smail2.5.3-coh) id AA03581; 30 Aug 94 21:58:26 GMT (Tue)
Received: from wolfe.wimsey.com by gateway.mwc.com with smtp
	(Smail3.1.28.1 #23) id m0qfatC-00025MC; Tue, 30 Aug 94 16:38 CDT
Received: from rwsyss by wolfe.wimsey.com with uucp
	(Smail3.1.28.1 #9) id m0qfaut-000BwXC; Tue, 30 Aug 94 14:40 PDT
Received: by rwsys.wimsey.bc.ca (Smail3.1.28.1 #1)
	id m0qfaoy-0000ygC; Tue, 30 Aug 94 14:34 PDT
Message-Id: <m0qfaoy-0000ygC@rwsys.wimsey.bc.ca>
Date: Tue, 30 Aug 94 14:34 PDT
From: rw@rwsys.wimsey.bc.ca (RW)
To: fred@lepanto.mwc.com
Subject: Re: manual
Status: RO

>I've just gone over your long message.
>
>With regard to maintaining the current "net" interface, the only advantage
>I see is that it's a good introduction to networking.  In this context,
>its limitations are actually a strength.  However, once users get it up
>and going, they'll grow beyond it very quickly.  In particular, they'll
>want to get mail and news going - at least, I do.
>
Good point.

>My point simply is *don't let the documentation restrict your plans*.
>It's my job to make sure that the docs keep pace with your bits, not the
>other way around.
>
ACK, but I don't want to gratuitously obsolete what you've done.

>With regard to selling cohulip to MWC, I don't think it would be correct
>for me to approach Bob.  My position is compromised as I'm the MWC payroll.
>I strongly suggest that you call him yourself.  I will give you one bit of
>advice, though.  As a writer, I've dealt with publishers before.  Every
>publisher wants a royalty arrangement - and not because it benefits the
>writer.  I'll say the same thing to you that I say to every other writer,
>whether of software or of books:  get as much money as you can up front,
>and if you get royalty payments, think of them as gravy.  I will add, further,
>that one reason MWC is still alive long after other, larger software businesses
>have bitten the dust, is that it is willing to perform the drudgery of
>selling its products in the retail market.  A word to the wise is sufficient.
>
hmmm... My better half thinks I should look at retailing. Doug Peterson
wrote to ask me if it was OK to suggest to MWC that cohulip and the
ne2000 driver be tossed onto a 'bonus' disk along with all the new drivers
that have recently appeared. I'm going to beta test an update for the
ne2000 driver, so I asked him to wait. The beta test should not take
long; it's the same driver but I've add an ability to auto detect irq
and io base so that users usually won't have to fiddle with switches
or jumpers. Its a feature that can easily be overridden.

It will take even longer to get my tcp/ip to market if I retail it.
So, I'll go along with Doug's idea of releasing a bonus disk. The
sooner the better. The next release will either leave the net
interface on, and be dubbed cohulip 3.x or I'll rename it. I'm 
going to take some time to make it of a better quality one way or
another, so it is definitly reasonable to release your manual and
cohulip2.1 as a bonus disk, mwcbbs kit, raven files or similar free
methods.

>I'll work up a new manual tonight and dispatch it to you tomorrow.
>
OK.

--Randy



From mwc!gateway!nanguo.cstpl.com.au!earth Thu Sep  1 06:30:22 1994
Return-Path: <mwc!gateway!nanguo.cstpl.com.au!earth>
Received: from mwc by lepanto.mwc.com with uucp
	(Smail3.1.28.1 #3) id m0qgALW-0001CFC; Thu, 1 Sep 94 06:30 CDT
Received: by mwc.com (smail2.5.3-coh) id AA12319; 1 Sep 94 11:06:09 GMT (Thu)
Received: from bunyip.cc.uq.oz.au by gateway.mwc.com with smtp
	(Smail3.1.28.1 #23) id m0qgA4e-0001zuC; Thu, 1 Sep 94 06:12 CDT
Received: from nanguo.cstpl.com.au by bunyip.cc.uq.oz.au 
          id <29963-0@bunyip.cc.uq.oz.au>; Thu, 1 Sep 1994 18:05:17 +1000
Received: by nanguo.cstpl.com.au (Smail3.1.28.1 #27) id m0qg6vX-0000iRC;
          Thu, 1 Sep 94 17:51 AST
Message-Id: <m0qg6vX-0000iRC@nanguo.cstpl.com.au>
From: earth@nanguo.cstpl.com.au (Bob Chalmers)
Subject: cohlip scripts
To: fred@lepanto.mwc.com (Fred Butzen)
Date: Thu, 1 Sep 1994 17:51:18 +1000 (AST)
X-Mailer: ELM [version 2.4 PL22]
MIME-Version: 1.0
Content-Type: text/plain; charset=US-ASCII
Content-Transfer-Encoding: 7bit
Content-Length: 3760
Status: RO



Hi Fred,
	thoought you might be able to use this. I cant get your shell script
to work on my modem. A Hayes compat, but that means little these days, so
I thought you might like to include this as an alternative. I know its
not much different. But this works, teh other doesn't ???  figure?

=============================== cut ========================================
#!/bin/sh
RMODEM=/dev/asy01fr
LMODEM=/dev/asy01fl

case $1 in
	tafe)	TARGET=tafe;;
	raven)	TARGET=raven;;
	*)	echo "Usage: Net tafe|raven|...";
esac

# disable the port
echo "Disabling port $RMODEM"
/etc/disable $RMODEM

# invoke net for the target network
net start.$TARGET

#after typing 'exit' at the 'net>' prompt,
# reset modem and port after a net session
echo "Resetting the modem"

# get the modem's attention wiht the '+++' sequence.
# I've taken this from modeminit. It's the only way I can get this
# Hayes compatible modem to respond correctly.
{
  sleep 2
  cat >&1 <<\!
+++
!
} <$LMODEM
sleep 2
#give it at least 5 seconds, come on now.
# hang up the phone, H0 should accomplish this on most systems.
{ 
  sleep 3
  cat >&1 <<\!
ATH0
!
} <$LMODEM
sleep 2

# On my system, ATZ resets to the 'default' settings, in config area 0.
# reload the standard modem settings
{ stty 9600
  sleep 2
  cat >&1 <<\!
ATZ
!
} <$LMODEM
sleep 2

# Make sure that the /etc/enable set of scripts has permissions for users.
# re-enable the port
echo "Re enabling port $RMODEM"
/etc/enable $RMODEM

# end
# Ok, time for bed for all busy little computer people.
============================== cut ========================================


Also, because tafe has a permanently assigned ip address for me, I can use
this as start.tafe. Because i use the modem normally for UUCP as well, I 
dont even reset it, and don't need to wait for the CONNECT incoming. The
TAFE computer has slip enabled and waiting <---- not that. The port is 
already in SLIP mode on the SCO, so no interaction is necessary.
All one has to do is actually connect.

----------------------cut
hostname nanguo.cstpl.com.au
attach asy 0 /dev/asy01fl slip sl0 0 1500 9600 ATDT512692\r
ip address 203.1.96.1
route add 147.132.76.1 sl0
route add 147.132.77.3 sl0
route add default sl0
ip ttl 32
tcp mss 1048
tcp window 1048
tcp irtt 2000
eol unix
start smtp
start telunix 23
start echo
start finger
start ftp
----------------------cut
This one is a bit different, its raven. I have had to xxxx the phone number
but. I promised jay beavers that I would not divulge it. This one has a
floating address, so i have to see it, and type it in,
NOTE, the line beginning attach is ONE line
----------------------cut
hostname nanguo.cstpl.com.au
attach asy 0 /dev/asy01fl slip sl0 0 1500 9600 AT&F\r OK ATX0E1V1Q0M3DTxxxxxxxxxx\r CONNECT \r UAF-DU-04> show\sport\sslip\r UAF-DU-04> c\sslip\r session
#domain addserver 137.229.10.39
route add default sl0
ip ttl 32
tcp mss 1048
tcp window 1048
tcp irtt 2000
eol unix
start smtp
start telunix 23
start echo
start finger
start ftp
-----------------------cut

Hope thats of some help Fred. My turn :-). Could you ask your service
provider if they have a connection point in Meriden CT. please, and if they
are interested in 'joint ventures'. Failing that, is there a overall
Internet org in the US. I know, bad english..

best regards,
Bob


-- 
..............AARNet Internet Service Provider VAR..........................
|  Robert Chalmers.         ,-._|\*<-Mackay    robert@nanguo.cstpl.com.au  |
|   CSTPL (Comms).         /      \ FAX: +61 79 524032  PH: +61 79 524395  |
| International Access.    \_.-\__/  *Info,	        info@cstpl.com.au  |
|  Australian Access. 		 v   *Sales,  	        sales@cstpl.com.au |
|---MIME CAPABLE------* ao zhong you yi wan sui *--------------------------|

