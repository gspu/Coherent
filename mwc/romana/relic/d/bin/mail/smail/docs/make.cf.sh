#! /bin/sh
#
# @(#)make.cf.sh	2.5 (smail) 9/15/87
#
cat <<!EOM!
This script will prompt you for the automatically configurable parameters
in the stock version of the sendmail configuration file.  Naturally, any
local extensions will have to be added manually.

Clyde is a VAX running AT&T System V Release 2.0, with a port of sendmail.
Clyde is a gateway for the domain .att.com.

Below is a trace of the session that
configured the sendmail.cf on clyde.ATT.COM:
===

!EOM!

echo "press return to continue"; read foo

cat <<!EOM!
Enter Date (MM-DD-YY):
06-24-86
Enter This Host's Name:
clyde
Enter This Host's Official Domain:
ATT.COM
Enter Any Equivalent Domain Classes:
ATT
Enter Any Domains For Which This Host Is An Authority:
ATT.UUCP
Does This Host Have SMTP Connections (y/n)?
no
Enter Full Path to Executable That Will Provide Local Mail Delivery:
/bin/lmail
Is /bin/lmail A Berkeley Mailer [i.e., use -r to specify sender] (y/n)?
no
Will This Host Act As A Gateway Between Domains (y/n)?
yes
Are subdomains beneath this hosts' domain to be hidden (y/n)?
yes
===
!EOM!
# get date of configuration
CF_DATE=`/bin/sh ./smail.prompt string "Enter Date (MM-DD-YY):"`

# get host name
CF_HOST=`/bin/sh ./smail.prompt string "Enter This Host's Name:"`

# get host domain
CF_DOMAIN=`/bin/sh ./smail.prompt string "Enter This Host's Official Domain:"`

# get domain classes
CF_DCLASS=`/bin/sh ./smail.prompt string "Enter Any Equivalent Domain Classes:"`

# get domain authority
CF_AUTHORITY=`/bin/sh ./smail.prompt string "Enter Any Domains For Which This Host Is An Authority:"`

CF_SMTP=`/bin/sh ./smail.prompt yesno "Does This Host Have SMTP Connections (y/n)?"`
if test "$CF_SMTP" = "yes"
then

#get list of local SMTP connections
	CF_SMTP=`/bin/sh ./smail.prompt file "Enter Full Path to File that Contains List of SMTP Connections:"`

	CF_SMTP="FE$CF_SMTP %s"
else
	CF_SMTP=""
fi

# get path to local delivery agent
CF_LOCALMAIL=`/bin/sh ./smail.prompt file "Enter Full Path to Executable That Will Provide Local Mail Delivery:"`

CF_SYSTEM=`/bin/sh ./smail.prompt yesno "Is $CF_LOCALMAIL A Berkeley Mailer [i.e., use -r to specify sender] (y/n)?"`
if test "$CF_SYSTEM" = "yes"
then
	CF_SVMAIL="#"
	CF_BSMAIL=""
else
	CF_SVMAIL=""
	CF_BSMAIL="#"
fi

CF_GATEWAY=`/bin/sh ./smail.prompt yesno "Will This Host Act As A Gateway Between Domains(y/n)?"`
if test "$CF_GATEWAY" = "yes"
then
	CF_GATEWAY=""
else
	CF_GATEWAY="#"
fi

CF_HIDDENHOSTS=`/bin/sh ./smail.prompt yesno "Are subdomains beneath this hosts' domain to be hidden (y/n)?"`
if test "$CF_HIDDENHOSTS" = "yes"
then
	CF_HIDDENHOSTS=""
else
	CF_HIDDENHOSTS="#"
fi

sed 	\
	-e "s/CF_HOST/Dw$CF_HOST/" \
	-e "s/CF_DOMAIN/DD$CF_DOMAIN/" \
	-e "s/CF_AUTHORITY/DA$CF_AUTHORITY/" \
	-e "s/CF_DCLASS/CDUUCP $CF_DCLASS/" \
	-e "s;CF_SMTP;$CF_SMTP;" \
	-e "s;CF_DATE;$CF_DATE;" \
	-e "s;CF_LOCALMAIL;$CF_LOCALMAIL;" \
	-e "s;CF_BSMAIL;$CF_BSMAIL;" \
	-e "s;CF_SVMAIL;$CF_SVMAIL;" \
	-e "s;CF_GATEWAY;$CF_GATEWAY;" \
	-e "s;CF_HIDDENHOSTS;$CF_HIDDENHOSTS;" \
	template.cf > sendmail.cf
