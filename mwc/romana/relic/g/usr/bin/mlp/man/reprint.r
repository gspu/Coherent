.ll 75
.po 0
\fBreprint\fP -- MLP Spooler Command

\fBreprint\fP request_num [start_page] [end_page]

Previously printed requests may be reprinted with this command as with
\fBlp\fP except that \fBreprint\fP is easier to use.

   \fBrequest_num\fP   The number of the report as given by \fBlp\fP and
                     displayed by the \fBlpstat\fP command.

   \fBstart_page\fP    Start reprinting from page (default 1).

   \fBend_page\fP      Stop reprinting at page (default 9999).

If no starting and ending pages are given, then the report is reprinted in
its entirety.

The report will print one copy at the priority and on the printer to which
it was originally spooled.  These settings my be overridden by the 
\fBMLP_PRIORITY\fP and \fBMLP_COPIES\fP environment variables.  Greater
flexibility may be had by using \fBlp\fP.

NOTE: Page counting works reliably only for textual print requests.  Bitmap
images printed through MLP will probably have wildly inaccurate page counts.

Files
.br
\fB/usr/spool/mlp/queue\fP -- Spool queue

