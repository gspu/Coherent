. PostScript/spine.r 5/7/91
. Print notebook spine with MWC logo
. assumes Landscape mode: troff -l spine.r
.rb /u/steve/PostScript/lib/box.ps
.rb /u/steve/PostScript/lib/mwc2.ps
.ll 11i
.co
/inch { 72 mul } bind def
.1 0 .5 1.2 inch 10.5 inch .25 inch .25 inch box
.1 0 1  1.0 inch 10.3 inch .35 inch .35 inch box
0 1 .8 inch .85 inch 10 inch mwclogo2
.co
.ps 60
.ft B
.in 1i
.sp |1.1i
i386 C Compiler

