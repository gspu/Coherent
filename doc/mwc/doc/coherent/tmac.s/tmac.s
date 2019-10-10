.\"	tmac.s:  Mark Williams implementation of standard "ms" macro package
.\"
.\"	Copyright (c) 1981, 1991 by Mark Williams Company, Northbrook, Illinois.
.\"	This file and its contents may not be reproduced with written permission
.\"	of Mark Williams Company.
.\"
.\"	History:
.\"			09/15/1991	fwb	Comments and bug fixes
.\"
.\"	External (User-settable) Number Registers:
.\"		FL	Footnote length
.\"		FM	Footer margin
.\"		HM	Header margin
.\"		LL	Line length
.\"		LT	Title length
.\"		PD	Paragraph spacing (space between paragraphs)
.\"		PI	Paragraph indentation
.\"		PO	Page offset
.\"		PS	Point size
.\"		QI	Quotation indentation
.\"		VS	Vertical spacing
.\"
.\"	External (User-settable) Strings:
.\"		LF	Left footer slug
.\"		LH	Left header slug
.\"		CF	Center footer slug
.\"		CH	Center header slug
.\"		RF	Right footer slug
.\"		RH	Right header slug
.\"
.\"	Internal Number Registers:
.\"		Bg	Has BG macro been called yet?
.\"		Di	Has a title diversion been created?
.\"
.\"	Internal Strings:
.\"		Ds	Today, in form "September 15, 1991"
.\"
.\" EXTERNAL (user-callable) MACROS
.\"
.de 1C		\" Begin a new page and initiate one-column format
.tm Line \\n(.c: Macro 1C not yet implemented
..
.2C		\" Enter two-column format
.tm Line \\n(.c: Macro 2C not yet implemented
..
.de AB		\" Begin abstract
.if !\\n(Di \{\
.		\" If you call "AB" after opening a diversion, it's YOUR problem
.da TX
.nr Di 1\}
.sp
.ce 0
.ps \\n(PS
.ft R
.ll -0.5i
.in +0.5i
.ce
\fIABSTRACT\fP
..
.de AE		\" End abstract
.ll +0.5i
.in -0.5i
.sp 0.5i
.ce 1000
\\*(Ds
.ce 0
.sp 0.5i
..
.de AI		\" Author's institution
.if !\\n(Di \{\
.da TX
.nr Di 1\}
.sp
.ce 1000
.ps \\n(PS
.ft R
..
.de AT		\" Print *Attached* and turn off line filling
.tm Line \\n(.c: Macro AT not yet implemented
..
.de AU		\" Author
.if !\\n(Di \{\
.da TX
.nr Di 1\}
.sp
.ce 1000
.ps \\n(PS
.ft R
..
.de B		\" Bold
.if \\n(.$>0 \\fB\\$1\\fP
.if \\n(.$=0 .ft B
..
.de B1		\" Open a display to enclose within a box
.in +\\n(Iku
.DS
.ll \\n(LLu
..
.de B2		\" Close a display to enclose within a box
.br
.di
.if \\n(dn>=\\n(.t .bp
.		\" Sorry -- boxes work only in PostScript mode
.if p .Lb (\\n(.o+\\n(IN)/10 (\\n(.p-\\n(.h)/10 (\\n(.l-\\n(IN)/10 -\\n(dn/10
.sp \\n(PDu
.nf
.Kd
.rm Kd
.ev
.in -\\n(Iku
.ft R
..
.de BD		\" Block-centered display
.nr Bd 1
.nf
..
.de BG		\" Begin:  initialize the world
.di
.nr BG 0
.ie \\n(Di .TX
.el        \\n(PDu
..
.de BT		\" Bottom title
'tl '\\*(LF'\\*(CF'\\*(RF'
..
.de BX		\" Enclose $1 within a little box
.LT
.if \\(.$=1 \\$1\h'-(\w'\\$1'+1p)'\v'2p'\L'-\\n(.v'\l'(\w'\\$1'+2p)'\L'\\n(.v'\l'-(\w'\\$1'+2p)'\v'-2p'\h'-\w'\\$1''
..
.de CD		\" Centered display
.ce 10000
..
.de CM		\" Cut mark to indicate end of troff page
.tm Line \\n(.c: Macro CM not yet implemented
..
.de CS		\" Cover sheet; TM mode only (which we don't support)
.tm Line \\n(.c: Macro CS not yet implemented
..
.de CT		\" Print "Copies To" and enter no-fill mode
.tm Line \\n(.c: Macro CT not yet implemented
..
.de DA		\" Re-build the date string; insert into center footer string
.if \\n(.$ .ds Ds \\$1 \\$2 \\$3 \\$4
.ds CF \\*(Ds
..
.de DE		\" Display End
.ce 0
.nr Xx 0u
.if \\n(Bi \{\
.nr Xx -.5i
.nr Bi 0
. \}
.if \\n(Bd \{\
.nr Bd 0
.di
.nr Xx (\\n(.lu-\\n(dlu)/2u
.in +\\n(Xxu
.da Kd
. \}
.KE
.in -\\n(Xxu
..
.de DS		\" Display start
.ie \\n(.$=0 .ds Xx LD
.el          .ds Xx \\$1D
.KS
.\\*(Xx
..
.de EM		\" Ending macro:  process left-over keep-release
.tm Line \\n(.c: Macro EM not yet implemented
..
.de EN		\" Space after equation produced by "eqn" or "neqn"
.tm Line \\n(.c: Macro EN not yet implemented
..
.de EQ		\" Precede equation produced by "eqn" or "neqn"
.tm Maco EQ not yet implemented
..
.de FE		\" Footnote end
.br
.nr Fv \\n(.v
.ev
.di
.nr Ft -\\n(dn
.if \\n(Fn=1 .nr Ft -(\\n(.v-\\n(Fv)
.ch Fo \\n(Ftu
.if (\\n(nl+1v)>(\\n(.p+\\n(Ft) .ch Fo \\n(nlu+1v
..
.de FS		\" Footnote start
.da Fn
.ev 1
.ll \\n(FLu
.if \\n+(Fn=1 .Fs
.fi
..
.de I		\" Italic font
.if \\n(.$>0 \\fI\\$1\\fP
.if \\n(.$=0 .ft I
..
.de ID		\" Indented display
.in +0.5i
.nf
.nr Bi 1
..
.de IE		\" Synonym for macro LP
.RT
.ne 5
.sp
..
.de IP		\" Indented paragraph
.LP
.if \\n(.$>1 .nr Ik \\$2n
.nr Il 1
.in +\\n(Iku
.ti -\\n(Iku
\\$1\h'\\n(Iku-\\w`\\$1`u'\\c
..
.de IZ		\" General initialization
.			\" User-accessible number registers
.nr CW 7u*(6.0i/15u)	\" Column width
.nr FL 5.5i		\" Footnote length
.nr FM 1.0i		\" Footer margin
.nr HM 1.0i		\" Header margin
.nr LL 6.0i		\" Line length
.nr LT 6.0i		\" Length of title
.ie t .nr PD (12p*3)/10	\" Paragraph spacing
.el   .nr PD \n(.V
.nr PI 5n		\" Paragraph indent
.nr PO 26i/27i		\" Page offset
.nr PS 10u		\" Default point size
.nr VS 12p		\" Vertical spacing
.			\" Internal registers
.nr Bg 1		\" Control calling of .BG macro
.nr Fn 0 1		\" Footnote count
.nr GW \n(LLu/15u	\" Gutter width
.nr Ik 5n		\" .IP default indentation
.nr Qd \n(PI		\" Quoted indentation difference
.nr Ps \n(PS		\" Internal version of default point size
.nr Ft 0-\n(FM		\" Internal version of footer margin 
.			\" Some commonly used strings
.ds CF "- \\n(PN -"	\" Center field of footer
.ds - \(mi
.ds ' \z\(aa
.ds ` \z\(ga
.ds ^ \z^
.ds ~ \z~
.ds C \z^
.ds , \z,
.			\" Miscellaneous initialization
.po 1.0i		\" Page offset
.wh 0 Hd		\" Header trap
.wh -\\n(FMu Fo		\" Footer trap
.wh -\\n(FMu Fx		\" Footnote trap
.			\" Set default tab stops
.ta 0.5i 1.0i 1.5i 2.0i 2.5i 3.0i 3.5i 4.0i 4.5i 5.0i 5.5i 6.0i 6.5i 7.0i 7.5i 8.0i 8.5i 9.0i
.			\" Build the date string
.if \n(mo=1 .ds Xx January
.if \n(mo=2 .ds Xx February
.if \n(mo=3 .ds Xx March
.if \n(mo=4 .ds Xx April
.if \n(mo=5 .ds Xx May
.if \n(mo=6 .ds Xx June
.if \n(mo=7 .ds Xx July
.if \n(mo=8 .ds Xx August
.if \n(mo=9 .ds Xx September
.if \n(mo=10 .ds Xx October
.if \n(mo=11 .ds Xx November
.if \n(mo=12 .ds Xx December
.ds Ds "\\*(Xx \\n(dy, 19\\n(yr"
..
.de KE		\" End keep
.br
.di
.if !\\n(Kf \{\
.if \\n(dn>=\\n(.t .bp
.nf
.Kd
.rm Kd
. \}
.ev
..
.de KF		\" Start floating keep
.nr Kf 1
.Ks
..
.de KS		\" Start keep
.nr Kf 0
.Ks
..
.de LB		\" Labelled paragraph; like .IP
.tm Line \\n(.c: Macro LB not yet implemented
..
.de LD		\" Left display
.nf
..
.de LG		\" Make text larger, by 2 points
.if \\n(.$>0 \s+2\\$1\s0
.if \\n(.$=0 .nr Ps \\n(Ps+2
..
.de LP		\" Left paragraph
.RT
.ne 5
.sp \\n(PDu
..
.de MC		\" Begin multi-column mode
.tm Line \\n(.c: Macro MC not yet implemented
..
.de ND		\" No Date: re-initialize date string, drop it from CF slug
.if \\n(.$ .ds Ds \\$1 \\$2 \\$3 \\$4
.ds CF ""
..
.de NH		\" Numbered heading
.RT
.sp
.ft B
.nr Xx 1
.if \\n(.$>0 .nr Xx \\$1
.if \\n(Xx<5 .nr @5 0
.if \\n(Xx<4 .nr @4 0
.if \\n(Xx<3 .nr @3 0
.if \\n(Xx<2 .nr @2 0
.if \\n(Xx<1 .nr @1 0
.if \\n(Xx=0 .nr Xx 1
.if \\n(Xx>5 .tm NH level too large
.nr @\\n(Xx +1
.ds Yy \\n(@1
.if \\n(Xx=1 .as Yy .
.if \\n(Xx>1 .as Yy .\\n(@2
.if \\n(Xx>2 .as Yy .\\n(@3
.if \\n(Xx>3 .as Yy .\\n(@4
.if \\n(Xx>4 .as Yy .\\n(@5
\\*(Yy
..
.de NL		\" Return text to normal size
.if \\n(.$>0 \s\\n(Ps\\$1\s0
.if \\n(.$=0 .nr Ps \\n(Ps
..
.de NP		\" Begin new page, reset registers
.tm Line \\n(.c: Macro NP not yet implemented
..
.de OK		\" Other keywords on TM cover sheet
.tm Line \\n(.c: Macro OK not yet implemented
..
.de PP		\" Start new paragraph
.RT
.sp \\n(PDu
.ne 5
.ti \\n(PIu
..
.de PT		\" Page title
'tl '\\*(LT'\\*(CT'\\*(RT'
..
.de QE		\" End quoted paragraph
.sp
.ft P
.in -\\n(PIu
.ll +\\n(PIu*2u
..
.de QP		\" Quoted paragraph
.PP
.nr Qn 1
..
.de QS		\" Start quoted paragraph
.sp
.ft R
.in +\\n(PIu
.ll -\\n(PIu*2u
..
.de R		\" Roman font
.if \\n(.$>0 \\fR\\$1\\fP
.if \\n(.$=0 .ft R
..
.de RE		\" End relative indent
.in -\\n(Iku
..
.de RP		\" Cover sheet and first page for released paper
.tm Line \\n(.c: Macro RP not yet implemented
..
.de RS		\" Start relative indent
.in +\\n(Iku
..
.de RT		\" Reset everything to normal state
.if \\n(Bg .BG
.ce 0
.if \\n(Di \{\
.di
.nr Di 0
.nf
.Tl\}
.fi
.ps \\n(Ps
.vs \\n(VSu
.ft R
.if \\n(Il \{\
.nr Il 0
.in -\\n(Iku\}
.if \\n(Qn \{\
.nr Qn -1
.in -\\n(Qdu
.ll +\\n(Qdu\}
..
.de SG		\" Insert signature of authors, TM only
.tm Line \\n(.c: Macro SG not yet implemented
..
.de SH		\" Sub-heading
.RT
.sp
.ft B
..
.de SM		\" Make text smaller, by 2 points
.if \\n(.$>0 \s-2\\$1\s0
.if \\n(.$=0 .nr Ps \\n(Ps-2
..
.de TA		\" Set tabs in ens
.ta \\$1n \\$2n \\$3n \\$4n \\$5n \\$6n \\$7n \\$8n \\$9n
..
.de TE		\" End table
.tm Line \\n(.c: Macro TE not yet implemented
..
.de TH		\" End heading section of table
.tm Line \\n(.c: Macro TH not yet implemented
..
.de TL		\" Main title
.nr Di 1
.di TX
.sp \\n(PDu
.ce 1000
.ps \\n(PS+2
.ft B
..
.de TS		\" Begin table
.tm Line \\n(.c: Macro TS not yet implemented
..
.de UL		\" Underline a bunch of text
\\$1\l'|0\(ul'
..
.de US		\" Print "UNIX" and ownership footnote on first use
.tm Line \\n(.c: Sorry, COHERENT isn't UNIX!
..
.de UX		\" Print "UNIX" and ownership footnote on first use
.tm Line \\n(.c: Sorry, COHERENT isn't UNIX!
..
.\"
.\" INTERNAL MACROS
.\"
.de Fs		\" Put out separator
\l'1i'
.br
..
.de Fo		\" Footer trap
.nr dn 0
.if \\n(Fn \{\
.ev 1
.nf
.Fn
.rm Fn
.if "\\n(.z"Fe" .di
.di
.nr Fn 0
.ev
.if \\n(dn .nr Fh 1
. \}
'sp \\n(FMu/2u
'ft R
'BT
'ft P
'bp
..
.de Fv		\" Catch footnote overflow
.nr Fv 0
.FS
.nf
.Fe
.FE
..
.de Fx		\" Divert extra stuff
.if \\n(Fn .di Fe
..
.de Hd		\" Header trap
'nr PN \\n%
'sp \\n(HMu/2u-1v
'ft R
'PT
'ft P
'sp \\n(HMu/2u
'if \\n(Kf \{\
'nr Kf 0
'ev 1
'nf
'Kd
'rm Kd
'ev
. \}
.nr Fn 0	\" Clear footnote count
.ch Fo -\\n(FMu
.if \\n(Fh .Fv	\" And put out overflow
..
.de Ks		\" Internal version of KS/KF
.br
.ev 1
.fi
.da Kd
..
.\" Draw a line box.  Here follows some more truly ugly code ...
.\" args:  $1=X position; $2=Y position; $3=box's width; $4=box's height
.de Lb
.co
1 setlinewidth
.co
.co \\$1 \\$2 
moveto
.co
.co \\$3 0 
rlineto
.co
.co 0 \\$4 
rlineto
.co
.co -\\$3 0 
rlineto
closepath
stroke
.co
..
.IZ		\" Call IZ macro to initialize ms to default values
.rm IZ		\" Blow away IZ:  we don't need it any more
