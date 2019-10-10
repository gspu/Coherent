\fBlpstat\fP -- MLP Spooler Command

\fBlpstat\fP [-v] [-r] [-p] [-o] [-d] [-t] [-s] [-q]
.br

\fBlpstat\fP reports the status of a user's request and various aspects of 
the MLP system.  If no switches are specified, then \fB-o\fP is assumed.

\fB-v\fP   Lists printers and the devices associated with them.
.br

\fB-r\fP   Scheduler status.
.br

\fB-p\fP   Activity or idleness of each printer.
.br

\fB-o\fP   Status of printing and pending requests.
.br

\fB-d\fP   System default destination printer.
.br

\fB-t\fP   All of the above.
.br

\fB-s\fP   A summary of the above
.br

\fB-q\fP   MLP queue status.  This is a detailed list of jobs in the queue.

Files
.br
\fB/usr/spool/mlp/controls\fP -- MLP configuration database (text)
.br
\fB/usr/spool/mlp/status\fP -- MLP status database (text)

