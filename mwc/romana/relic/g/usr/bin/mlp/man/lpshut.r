\fBlpshut\fP -- MLP Spooler Command

\fBlpshut\fP [-d]

\fBlpshut\fP stops the MLP spooler system's scheduler.  This will usually
stop any currently printing requests from completing.  If this happens, the
users who requested the reports will be notified via email.  Requests that 
were aborted will reprint from the beginning when the scheduler restarts.
Similarly, requests that were waiting to print will continue to wait until
the the scheduler resumes.

\fB-d\fP   Allow currently printing requests to finish.

The scheduler may be restarted with the \fBlpsched\fP command.

