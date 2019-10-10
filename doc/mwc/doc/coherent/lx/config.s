.TH config "" "" "System Administration"
.PC "File that configures smail"
\fB/usr/lib/mail/config\fR
.PP
.II /usr/lib/mail/config
.II config
File
.B /usr/lib/mail/config
holds instructions that configure the mailer-delivery program
.BR smail .
You can modify this file to supplement, modify, or override
.BR smail 's
default configuration.
.PP
.II /usr/lib/uucp/config
Please note that this file is in no way related to file
.BR /usr/lib/uucp/config ,
which can be used to configure the Taylor \*(UU system.
For details on how to configure \*(UU,
see the Lexicon entry for
.BR /usr/lib/mail/config ,
which immediately follows this article in the Lexicon.
.PP
The rest of this article describes
.BR config ,
the attributes you can set within it, and how the setting of each
attribute affects
.BR smail 's
behavior.
.SH "Suite of Configuration Files"
To begin, your machine can have two
.B smail
configuration files:
a primary one, and a secondary one.
Either can reset the values of any
.B smail
variable; for example, each can define names for the local host,
define where files reside, or set the values for site-definable
message-header fields.
You are not obliged to use a configuration file:
if
.BR smail 's
default configuration suits you, then you can rename or move the
primary configuration file so it will no longer be read.
Likewise, if you have a primary configuration file, you are not obliged
to have a secondary one.
.PP
.B smail
reads the primary configuration file first, then the secondary configuration
file.
The values in the secondary configuration file can override those set in the
primary file; the primary file, in turn, can redefine the name of the secondary
configuration file.
This gives you great flexibility to configure
.B smail
to suit your needs and preferences.
.\".PP
.\"The design of having two such files is particularly useful in
.\"networked environments with distributed file systems.
.\"For example, on the Sun network at Amdahl Corporation,
.\"administrators name the primary configuration file
.\".BR /usr/local/lib/smail/config ,
.\"which is found on the network's file servers.
.\"This file is maintained by the group that maintains
.\"the mailers running on all of the Suns.
.\"The primary configuration file, in turn, defines a secondary
.\"configuration file named
.\".BR /private/usr/lib/smail/config .
.\"If such a file exists
.\"on a given workstation, the administration of that workstation can modify it
.\"to change
.\".BR smail 's
.\"behavior without needing different binaries.
.\"Thus, the primary configuration file provides network-wide baseline settings,
.\"with the secondary configuration configuring mail to suit the preferences of
.\"the users of a given workstation.
.\".PP
.\"Because the second configuration file can redefine the
.\"path to any other configuration file or directory, it can change any
.\"aspect of
.\".BR smail 's
.\"behavior.
.\"Thus, a gateway machine can use the same primary configuration file,
.\"director file and transport file as the other Suns, while using its
.\"own private router file.
.\"In addition, a machine on which a new delivery agent
.\"is being tested can define a private configuration
.\"file that points to its own router and transport files.
.SH "Format of a Configuration File"
A configuration file consists of instructions; each instruction, in turn,
sets an
.I attribute
to a value.
Attributes come in three flavors:
.IR string ,
.IR numeric ,
and
.IR Boolean .
To set a variable to a string or numeric value, use the form:
.DS
	\fIvariable \fB= \fIvalue\fR
.DE
.PP
For example, the instructions
.DM
	postmaster = tron@glotz.uucp
	domains = wall.com
	spool_mode = 0664
.DE
.PP
set the default address for the postmaster to
.BR tron@glotz.uucp ,
the attribute
.B domains
to
.BR wall.com ,
and the permissions for spool files to permit the file's owner and group
to write into it.
.PP
Boolean attributes are either turned off or turned on.
To turn on a Boolean attribute, use the notation:
.DS
	\fB+\fIboolean-attribute\fR
.DE
.PP
To turn it off, use the notation:
.DS
	\fB\-\fIboolean-attribute\fR
.DE
.PP
You can also use the notation
.I \-attribute
to set a numeric variable to zero and to un-set a value for a string variable.
For example, the following instructions disable the use of
an external transport file and tells
.B smail
that configuration files are not optional:
.DM
	-transport_file
	+require_configs
.DE
.PP
.B smail
ignores blanks lines within a configuration file.
If
.B smail
encounters a `#' character, it ignores that character plus all text
to its right; thus, you can use this character to introduce a comment.
.PP
If a line begins with white space,
.B smail
assumes that it continues the previous line; in this way, you can extend
an instruction over more than one line.
For example, the following instructions set the
.B Received:
header field to use for messages to a multi-line value,
and also set the name of a user that has few access capabilities:
.DM
	# Use a verbose format for the Received: header field
	received_field = "Received: by $primary_name
		with smail ($version_string)
		id <$message_id@$primary_name); $date"
.sp \n(pDu
	nobody = unknown # comment: user "unknown" has few access capabilities
.DE
.SH "smail Attributes"
The following names the attributes that you can set in a configuration file.
Each attribute's name is followed by its type and its default setting in
parentheses.
.IP "\fBauth_domains \fR(string, off)\fR"
Name the domains for which your host is considered authoritative \(em
i.e., the domains that your host knows how to access directly.
The domain names must be separated by a single colon character `:'.
Mail addressed to any domain named in this list will not be forwarded to the
smart host (described below).
.IP "\fBauto_mkdir\fR (Boolean, on)\fR"
If set,
.B smail
creates all directories required for spooling and logging if they do not exist.
However,
.B smail
will never create required parent directories.
.IP "\fBauto_mkdir_mode \fR(integer, 0755)"
When
.B smail
creates a directory, give it this permission mask.
For details on what the numbers in a permission mask mean,
see the Lexicon entry for
.BR chmod .
.IP "\fBconsole \fR(string, \fB/dev/console\fR)"
Name the console device.
This device is used as a last resort in attempting to write panic messages.
.IP "\fBcopying_file \fR(string, \fBCOPYING\fR)"
The path name to file
.BR COPYING ,
which states your distribution rights and details the warranty
information from the authors of
.BR smail .
If this does not begin with `/',
.B smail
assumes that it is in the directory named by attribute
.B smail_lib_dir 
(described below).
.IP "\fBdate_field \fR(string, \fBDate: $spool_date\fR)"
.B smail
expands this string to form field
.B Date:
in a mail message's header,
should the header not already contain such a field.
.IP "\fBdelivery_mode \fR(string, \fBforeground\fR)"
The default mode for delivering new mail.
This can be one of the following values:
.RS
.IP \fBforeground\fR
Immediate delivery via the process that received the message.
.IS \fBbackground\fR
Immediate delivery via a child process.
The process that received the message exits immediately.
.IS \fBqueued\fR
Do not attempt delivery until a later queue run.
.RE
.IP "\fBdirector_file (string, \fBdirectors\fR)"
This names the file that configures
.BR smail 's
directors.
If this does not begin with `/',
.B smail
assumes that it is in the directory named by variable
.B smail_lib_dir
(described below).
.IP "\fBdomains \fR(string)"
This sets the domain name that
.B smail
writes into the header of an outgoing mail message.
It is computed at run time.
If attribute
.B visible_name
is turned off, then
.B smail
sets it to the first name set by attribute
.BR hostnames .
If
.B hostnames
is not set, then
.B smail
constructs the domain-name host names of the form \fIhostname\fB.\fIdomain\fR.
.I hostname
is set in file
.B /etc/uucpname .
.I domain
is a name set by the attribute
.BR domains \(em
.B smail
uses each entry in
.BR domains ,
in order, to create the
.I hostnames
value.
.IP
For sites in the
.B UUCP
zone,
.B domains
often will merely be set to the string
.BR uucp .
Finally, you can use the variable
.B $visible_name
within the string to which you set this attribute.
.IP
For compatibility with earlier versions of
.BR smail ,
this attribute can also be called
.BR visible_domains .
.IP "\fBerror_copy_postmaster \fR(Boolean, off)\fR"
Send the postmaster a copy of every error message.
Normally,
.B smail
sends the postmaster only the errors that appear to result from administrator
mistakes.
If you set this attribute, then
.B smail
also sends the postmaster the errors that are returned to the sender
or that are mailed to owners of mailing lists.
.IP "\fBfnlock_interval \fR(number, 3)"
Set the sleep interval between retries while attempting
to lock mailbox files with a lockfile-based locking protocol.
Under \*(CO, the function
.B sleep()
has a one-second granularity; therefore, you must this value to at least two.
.IP "\fBfnlock_mode \fR(number, 0666)"
Create mailbox lock files.
.IP "\fBfnlock_retries \fR(number, five)"
The number of times
.B smail
attempts to lock mailbox files using a file-based locking protocol.
.IP "\fBfrom_field \fR(string)"
.B smail
expands this string to form the fields
.B From:
and
.B Sender:
in a mail message's header.
The expanded string must begin with
.BR From: ,
which may be replaced by other strings to form an actual header field.
The default value is:
.DM
	From: $sender${if def:sender_name: ($sender_name)}
.DE
.IP "\fBgrades \fR(string)"
Set the grade (or priority) characters that correspond to values of the
.B Precedence:
field in a mail message's header.
The fields within the string are separated by `:';
precedence strings alternate with grade characters.
Numbers have higher priority than upper-case letters, which in turn are higher
than lower-case letters.
Lower numbers are higher in priority than higher numbers, and the
same goes for letters lower in the alphabet.
Grades in the range `a' through `m' only return an error
message and header to the sender when an error occurs.
Grades in the range `n' through `z' return nothing to the sender should an
error occur.
The precedence names recognized by many BSD
.B sendmail
configurations are
.BR special-delivery ,
.BR first-class ,
and
.BR junk .
Others are useful mainly for getting mail out of the local machine
or for communicating with other machines that run
.B smail
in a similar configuration.
The grade character for a message is available in string expansions as the
variable
.BR $grade .
The default setting is:
.DM
	special-delivery:9:air-mail:A:first-class:C:bulk:a:junk:n
.DE
.IP "\fBhit_table_len \fR(number, 241)"
The length of the internal-address ``hit'' table.
.B smail
hashes addresses into this table to prevent
multiple deliveries to the same address.
Longer tables speed address hashing, at the price of a small increase in
the amount of memory used.
NB, this value may be ignored in the future.
.IP "\fBhost_lock_timeout \fR(numeric, 30)"
Set the time during which
.B smail
will attempt to lock a host's retry file; this file is used to guarantee
exclusive delivery to that thost.
If
.B smail
cannot lock the file within this time, then it leaves the message in the
queue, to be delivered later.
.IP
A number with no suffix indicates seconds.
Suffixes can be added to indicate a time multiplier:
.B m
indicates minutes,
.B h
indicates hours, and
.B d
indicates days.
.IP "\fBhostnames \fR(string)"
.IS "\fBhostname \fR(string)"
A colon-separated list of names for the local host.
This list, together with the attributes
.B uucp_host
and
.BR more_hostnames ,
should represent all possible names for the local host.
Note that
.B smail
does not recognize the name
.B hostname
as a name for the local host unless that name is also set by one of the other
.B hostname
variables.
If your local host is in more than one domain or can gateway to more
than one level of domains, then this attribute should represent those names.
For a host in a registered domain in the \*(UU zone,
which is also in the maps distributed over USENET,
.B localhost.uucp
should also be in the list.
The first value in
.B hostnames
is used internally as a special ``primary name'' for the local host.
.IP
Under \*(CO, this attribute is turned off by default.
.B smail
computes the value of
.B hostnames
by pairing the local host's name, as set in file
.BR /etc/uucpname ,
with every value set by attribute
.BR domains .
.B smail
re-computes the default value each time you run it.
.IP "\fBlock_by_name \fR(Boolean, on)"
If this variable is turned on, locking of the input spool file
is always based on lock files.
Otherwise, an i-node\(embased locking mechanism may be used,
such as the BSD function
.BR flock()
or
.B lockf()
under System V or \*(CO.
I-node\(embased locking is more efficient, if available.
However, lock files can be easily created by shell scripts,
which may be advantageous under some circumstances.
.IP "\fBlock_mode \fR(number, 0444)"
.IP "\fBlog_mode \fR(number, 0664)"
The mode assigned to newly created mail-system log files.
.IP "\fBlogfile \fR(string, \fB/usr/spool/smail/log/logfile\fR)"
The file into which
.B smail
writes transaction messages and error messages.
If this file does not exist,
.B smail
creates it with the mode set by variable
.BR log_mode .
.IP "\fBmax_hop_count \fR(number, 20)"
If the hop count for a message equals or exceeds this number, then
any attempt at remote delivery results in an error message
being returned to the sender.
.B smail
uses this mechanism to prevent infinite loops.
To set the hop count for a specific message, use
.BR smail 's
command-line option
.BR \-h .
Otherwise,
.B smail
computes it from the number of
.B Received:
fields in the message header.
.IP "\fBmax_load_ave \fR(number)"
For systems on which a load average can be computed, this attribute sets
the maximum load average at which mail will be delivered.
If the load average exceeds this number,
.B smail
saves incoming mail within the input spool directory for delivery later.
Under \*(CO, this attribute is not set; therefore,
.B smail
does not compute the load average, and always attempts to deliver mail.
.IP "\fBmax_message_size \fR(number, \fB100k\fR)"
Set the maximum size of a message.
.B smail
truncates messages longer than this.
(This is not yet implemented; at present, 
.B smail
sets nolimit on the size of a message.)
.IP "\fBmessage_buf_size \fR(number, \fB100k\fR)"
The size of the internal buffer that
.B smail
uses to read and write messages.
.\"For systems with a reasonable VM
.\"space, this can be equal to the maximum size for
.\"messages.
The larger the value of this buffer, the fewer the number of calls to
.B read()
are required to read the message, because the entire message is always
kept in memory.
The default value is 100 kilobytes (\fB100k\fR).
.IP "\fBmessage_id_field \fR(string)"
.B smail
expands this attribute to form the field
.B Message-Id:
in a mail message's header.
This will be used if such a field does not already exist in the header.
The default value is:
.DM
	Message-Id: <$message_id@$primary_name>
.DE
.IP "\fBmessage_log_mode \fR(number, 0644)\fR"
Each message has associated with it a unique file
that contains a transaction log for that message.
This number sets the permissions that
.B smail
gives this file when it creates it.
.IP "\fBmethod_dir \fR(string, \fBmethods\fR)"
If a method attribute for a router does not specify
a path name that begins with `/',
.B smail
prefixes this directory onto the path to form the complete path
for the method file.
If this does not begin with `/',
.B smail
assumes that it is in the directory set by attribute
.B smail_lib_dir
(described below).
See the description of the router file for more information on method files.
.IP "\fBmore_hostnames \fR(string, off)"
A colon-separated list of host names.
These host names are in addition to any names that
.B smail
computed from the domains when forming the value of the variable
.BR hostnames .
Thus, it is useful for specifying names that are not formed from the computed
name for the host.
.IP
Attribute
.B more_hostnames
can also be called
.BR gateway_names ,
because it is often used to indicate the
list of domains for which this machine is a gateway.
.IP "\fBnobody \fR(string, \fBnobody\fR)"
The default user.
This variables defines permissions to use when no other user is specified.
Also,
.B smail
uses this user in some conditions when it is not certain whether a set of
actions can be trusted, if performed under other, potentially more powerful
users.
This should reference a login identifier that has little power
to do harm or access protected files.
.IP "\fBpaniclog \fR(string, \fB/usr/spool/smail/log/paniclog\fR)"
The name of the file onto which
.B smail
appends panic messages and other important error messages.
If this file does not exist,
.B smail
creates it and assigns it the permissions specified by variable
.BR log_mode .
.B smail
records in this log all errors that require human intervention,
such as configuration errors or directory-permission errors,
that prevent mail spooling or delivery.
.IP
When a configuration error occurs,
.B smail
usually moves the mail into a special error directory
under the input spool directory.
This prevents
.B smail
from again attempting to delivery the message
until the configuration error has been corrected.
.IP
Thus, you should regularly check both the panic log and the error directory,
especially after you have changed a configuration.
When the problem has been resolved, you can move the diverted messages
back into the spool directory, and
.B smail
will again attempt to deliver them.
.IP "\fBpostmaster_address \fR(string, \fBroot\fR)"
.IS "\fBpostmaster \fR(string, \fBroot\fR)"
This attribute sets the default address of the postmaster.
If the address
.B Postmaster
is not resolved by any of the configured directors,
.B smail
then uses this address.
.IP "\fBqualify_file \fR(string, \fBqualify\fR)"
This variable names the file that contains the host-name qualification
information.
If this does not begin with `/',
.B smail
assumes that it is a subdirectory of the directory defined by the attribute
.BR smail_lib_dir .
.IP "\fBqueue_only \fR(Boolean, off)"
If this flag is set, then
.B smail
does not deliver incoming mail immediately.
It only attempts delivery when it explicitly processes the input queue,
such as when you invoke it with command-line option
.BR \-q .
.IP "\fBreceived_field \fR(string)"
.B smail
expands this string to form the field
.B Received:
in a mail message's header.
It inserts this field into the header
if the ``received'' attribute is not explicitly turned off for a transport.
The default value for
.B received_field
is:
.DM
	received_field="Received: \e
		${if def:sender_host\e
		     {from $sender_host by $primary_name\e
		      ${if def:sender_proto: with $sender_proto}\e
		      \en\et(Smail$version #$compile_num) }\e
		else {by $primary_name ${if def:sender_proto:with $sender_proto }\e
		      (Smail$version #$compile_num)\en\et}}\e
		id $message_id; $spool_date"
.DE
.IP "\fBrequire_configs \fR(Boolean, off)\fR"
If this option is turned off or is not set, then
.B smail
does not require its configuration files to exist.
This applies to the primary and secondary configuration files,
and the director, router, and transport files (respectively,
.BR /usr/lib/mail/directors ,
.BR /usr/lib/mail/routers ,
and
.BR /usr/lib/mail/transports ).
If one of these files does not exist,
.B smail
ignores it and instead uses its internally compiled configuration.
If, however, you turn on this attribute, then if
.B smail
cannot find a configuration file whose file name is not null,
it displays a panic message and exits.
.IP
To set a configuration file's name to null, turn off the attribute that
names it.
For example, to set the router file's name to null, use the
attribute
.BR \-router .
.IP "\fBretry_file \fR(string, \fBretry\fR)"
This names the file that contains the retry-control information.
If this name does not begin with `/',
.B smail
assumes that it is in directory named by variable
.B smail_lib_dir
(described below).
.IP "\fBretry_duration \fR(interval, \fB5d\fR)"
This specifies the default period of time for which
.B smail
will attempt to deliver a message.
If the message cannot be delivered within this period of time,
.B smail
assumes it is undeliverable, and sends a ``bounce'' message
either to the sender or to the list's owner, should there be one.
A number with no suffix indicates seconds.
Suffices can be added to indicate a time multiplier:
.B m
indicates minutes,
.B h
indicates hours, and
.B d
indicates days.
Under \*(CO, the default is five days.
.IP "\fBretry_interval \fR(interval, \fB10m\fR)"
If
.B smail
cannot connect to a given host, it will wait at least this amount of time
before it tries again.
This applies to all messages routed to the host in question,
to help process a queue efficiently.
.IP "\fBreturn_path_field \fR(string, \fBReturn-Path: <$sender>\fR)"
.B smail
expands this string into field
.B Return\-Path:
in the mail-message's header.
It inserts this field into the header if attribute
.B return_path
is turned on for a given transport in file
.BR /usr/lib/mail/transports .
.IP "\fBrouter_file \fR(string, \fBrouters\fR)"
This attribute names the file that contains the router-configuration
information.
If this does not begin with `/',
.B smail
assumes that it is in the directory named by attribute
.B smail_lib_dir
(described below).
.IP "\fBsecond_config_file \fR(string, none)"
This names the secondary configuration file.
The section on configuration files, above, describes how this file
relates to the primary configuration file.
If this file's name does not begin with `/',
.B smail
assumes that it is in the directory named by attribute
.B smail_lib_dir
(described below).
.IP
This is primarily useful in networks whose machines share file systems.
In particular, the attributes
.BR smart_user ,
.BR smart_path ,
and
.B smart_transport
are set in the secondary configuration file.
.IP "\fBsender_env_variable \fR(string, not set)"
This attribute names the environmental variable that, in turn,
gives the name of the mail message's sender.
Normally, the name of the sender is determined from her login identifier,
or by checking calling process's real-user identifier.
If
.B sender_env_variable
is set and the environmental variable it names exists, then
.B smail
uses that name by default.
For example, if the line
.DM
	sender_env_variable=BOGUS_NAME
.DE
.IP
appears in
.BR /usr/lib/mail/config ,
and if variable
.B BOGUS_NAME
is set in the user's environment, then
.B smail
uses that name to identify the sender, instead of the name for that user
that appears in file
.BR /etc/passwd .
.IP "\fBsmail \fR(string, \fB/bin/smail\fR)"
This attribute names the
.B smail
binary.
.B smail
uses this to re-\fBexec\fR itself when a major configuration
change has been detected, or to
.B "exec smail"
when delivering error messages.
If this name does not begin
with `/',
.B smail
assumes that this binary is kept in the directory named by attribute
.BR smail_lib_dir .
.IP "\fBsmail_lib_dir \fR(string, \fB/usr/lib/mail\fR)"
This attribute gives the full path name of the directory in which
.B smail
by default seeks its configuration files.
.IP "\fBsmail_util_dir \fR(string, \fB/usr/lib/mail\fR)"
This attribute gives the full path name of the directory that holds
.BR smail 's
utilities, in particular the utilities
.B mkaliases
and
.BR mkdbm .
.IP "\fBsmart_path \fR(string, not set)"
This attribute defines the value that the router
.B smarthost
uses by default for its
.B path
attribute.
It gives the path to a machine whose routing data base
is more complete than the one on your local host.
By default, this is not set; however, if you using \*(UU to receive
mail service from another system, you must set this variable to the
name of that system.
For details, see the Lexicon entry for
.BR routers .
.\"See a later section
.\"describing the smarthost driver for more information
.\"on the use of this attribute.
.IP "\fBsmart_transport \fR(string, not set)"
This attribute defines the value that the
.B smarthost
router driver uses by default for its attribute
.BR transport .
For details, see the Lexicon entry for
.BR routers .
.\"See a later section describing the
.\"smarthost driver for more information on the use of
.\"this attribute.
.IP "\fBsmart_user \fR(string, not set)"
This attribute defines the value that the
.B smarthost
router driver uses by default for its attribute
.BR smart_user .
For details, see the Lexicon entry for
.BR routers .
.IP "\fBsmtp_accept_max \fR(number, 20)"
This attribute sets the maximum number of SMTP connections that
.B smail
will process at any one time.
.II smtpd
This is for use with SMTP daemons started with
.BR smail 's
command-line option
.BR \-bd ,
or through the command
.BR smtpd .
If
.B smail
receives a a connection request when this number of SMTP-connection
children have already been forked,
.B smail
shuts down the connection with SMTP message 421.
If this attribute is set to zero, then the number of SMTP connections
is unlimited.
.IP "\fBsmtp_accept_queue \fR(number, 5)"
If this number of SMTP connection processes is exceeded, then
.B smail
accepts additional connections but queues their messages for later processing.
When the number of current connection processes
drops below this number,
.B smail
resumes the immediate processing of mail
(if attribute
.B delivery_mode 
is set to
.B foreground
or
.BR background .)
If
.B delivery_mode
is set to zero, then
.B smail
will always process mail immediately, regardless of the
number SMTP connections that it is handling.
Note that the value of
.B smtp_accept_queue
should be less than the value of
.BR smtp_accept_max .
Setting
.B smtp_accept_max
to zero prevents
.B smtp_accept_queue
from working correctly in all cases.
.IP "\fBsmtp_banner \fR(string)"
.B smail
expands this string to the SMTP startup banner.
.BR smail 's
SMTP server writes this banner when it accepts a connection request.
Each line of this message is automatically preceded by
identification code ``220''; newlines are correctly
changed into a carriage-return newline sequence.
The default value for
.B smtp_banner
is:
.DM
	$primary_name Smail$version #$compile_num ready at $date
.DE
.IP "\fBsmtp_debug \fR(Boolean, on)"
This Boolean variable controls the meaning of the
.B DEBUG
command when receiving SMTP commands.
If this variable is on, then the
.B DEBUG
command (with an optional debugging level) sets debugging to the
specified level, or to level 1 if no level was specified.
.B smail
writes the debugging output to the SMTP connection.
.IP "\fBsmtp_receive_command_timeout \fR(interval, \fB5m\fR)"
This attribute sets the time that
.BR smail 's
SMTP daemon waits for a
.B receiver
command after it displays its prompt.
If the daemon does not receive the command within this interval,
it closes down the connection and exits.
The default is \fB5m\fR, that is, five minutes.
.IP "\fBsmtp_receive_message_timeout (interval, \fB2h\fR)"
This attribute sets the time that
.B smail 's
SMTP daemon waits for a message after it has displayed its prompt:
.DM
	354 Enter mail
.DE
.IP
If it does not receive the entire message within this interval,
it removes the message, closes the connection, and exits.
The default is \fB2h\fR, that is, two hours.
.IP "\fBspool_dirs \fR(string, \fB/usr/spool/smail\fR)"
This sets the directory or directories into which
.B smail
spools incoming mail.
If it names more than one directory, the directories must be separated by
a colon `:'.
If
.B smail
cannot write a message to the first directory
(say, due to permission problems, file\-system\-full errors, etc.),
it tries to write the message into the other directories, one after another,
until it either succeeds in writing the message or runs out of
directories to try.
Each spool directory is expected to have the following writable subdirectories:
.DS
.ta 0.5i 2.0i
	\fBinput\fR	The actual spool files
	\fBlock\fR	Temporary lock files
	\fBmsglog\fR	Temporary per-message transaction logs and audit trails
	\fBerror\fR	Messages failing from problems requiring human intervention
.DE
.IP "\fBspool_grade \fR(character, \fBC\fR)"
This attribute gives the default grade for mail messages.
It can be overridden by a
.B Precedence:
field in a message's header.
.B smail
uses the grade to sort messages in the input spool directory.
The grade is also available in string expansions as the variable
.BR $grade .
See the description of the attribute
.BR grades ,
above, for more information.
.IP "\fBspool_mode \fR(number, \fB0440\fR)"
This attribute sets the permissions
.B smail
gives to spooled files.
.IP "\fBtransport_file \fR(string, \fBtransports\fR)"
This attribute names the file that holds the transport-configuration
information.
If the directory does not begin with `/',
.B smail
assumes it is in the directory named by the attribute
.B smail_lib_dir
(described above).
.IP "\fBtrusted_users \fR(string, off)"
This names the users who are trusted to specify a sender for a message.
Users who are not in this list cannot specify a
.B Sender:
field in a mail header;
if they do,
.B smail
removes it.
If a trusted user specifies a
.B From:
header field, then
.B smail
also creates a
.B Sender:
field that names the real user who submitted the message.
.IP
In general, this attribute should name every user
under whom remote mail is received and sent to
.BR smail .
If this list is turned off, using the form
.BR -trusted ,
then every user is trusted.
.IP
NB,
.B smail
uses the real user identifier to verify a trusted user.
However, the program
.B uucico
runs under the real user identifier of the user who invoked it \(em and
any user can invoke
.BR uucico .
.B smail
cannot distinguish this case from any other, and thus will
do the ``wrong thing'' in this instance.
Under \*(CO, this attribute is turned off by default to avoid this problem.
.IP "\fBtrusted_groups \fR(string, off)"
This attribute names the user groups that are trusted to specify a sender
of a message.
.B smail
checks a user's effective group identifier to ensure that he really is
a member of a trusted group.
Thus, were
.B smail
a
.B setgid
program, then this string would be of no value and should be turned off.
However, if
.B smail
is not set gid (as it is not under \*(CO), then programs that invoke
.B smail
under a specific effective gid, not a specific real uid,
can be detected and can be properly treated as trusted.
.IP "\fBuucp_name \fR(string)"
This attribute gives the name of your local host.
It is computed at run-time.
This name is available in string expansions as the variable
.B $uucp_name .
.B smail
also uses it in the
``remote from \fIhostname\^\fR'' suffix to ``From''
lines for mail being delivered to remote machines, when the
.B from
attribute is turned on for a transport.
.IP "\fBvisible_domains \fR(string)"
This is a synonym for attribute
.BR domains .
.SH "See Also"
.Xr "Administering COHERENT," administe
.Xr "directors," directors
.Xr "mail [overview]," mail.o
.Xr "routers," routers
.Xr "smail," smail
.Xr "transports" transport
.SH Notes
Copyright \(co 1987, 1988 Ronald S. Karr and Landon Curt Noll.
Copyright \(co 1992 Ronald S. Karr.
.PP
For details on the distribution rights and restrictions
associated with this software, see file
.BR COPYING ,
which is included with the source code to the
.B smail
system; or type the command:
.BR "smail \-bc" .
