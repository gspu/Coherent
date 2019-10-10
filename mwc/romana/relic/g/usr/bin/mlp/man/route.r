\fBroute\fP -- MLP Spooler Command

\fBroute\fP [destination_name]

This command allows users to display or change their personal default
destination printer.  A user's print request will print to this printer unless
specifically overridden in the \fBlp\fP or \fBlpr\fP commands.  If a user does
not have a personal default, the system default destination is used instead.

Invoking \fBroute\fP by itself displays the user's default printer followed by a list
of printers known to MLP.  For example:

   main  ( oki, letter, ticket, main, system, display, local )

would tell the user that their default is named "main" and the possible choices
include "oki," "letter," "ticket," and "main."

To change ones' own personal default, simply give the printer's name after 
\fBroute\fP on the command line.  Nonexistent printers will provoke an error
message.

There are three special predefined "printers" available.  These are: \fBsystem\fP,
\fBdisplay\fP, and \fBlocal\fP.  Here is what they are.

Routing to \fBsystem\fP directs the user's requests to the current system default
destination.  By routing to this "printer" the user need not be aware of how
the system's various printers are named.

Routing to \fBdisplay\fP causes the user's request to be sent to his display rather
than an actual printer. This is useful for previewing a report.

Routing to \fBlocal\fP causes the user's report to print on a slave printer connected
to the user's terminal.  The terminal must be able to support such a printer and
the printer must exist.  Also, the codes PS and PN must be defined in \fB/etc/termcap\fP
so MLP knows how to direct the data.

Files
.br
\fB/usr/spool/mlp/route/USERNAME\fP -- MLP user preferences
.br
\fB/etc/termcap\fP -- Terminal capability database

