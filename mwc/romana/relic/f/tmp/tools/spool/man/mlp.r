\fBMLP\fP -- Spooler System

\fBMLP\fP stands for \fBM\fPultiple \fBL\fPine \fBP\fPrinter. It names a group
of programs that help users and applications access the computer's printers
in an orderly and efficient way.

Information to be printed is submitted to the \fBlp\fP program which makes a 
copy in a place called the "spool queue" and then notifies the scheduler.  The
scheduler (\fBlpsched\fP) periodically scans the queue looking for requests
that are waiting to print.  When it finds one, it assigns a printer to do the
job.  If the desired printer is busy, then the request will be reconsidered when
the device becomes available.  Requests are normally held in the queue for a
certain period of time so that they may be reprinted if the user so wishes.
Every few minutes, the scheduler purges requests that have grown too old.  The
scheduler also maintains a log of its activities and reports anything unusual
to either the system administrator or the appropriate user through the email
facility.

\fBMLP\fP replaces the spooler distributed with COHERENT 4.0.  Using the
existing spooler in conjunction with \fBMLP\fP is not recommended since they
do not know how to share the printers with each-other.

The following programs comprise the \fBMLP\fP system:

   \fBlpsched\fP  System scheduler, assigns requests to printers.
.br
   \fBlpshut\fP   Halts the scheduler's activities.
.br
   \fBlp\fP       Receives printed output from users and applications.
.br
   \fBlpr\fP      A synonym of \fBlp\fP.
.br
   \fBlpstat\fP   Reports the status of request and \fBMLP\fP.
.br
   \fBlpadmin\fP  Creates or removes printer definitions.
.br
   \fBreprint\fP  Reprints a queued request (see \fBlp\fP also).
.br
   \fBcancel\fP   Cancels one or more printing or waiting requests.
.br
   \fBchreq\fP    Changes the attributes of one or more requests.
.br
   \fBroute\fP    Changes or displays a user's personal default printer.

See manual topic \fBmlptech\fP for a technical description of \fBMLP\fP.

Files
.br
\fB/usr/spool/mlp\fP -- \fBMLP\fP's home directory
.br
\fB/usr/spool/mlp/controls\fP -- Configuration database (text)
.br
\fB/usr/spool/mlp/status\fP -- Status database (text)
.br
\fB/usr/spool/mlp/queue\fP -- Spool queue
.br
\fB/usr/spool/mlp/backend\fP -- Device interface programs (or scripts)
.br
\fB/usr/spool/mlp/route\fP -- User \fBMLP\fP preferences

