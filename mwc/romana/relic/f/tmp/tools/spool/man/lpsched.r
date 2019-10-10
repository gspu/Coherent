\fBlpsched\fP -- MLP Spooler Command

.B lpsched

\fBlpsched\fP is the MLP spooler system scheduler.  Its job is to print
the requests that are submitted through the \fBlp\fP command.  Normally this
program is executed only once -- when COHERENT goes multiuser.  However, if
it has been halted, it may be restarted by invoking \fBlpsched\fP.  If the
scheduler is already running, any restart attempts will be ignored.

Files
.br
\fB/etc/rc\fP -- COHERENT multiuser startup script.

