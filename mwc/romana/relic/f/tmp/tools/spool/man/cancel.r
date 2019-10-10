\fBcancel\fP -- MLP Spooler Command

\fBcancel\fP request_num...

Users may cancel printing or pending requests by listing the
request number(s) after the \fBcancel\fP command.  These requests may also
be specified in the Unix System V form of:  printer_name-request_number.
Once a request
has been canceled, it may be reprinted with \fBlp\fP or \fBreprint\fP until it
is finally purged from the queue. (Note: this happens automatically, see
\fBchreq\fP for more details.)

Users are limited to canceling their own requests, while superuser can cancel
any or all of them.

Files
.br
\fB/usr/spool/mlp/queue\fP -- Spool queue

