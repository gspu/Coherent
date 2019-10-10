\fBlpadmin\fP -- MLP Spooler Command

\fBlpadmin\fP -dprinter -vdevice [-mmodel]
.br
.sp
\fBlpadmin\fP -xprinter
.br
.sp
\fBlpadmin\fP -dprinter
.br

\fBlpadmin\fP allows the system administrator to define or remove a printer and
assign the system default destination.  These things could be accomplished by
editing the MLP Controls database directly.  In fact, there are many aspects of
MLP that can be "tuned" by changing values in Controls.  \fBlpadmin\fP exists
primarily for compatibility with Unix System V.

\fB-dprinter\fP   Printer name to define or redefine.
.br

\fB-vdevice\fP    Full device name of printer (above).  Exa:  /dev/lpt1
.br

\fB-mmodel\fP     Name of a backend program or script to post process the
.br
            request.  If not given, "default" is assumed.
.br

\fB-xprinter\fP   Printer name to remove from MLP.
.br

\fB-dprinter\fP   New system default destination printer.

Files
.br
\fB/usr/spool/mlp/controls\fP -- MLP configuration database (text)
.br
\fB/usr/spool/mlp/backend\fP -- Home of backend programs and scripts

