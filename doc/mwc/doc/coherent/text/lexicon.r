.NH "The Lexicon" 1
.PP
.II Lexicon^introduction
The rest of this manual consists of the Lexicon.
The Lexicon consists of more than 1,000 articles, each of which
describes a function or command, defines a term, or otherwise
gives you useful information.
The articles appear in alphabetical order.
.PP
Internally, the Lexicon has a \fItree structure\fR.
The \*(QLroot\*(QR entry is the one for
.BR Lexicon .
It, in turn, introduces (or ``branches to'')
three ``overview'':
.BR "Running COHERENT" ,
.BR "Administering COHERENT" ,
and
.BR "Programming COHERENT" .
Each overview article points to a group of related entries.
For example, the article
.B "Programming COHERENT"
points to the articles on the \*(CO C compiler and to articles that introduce
the library functions, macros, and header files included with \*(CO.
.PP
Each Lexicon entry cross-references other entries.
These cross-references point up the documentation tree,
to its overview article and, ultimately, to the entry for
.BR COHERENT ;
down the tree to subordinate entries; and across to
entries on related subjects.
For example, the entry for \fBgetchar()\fR cross-references \fBSTDIO\fR,
which is its overview article, plus \fBputchar()\fR and \fBgetc()\fR,
which are related entries of interest to the user.
The Lexicon is designed so that you can trace from any one entry to
any other, simply by following the chain of cross-references
up and down the documentation tree.
.PP
For more information on how to use the Lexicon and how it is organized,
see the Lexicon entry for
.BR COHERENT .
