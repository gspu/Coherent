\fBchreq\fP -- MLP Spooler Command

\fBchreq\fP -dprinter  request_num...
.br

\fBchreq\fP -llifetime request_num...
.br

\fBchreq\fP -ppriority request_num...
.br

\fBchreq\fP allows users to change the attributes of spooled requests.  The
changes my be applied to printing, waiting and quiescent requests.  The
following three attributes may be changed:  Printer, Priority and Lifetime.
More than one request may be changed at a time and the scheduler may be 
running.

Changing a request's printer is equivalent to moving the request from one
printer to another.  If the request is currently printing, it is aborted and
then moved.  Then the scheduler will deal with the job in light of its
new status.

In a like manner, a request's print priority may be changed to a higher or
lower value.  These values can range from 0 to 9 with 0 being the highest
priority.  If the request is currently printing, it will be aborted and
restarted in an order befitting its new status.

Lifetime is the final attribute that may be changed.  When a request is placed
into the spool, it is assigned a lifetime value.  These are \fBT\fPemporary,
\fBS\fPhort-term, and \fBL\fPong-term.  Queued requests with these values have
lifetimes of 1/2 hour, 24 hours and 1 week respectively.  These values may
be overridden in the MLP Controls database, so check with the system
administrator to be sure.  Requests are assigned a value of \fBT\fP by default
though this may be overridden by the environment variable \fBMLP_LIFE\fP.

Files
.br
\fB/usr/spool/mlp/queue\fP -- Spool queue
.br
\fB/usr/spool/mlp/controls\fP -- MLP configuration database

