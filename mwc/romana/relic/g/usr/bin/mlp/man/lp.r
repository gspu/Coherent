\fBlp\fP -- MLP Spooler Command

\fBlp\fP [-dprinter] [-ttitle] [-ncopies] [-m] [-n] [-s] [-w] files...
.br

\fBlp\fP [-dprinter] [-ttitle] [-ncopies] [-m] [-n] [-s] [-w] -R request_num 
.br
    [start_page] [end_page]

.br
\fBlp\fP -S

\fBlp\fP is the program that users and applications employ to print their
output.  This output can come in the form of files or from standard-input
to \fBlp\fP.  \fBlp\fP puts this information into the spool queue and tells
the scheduler to despool it.  \fBlp\fP can also shutdown the scheduler and
reprint queued requests.

\fB-dprinter\fP   Selects a specific destination printer for this request, 
.br
            overriding a user's or the system's default.
.br

\fB-t title\fP    Places \fBtitle\fP into the description field of the
.br
            request as it is placed in the spool queue.  By default,
.br
            this field is filled with the name of the request's file or
.br
            by a value supplied by an application through the CA interface.

\fB-ncopies\fP    Determines the number of copies to print.
.br

\fB-m\fP          Mails the user when the request has despooled.
.br

\fB-w\fP          Writes the user when the request has been despooled.
.br

\fB-s\fP          Silent--will not confirm request submissions.
.br

\fB-b\fP          Banner switch (ignored).
.br

\fB-S\fP          Shutdown scheduler.
.br

\fB-R\fP          Reprints a queued request.  Must be followed by a request
            ID (sequence number) and may be followed by a starting and
.br
            ending page number.  If the pages are omitted, then the
.br
            request is printed in its entirety.

