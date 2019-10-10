+0Introduction	1
%COHERENT^description=1
+1What Is COHERENT?	1
+2What is an Operating System?	1
+2Design Philosophy	2
%Conway, John=2
%device-independent I/O=2
+1Installation	2
+1User Registration and Reaction Report	2
%user reaction report=2
%manual^user reaction report=2
+1Technical Support	2
+2Help Us Help You	3
+1How To Use This Manual	3
%manual^how to use=3
+2Elementary Tutorials	3
+2Advanced Tutorials	4
+2The Lexicon	5
+1Where To Go From Here	5
.pn 6+1
+0Using the COHERENT System	7
+1How Do I Begin?	7
+2Logging in	7
%logging in^definition=7
%crackers=7
+2Special Terminal Keys	8
%eol=8
%enter=8
%linefeed=8
%return=8
%cont=8
%control key=8
%ctrl key=8
%stty=8
%<kill>=8
%<ctrl-H>=8
%<erase>=8
%<interrupt>=8
%rub out key=8
%del key=8
+2Try Some COHERENT Commands	8
%cat=8
%<Return>=8
%lc=9
+2Giving Commands to COHERENT	9
%command^definition=9
%commands^first part=9
%argument=9
%parameter=9
%option=9
%cat=9
%parameter^name=9
%option=9
%ls=9
%parameter^option=9
+2help, man, apropos: Help with Commands	10
%help=10
%man=10
+2Shutting Down COHERENT and Rebooting	11
%COHERENT^shutting down=11
%shutting down COHERENT=11
%COHERENT^rebooting=11
%rebooting COHERENT=11
+2Logging Out	12
%logging out=12
%<ctrl-D>=12
%exit=12
+1Working With Files and Directories	12
%file=12
%directory=12
+2File Names	12
%file^name=12
%case sensitivity^in file names=12
%lower case^in file names=12
+2Introduction to Directories	13
%directory=13
%home directory=13
%directory^home=13
+2Path Names	13
%root=13
%path name=13
%usr=13
%path name=13
%path name^fully specified=13
%slash^in path name=14
%home directory=14
%directory^home=14
grep: /v/PostScript/manual/using.eps: No such file or directory
troff: /v/doc/coherent/text/using.r: 738: .rb: cannot open file /v/PostScript/manual/using.eps
%root=14
%path name=14
%directory^root=14
%current directory=14
%directory^current=14
+2ls, lc: Listing Your Directory	14
%lc=14
%ls=14
%/bin=14
%options=15
%mode=15
+2cat: Print Contents of a File	15
%cat=15
%standard output=15
%redirection=15
%file^concatenation=15
%I/O redirection=15
%>=15
+2more: List Files on the Screen	16
%more=16
+2mkdir: Create a Directory	16
%mkdir=16
%file^creation=16
+2cd: Change Directory	16
%cd=16
+2pwd: Print Working Directory	16
%pwd=16
+2mv, cp: Move and Copy Files	17
%mv=17
%move files=17
%file^move=17
%rename files=17
%file^rename=17
%cp=17
%file^copying=17
+2rm, rmdir: Remove Files and Directories	19
%rm=19
%file^removal of=19
%rmdir=19
%directory^removing=19
%remove a directory=19
+2du, df: How Much Space?	19
%Parkinson's law=19
%du=19
%disk usage=19
%disk^block=19
%block, disk=19
%df=19
+2ln: Link Files	19
%links=19
%ln=20
%file^links=20
%rm=20
%file^links=20
+2File Permissions	20
%permission^access=20
%access permission=20
%mode field=21
+2chmod: Change File Permissions	21
%chmod=21
%permission^write=21
%write permission=21
%permission^read=21
%read permission=21
+2Creating and Mounting a File System	22
+2fdformat: Format a Floppy Disk	22
+2mkfs: Create a File System	22
%mkfs=22
%file^prototype=22
%prototype=22
+2mount: Mount a File System	23
%mount=23
+2Using a Newly Mounted File System	23
+2umount: Unmount a File System	23
%umount=23
+2fsck: Check a File System	24
%root^file system=24
%file system^root=24
%umount=24
%fsck=24
%sync=24
%i-node^list=24
+2Devices, Files, and Drivers	24
%device-independent I/O=24
%special file^block=24
%file^block special=24
%/etc/update=24
%sync=24
+2Character-Special Files	25
%special file^block=25
%file^block special=25
%file^special character=25
%raw files=25
%file^raw=25
%fsck=25
%dump=25
+2tty Processing	25
%terminal^mode=25
%cooked files=25
%files^cooked=25
+2A Tour Through the File System	25
%file system^layout=25
+2General File System Layout	25
%directory^root=25
%root^directory=25
%/=25
+2/bin	25
%/bin=25
%directory^current=25
%current directory=25
%/=25
%shell=25
%sh=25
%PATH=25
+2/dev	25
%/dev=25
+2/drv	25
%/drv=25
%/etc/drvld=25
%drvld=25
+2/etc	26
%/etc=26
%passwd=26
%/etc/passwd=26
%/etc/ttys=26
+2/lib	26
%standard I/O=26
%/lib=26
+2/usr	26
%/usr=26
%/usr/adm=26
%/usr/bin=26
%/usr/games=26
%/usr/games/lib/fortunes=26
%/usr/include=26
%nroff macros=26
%ms=26
%man=26
%nroff=26
%units=26
%/usr/lib/crontab=26
%cron=26
%/usr/man=26
%/usr/msgs=26
%/usr/pub=26
%/usr/spool=26
+2/u	26
%/u=26
+2Files: Conclusion	26
+1Introduction to COHERENT Commands	26
+2The Shell	27
%sh=27
%shell=27
+2Redirecting Input and Output	27
%cat=27
%>=27
%who=27
%I/O redirection=27
%standard^output=27
%user name=27
%mail=27
%<=27
%standard^input=27
+2Pipes	27
%pipe=27
%|=27
%filter=27
+2Superuser	28
%superuser=28
%root=28
%prompt=28
%password=28
+2vsh:  The Visual Shell	28
%vsh=28
+2Manipulating Text Under COHERENT	29
+2MicroEMACS: Text Screen Editor	29
%MicroEMACS=29
%me=29
%screen editor=29
+2pr, prps, lp: Print Files	30
%pr=30
+2nroff, troff: Text Formatters	31
%nroff=31
%troff=31
+2Miscellaneous Commands	32
+2who: Who Is on the System	32
%who=32
+2write: Electronic Dialogue	32
%msg=32
%write=32
%<ctrl-D>=32
%write=32
%write=32
%write=32
+2mail: Send an Electronic Letter	32
%mail=32
%mail^receiving=33
%receiving mail=33
%mail=34
%<Return>=34
+2msgs: Cumulative Message Board	34
%msgs=34
+2grep: Find Patterns in Text Files	34
%grep=34
%patterns=34
%regular expressions=34
%grep=34
%patterns=34
%grep=35
%patterns=35
+2date: Print the Date	35
%date=35
%time=35
%GMT=35
%timezone=35
+2passwd: Change Your Password	35
%password=35
%passwd=35
+2stty: Change Terminal Behavior	35
%terminal=35
%stty=35
%tab=36
%kill=36
%erase=36
%<kill>=36
%<erase>=36
%kill=36
%erase=36
+2Scheduling Commands For Regular Execution	36
%event scheduling=36
%24-hour time=37
%cron=37
%- (hyphen)=37
%, (comma)=37
+2Managing Processes	37
+2ps: List Active Processes	37
%process=37
%process^id=37
%PID=37
%&=37
%ps=37
%process^id=37
%/etc/ttys=37
+2kill: Signal Processes	38
%kill=38
%superuser=38
%kill=38
%root=38
%su=38
+1Programming Under COHERENT	38
%C=38
%assembly language=38
+2Basic Steps in COHERENT Programming	38
%macro=39
%C=39
%m4=39
%db=39
%cc=39
%ed=39
+2Create the Program Source	39
%parentheses=39
%main=39
%brace=39
%{}=39
%printf=39
%library^C=39
%newline^in C strings=39
%output formatting=39
+2cc: Compile the Program	39
%C=39
%cc=39
%ld=39
%compiler^C=39
%C^program linker=39
+2m4: Macro Processing	40
%m4=40
%m4=40
%parameter^substitution=40
%substitution^of parameters=40
+2make: Build Larger Programs	40
%make=40
%program^modularity=40
%file^include=40
%make=40
%file^modification time=40
+2db: Debug the Program	40
%debugging=40
%program^debugging=40
%structured^programming=40
%programming^structured=40
%db=40
%db=40
%machine instructions=40
%db=40
+1Administering the COHERENT System	41
+2Adding a New User	41
%user^name=41
%user^id=41
%password=41
%/etc/passwd=41
%group^name=41
%group^id=41
%newusr=41
+2System Security	41
+2Passwords	41
%protection=41
%password=41
%login=41
%password=41
%passwd=41
%/etc/passwd=41
+2File Protection	41
%file^protection=41
%protection=41
+2Encryption	42
%crypt=42
+2Dumping and Saving Files	42
+2System Accounting	42
%computer time accounting=42
%accounting=42
%accounting^reports=42
+2ac: Login Accounting	42
%accounting^login=42
%login^time=42
%/usr/adm/wtmp=43
%accounting^starting login=43
+2sa: Processing Accounting	43
%accounting^process=43
%system^time=43
%user^time=43
%real time=43
%accounting^starting process=44
%/usr/adm/acct=44
%/usr/adm/savacct=44
+1Conclusion	44
.pn 44+1
%sh^tutorial=45
+0Introducing sh, the Bourne Shell	45
%script=45
%shell=45
%commands^COHERENT=45
+1Simple Commands	45
%shell^simple commands=45
%semicolons=45
%;=45
%shell^sequential execution of commands=45
+1Special Characters	45
+1Running Commands in the Background	45
%commands^concurrent execution=45
%&=45
%commands^background=46
%process^id=46
%ps=46
%process=46
%ps=46
%background process=46
%process^background=46
%wait=46
+1Scripts	46
%file^of commands=46
%commands^in files=46
%sh=46
%script=46
%shell^script=46
%chmod=46
%sh=46
%script=47
+1†.profile: Login Shell Script	47
%prompt=47
%directory^home=47
%home directory=47
+1Substitutions	47
%substitution^in commands=47
%echo=47
+1File Name Substitution	47
%*=47
%patterns=47
%*=48
%echo=48
%*=48
%apostrophe=48
%?=48
%question mark=48
%file^creating empty=48
%]=48
%[=48
%order^of matched file names=49
%patterns=49
%/=49
+1Parameter Substitution	49
%parameter^positional=49
%script=49
%$=49
%parameter^positional=49
%parameter^fewer=49
%parameter^null=49
+1Shell Variable Substitution	50
%shell^variable=50
%case sensitivity^in shell variable=50
%shell^variable=50
%variable^shell=50
%space=51
%keyword^parameters=51
%parameter^keyword=51
%brace=51
%parameter^assigning keyword=51
%parameter^positional=52
%export=52
%shell^variable=52
%variable^shell=52
+1Command Substitution	52
%grave accent=52
+1Special Shell Variables	53
%cd=53
%HOME=53
%directory^home=53
%default^directory=53
%PS1=53
%PS2=53
%>=53
%$=53
%default^prompt=53
%prompt=53
%/bin=53
%PATH=53
%: (colon)=53
%parent directory=53
%directory^parent=53
+1dot . : Read Commands	53
%dot command=53
+1Values Returned by Commands	54
%failure=54
%success=54
%commands^value=54
%value from command=54
%cmp=54
+1test: Condition Testing	54
%test=54
%parameter^substitution=54
%testing^strings=54
%substitution^of parameters=54
+1Executing Commands Conditionally	54
%||=54
%cmp=54
%&&=55
+1Control Flow	55
+2for: Execute a Loop	55
%do=55
%done=55
%done=55
%>=55
%for=55
+2if: Execute Conditionally	56
%fi=56
%else=56
%if=56
%false=56
%if=56
%elif=56
+2while: Execute a Loop	57
%while=57
+2until: Another Looping Construct	57
%until=57
+2case: Serial Conditional Execution	57
%case=57
%)=57
%esac=57
%case=57
%case=58
%*=58
%choices^in case statements=58
%*)=58
%vertical bar=58
%|=58
%;; (double semicolon)=58
+1Summary	58
.pn 58+1
%MicroEMACS^tutorial=59
%me^tutorial=59
+0Introduction to MicroEMACS	59
+1What is MicroEMACS?	59
+1Keystrokes: <ctrl>, <esc>	59
%MicroEMACS^<ctrl>=59
+1Becoming Acquainted with MicroEMACS	59
+2Beginning a Document	60
+1Moving the Cursor	61
%MicroEMACS^backspace key=61
%MicroEMACS^<return>=61
%MicroEMACS^arrow keys=61
%MicroEMACS^movement commands=61
%MicroEMACS^cursor movement display=61
+2Moving the Cursor Forward	61
%MicroEMACS^<ctrl-F>=61
%MicroEMACS^<esc>F=61
%MicroEMACS^<ctrl-E>=61
%MicroEMACS^right=61
%MicroEMACS^forward, one space=61
%MicroEMACS^forward, one word=61
%MicroEMACS^forward, end of line=61
+2Moving the Cursor Backwards	61
%MicroEMACS^<ctrl-B>=61
%MicroEMACS^<esc>B=61
%MicroEMACS^<ctrl-A>=61
%MicroEMACS^back=61
%MicroEMACS^left=61
%MicroEMACS^backward, one space=61
%MicroEMACS^backward, one word=61
%MicroEMACS^backward, end of line=61
+2From Line to Line	61
%MicroEMACS^<ctrl-N>=61
%MicroEMACS^next line=61
%MicroEMACS^line position=61
%MicroEMACS^previous line=61
%MicroEMACS^<ctrl-P>=61
+2Repetitive Motion	62
%MicroEMACS^repetition=62
+2Moving Up and Down by a Screenful of Text	62
%MicroEMACS^screen up=62
%MicroEMACS^screen down=62
%MicroEMACS^<ctrl-V>=62
%MicroEMACS^<esc>V=62
+2Moving to Beginning or End of Text	62
%MicroEMACS^end of text=62
%MicroEMACS^<esc>>=62
%MicroEMACS^beginning of text=62
%MicroEMACS^<esc><=62
+2Saving Text and Quitting	62
%MicroEMACS^<ctrl-X><ctrl-C>=62
%MicroEMACS^quit=62
%MicroEMACS^saving text=62
%MicroEMACS^<ctrl-X><ctrl-S>=62
+1Killing and Deleting	62
%MicroEMACS^erase text=62
%MicroEMACS^killing and deleting=63
+2Deleting Vs. Killing	63
%MicroEMACS^kill text, versus deleting=63
%MicroEMACS^delete text, versus killing=63
%MicroEMACS^erase text, erasing spaces=63
+2Erasing Text to the Right	63
%MicroEMACS^<ctrl-D>=63
%MicroEMACS^<del>=63
%MicroEMACS^text, erase to right=63
%MicroEMACS^erase text, to the right=63
%MicroEMACS^<esc>D=63
+2Erasing Text to the Left	63
%MicroEMACS^erase text, to the left=63
%MicroEMACS^<backspace>=63
%MicroEMACS^text, erase to left=63
+2Erasing Lines of Text	63
%MicroEMACS^erase text, by line=63
%MicroEMACS^text, kill by lines=63
+2Yanking Back (Restoring) Text	64
%MicroEMACS^restore (yank back) killed text=64
%MicroEMACS^yank back text=64
%MicroEMACS^text, restore (yank back)=64
%MicroEMACS^text, yank back (restore)=64
%MicroEMACS^<ctrl-Y>=64
+2Quitting	64
%MicroEMACS^<ctrl-X><ctrl-C>=64
%MicroEMACS^quit without saving text=64
+1Block Killing and Moving Text	64
%MicroEMACS^move text=64
%MicroEMACS^text, move=64
%MicroEMACS^buffer, for killed text=64
%MicroEMACS^block-kill text=64
%MicroEMACS^move text=64
%MicroEMACS^kill and move commands=64
+2Moving One Line of Text	64
+2Multiple Copying of Killed Text	64
+2Kill and Move a Block of Text	64
%MicroEMACS^kill text, block=64
%MicroEMACS^<ctrl-@>=64
%MicroEMACS^<ctrl-W>=64
%MicroEMACS^text, block kill=64
+1Capitalization and Other Tools	65
+2Capitalization and Lowercasing	65
%MicroEMACS^uppercase=65
%MicroEMACS^lowercase=65
%MicroEMACS^capitalization=65
%MicroEMACS^<esc>C=65
%MicroEMACS^text, capitalize=65
%MicroEMACS^<esc>U=65
%MicroEMACS^text, uppercase=65
%MicroEMACS^<esc>L=65
%MicroEMACS^text, lowercase=65
+2Transpose Characters	66
%MicroEMACS^<ctrl-T>=66
%MicroEMACS^transpose characters=66
+2Screen Redraw	66
%MicroEMACS^<ctrl-L>=66
%MicroEMACS^screen redraw=66
%MicroEMACS^redraw screen=66
%MicroEMACS^center line on screen=66
%MicroEMACS^<ctrl-U><ctrl-L>=66
+2Return Indent	66
%MicroEMACS^return indent=66
%MicroEMACS^block indentation=66
+2Word Wrap	67
%MicroEMACS^word wrap=67
%MicroEMACS^<ctrl-X>F=67
+1Search and Reverse Search	68
%MicroEMACS^searching=68
%MicroEMACS^commands=68
+2Search Forward	68
%MicroEMACS^search, forward=68
%MicroEMACS^<esc>S=68
%MicroEMACS^<return>=68
+2Reverse Search	68
%MicroEMACS^search, reverse=68
%MicroEMACS^reverse search=68
%MicroEMACS^<esc>R=68
+2Cancel a Command	69
%MicroEMACS^cancel a command=69
%MicroEMACS^<ctrl-G>=69
+2Search and Replace	69
%MicroEMACS^<esc>%=69
%MicroEMACS^search and replace=69
+1Saving Text and Exiting	70
%MicroEMACS^text, saving=70
%MicroEMACS^exit=70
%MicroEMACS^saving text=70
%MicroEMACS^extended commands=70
%MicroEMACS^<ctrl-X>=70
+2Write Text to a New File	70
%MicroEMACS^text, write to new file=70
%MicroEMACS^<ctrl-X><ctrl-W>=70
+2Save Text and Exit	70
%MicroEMACS^<ctrl-Z>=70
%MicroEMACS^store command=70
+1Advanced Editing	70
+1Arguments	71
%MicroEMACS^arguments=71
%MicroEMACS^arguments=71
%MicroEMACS^<ctrl-U>=71
+2Arguments: Default Values	71
%MicroEMACS^arguments, default value=71
%MicroEMACS^arguments, increasing or decreasing=71
+2Selecting Values	71
%MicroEMACS^arguments, selecting values=71
+2Deleting With Arguments: An Exception	72
%MicroEMACS^yank back text=72
%MicroEMACS^arguments, deleting=72
+1Buffers and Files	72
+2Definitions	72
%MicroEMACS^buffer, definition=72
%MicroEMACS^file, definition=72
%MicroEMACS^buffer, how differs from file=72
%MicroEMACS^file, how differs from buffer=72
%MicroEMACS^file, naming=72
%MicroEMACS^buffer, naming=72
+2File and Buffer Commands	72
%MicroEMACS^file and buffer commands=72
+2Write and Rename Commands	72
%MicroEMACS^<ctrl-X><ctrl-W>=72
%MicroEMACS^write text to new file=72
%MicroEMACS^<ctrl-X><ctrl-F>=73
%MicroEMACS^rename file=73
%MicroEMACS^file, rename=73
+2Replace Text in a Buffer	73
%MicroEMACS^<ctrl-X><ctrl-R>=73
%MicroEMACS^replace buffer with named file=73
%MicroEMACS^buffer, replace with named file=73
%MicroEMACS^file, replace buffer with named f.=73
+2Visiting Another Buffer	73
%MicroEMACS^<ctrl-X><ctrl-V>=73
%MicroEMACS^switch buffers=73
%MicroEMACS^visit command=73
%MicroEMACS^buffer, switch b.=73
+2Move Text From One Buffer to Another	74
%MicroEMACS^text, move from one buffer to another=74
+2Checking Buffer Status	74
%MicroEMACS^number of buffers allowed=74
%MicroEMACS^buffers, number allowed=74
%MicroEMACS^buffer status command=74
%MicroEMACS^buffer status=74
%MicroEMACS^<ctrl-X><ctrl-B>=74
%MicroEMACS^buffer status window=74
%MicroEMACS^<ctrl-X>1=74
+2Renaming a Buffer	74
%MicroEMACS^buffer, prompting for new name=74
%MicroEMACS^buffer, need unique names=74
+2Delete a Buffer	74
%MicroEMACS^<ctrl-X>K=74
%MicroEMACS^delete buffer command=74
%MicroEMACS^buffer, delete=74
+1Windows	75
+2Creating Windows and Moving Between Them	75
%MicroEMACS^<ctrl-X>2=75
%MicroEMACS^window manipulation=75
%MicroEMACS^<ctrl-X>N=76
%MicroEMACS^<ctrl-X>P=76
%MicroEMACS^window, shifting between=76
%MicroEMACS^window, number possible=76
%MicroEMACS^arguments, with create window=76
%MicroEMACS^<ctrl-X>1=76
+2Enlarging and Shrinking Windows	76
%MicroEMACS^<ctrl-X>Z=76
%MicroEMACS^window, enlarge=76
%MicroEMACS^<ctrl-X><ctrl-Z>=76
%MicroEMACS^window, shrink=76
+2Displaying Text Within a Window	76
%MicroEMACS^<ctrl-X><ctrl-N>=77
%MicroEMACS^window, scroll down=77
%MicroEMACS^<ctrl-X><ctrl-P>=77
%MicroEMACS^scroll up=77
%MicroEMACS^scroll down=77
%MicroEMACS^<esc>!=77
%MicroEMACS^window, move within=77
+2One Buffer	77
%MicroEMACS^window, use with editing=77
%MicroEMACS^window, using multiple buffers=77
%MicroEMACS^buffer, with windows=77
%MicroEMACS^file, with windows=77
%MicroEMACS^switch buffer command=77
%MicroEMACS^<ctrl-X>B=77
+2Multiple Buffers	77
+2Moving and Copying Text Among Buffers	78
%MicroEMACS^window, moving text among=78
%MicroEMACS^window, copying text among=78
%MicroEMACS^copying text=78
+2Checking Buffer Status	78
%MicroEMACS^window, buffer status command use=78
%MicroEMACS^buffer status command, with windows=78
+2Saving Text From Windows	78
%MicroEMACS^window, saving text=78
+1Keyboard Macros	78
+2Creating a Keyboard Macro	78
%MicroEMACS^end macro command=78
%MicroEMACS^<ctrl-X>E=78
%MicroEMACS^execute macro command=78
+2Execute a Macro Repeatedly	79
+2Replacing a Macro	79
+2Renaming a Macro	79
+2Renaming Macros:  A Few Caveats	80
+2Setting the Initialization Macro	80
+1Flexible Key Bindings	80
+2Changing a Keybinding	80
%MicroEMACS^metakey=81
+2Rebinding Metakeys	81
+2Save and Restore Keybindings	81
%.emacs.rc=82
%MicroEMACS^.emacs.rc=82
+1Sending Commands to COHERENT	82
%MicroEMACS^program interrupt=82
%MicroEMACS^<ctrl-X>!=82
%MicroEMACS^<ctrl-C>=82
+2Compiling and Debugging Through MicroEMACS	82
%MicroEMACS^compiling and debugging=82
%cc^MicroEMACS mode=82
%MicroEMACS^automatic mode=82
%MicroEMACS^<ctrl-X>>=83
%MicroEMACS^next error=83
%MicroEMACS^<ctrl-X><=83
%MicroEMACS^previous error=83
+2The MicroEMACS Help Facility	83
%MicroEMACS^help, in MicroEMACS=83
%MicroEMACS^help window=83
%MicroEMACS^<ctrl-X>=83
%MicroEMACS^<esc>?=83
%MicroEMACS^<esc>2=83
+1Where To Go From Here	83
.pn 84+1
%ed^tutorial=85
+0Introduction to the ed Line Editor	85
+2Why You Need an Editor	85
+2Learning To Use the Editor	85
+1General Topics	85
%ed^<ctrl-D>=86
+2ed, Files, and Text	86
%ed^carriage return=86
+2Creating a File	86
+2Changing an Existing File	86
+2Working on Lines	87
%ed^line, number=87
+2Error Messages	87
+1Basic Editing Techniques	87
+2Creating a New File	87
%ed^prompt character=87
%ed^adding lines=87
%ed^sed=87
%ed^. (dot)=88
+2Changing a File	88
%ed^file, name, in ed command=88
%ed^current, line=88
%ed^current, line=89
%ed^inserting lines=89
+2Printing Lines	89
%ed^print command=89
%ed^line, number ranges=90
+2Abbreviating Line Numbers	90
%ed^$=90
%ed^*=90
%ed^$=90
+2How Many Lines?	90
%ed^==90
%ed^.==90
+2Removing Lines	91
%ed^removing lines=91
%ed^deleting lines=91
+2Abandoning Changes	92
+2Substituting Text Within a Line	92
%ed^changing lines=92
%ed^substitute command=92
%ed^pattern=92
+2Undoing Substitutions	93
+2Global Substitutions	94
%ed^global substitute=94
+2Special Characters	94
%ed^characters, special=94
+2Ranges of Substitution	94
+1Intermediate Editing	95
%ed^commands, advanced=95
+2Relative Line Numbering	95
%ed^line, numbers, relative=95
%ed^. (dot)=95
%ed^+=95
%ed^-=95
+2Changing Lines	96
+2Moving Blocks of Text	97
%ed^move, blocks of text=97
%ed^line, number zero=97
+2Copying Blocks of Text	98
%ed^copying blocks of texts=98
+2String Searches	98
%ed^line, locators=98
%ed^?=98
+2Remembered Search Arguments	99
+2Uses of Special Characters	99
%ed^backslash=100
+2Global Commands	100
%ed^global, command=100
+2Joining Lines	100
%ed^joining lines=100
+2Splitting Lines	101
%ed^spliting lines=101
+2Marking Lines	101
+2Searching in Reverse Direction	103
%ed^reverse searching=103
+1Expert Editing	103
%ed^advanced commands=103
%ed^commands, advanced=103
+2File Processing Commands	103
%ed^file, editing commands=103
+2Patterns	104
%ed^special characters=104
%ed^characters, special=104
+2Matching Many With One Character	105
+2Beginning and Ending of Lines	106
%ed^caret=106
+2Replacing Matched Part	106
%ed^&=106
+2Replacing Parts of Matched String	106
+2Listing Funny Lines	108
+2Keeping Track of Current Line	108
%ed^current, line=108
+2When Current Line Is Changed	109
%ed^;=109
+2More About Global Commands	110
%ed^global, command=110
%ed^commands, global=110
+2Issuing COHERENT Commands Within ed	110
+1For More Information	110
.pn 111+1
%sed^tutorial=112
+0Introduction to the sed Stream Editor	112
+1Getting to Know sed	112
%sed^pipes=112
%sed^ed=112
+2Getting Started	112
+2Simple Commands	113
%sed^>=113
+2Substituting	113
%sed^substitution=113
+2Selecting Lines	114
%sed^line^selection=114
%sed^line^range=115
%sed^pattern=115
%sed^$=115
+2p: Print Lines	115
%sed^p command^with s=116
+2Line Location	117
+2Add Lines of Text	118
+2Delete Lines	119
+2Change Lines	120
%sed^change lines=120
+2Include Lines From a File	120
%sed^including a file=120
%sed^reading in=120
+2Quit Processing	121
+2Next Line	121
%sed^next line=121
+1Advanced sed Commands	122
+2Work Area	123
+2Add to Work Area	123
+2Print First Line	124
+2Save Work Area	125
+2Transform Characters	127
+2Command Control	128
+2{ }: Command Grouping	128
+2!: All But	128
+2= : Print Line Number	128
+2Skipping Commands	129
+2t: Test Command	129
+1For More Information	130
.pn 130+1
%C language^tutorial=131
+0The C Language	131
%Ritchie, Dennis=131
%Thompson, Ken=131
+1Compiling C Programs under COHERENT	131
%cc=131
%compiler=131
+2Try the Compiler	131
+2Phases of Compilation	131
%cpp=132
%C preprocessor=132
%cc0=132
%parser=132
%cc1=132
%code generator=132
%cc2a=132
%optimizer generator=132
%optimization=132
%cc2b=132
%object generator=132
%optimization=132
%cc3=132
%assembly-language generator=132
%object module=132
%object module=132
%linker=132
%executable program=132
+2Renaming Executable Files	132
%executable files=132
+2Floating-Point Numbers	132
%floating-point numbers=132
+2Compiling Multiple Source Files	133
%multiple source files=133
%factor.c=133
%atod.c=133
%mathematics library=133
%libm=133
+2Linking Without Compiling	133
%linking without compiling=133
%source file=133
%object module=133
+2Compiling Without Linking	133
%compiling without linking=133
+2Assembly-Language Files	133
%assembly-language programs=133
%as=133
+2Changing the Size of the Stack	134
%stack size=134
%altering stack size=134
+2Where To Go From Here	134
troff: /v/doc/coherent/text/c_language.r: 401: cannot open file "/v/fwb/coherent/text/beginners.r"
troff: /v/doc/coherent/text/c_language.r: 402: cannot open file "/v/fwb/coherent/text/bibliography.r"
.pn 134+1
%awk^tutorial=135
+0Introduction to the awk Language	135
%Aho, A.V.=135
%Weinberger, P.J.=135
%Kernighan, Brian W.=135
+1Example Files	135
%Shakespeare, William=135
%Ruth, Babe=135
+1Using awk	136
+2Command-line Options	136
%awk^command-line options=136
+1Structure of an awk Program	136
%awk^statement=136
+2Records and Fields	137
%awk^records=137
%awk^fields=137
%awk^input-record separator=137
%awk^input-field separator=137
%awk^output-record separator=137
%awk^output-field separator=137
+1Patterns	137
+2Special Patterns	137
%awk^special patterns=137
%awk^pattern, special=137
%awk^BEGIN=137
%awk^END=137
%awk^FILENAME=137
%awk^FS=137
%awk^NF=137
%awk^NR=137
%awk^OFS=137
%awk^ORS=137
%awk^RS=137
+2Arithmetic Relational Expressions	138
%awk^arithmetic operators=138
%awk^,=138
+2Boolean Combinations of Expressions	140
%awk^Boolean operators=140
%Boole, George=140
+2Patterns	141
%awk^patterns=141
%ed=141
%sed=141
%awk^special characters=141
+2Ranges of Patterns	142
%awk^patterns, range=142
+2Resetting Separators	143
%awk^separators, reset=143
%awk^FS=143
%awk^OFS=143
%awk^ORS=143
%awk^RS=143
%awk^;=143
%awk^==144
+1Actions	145
%awk^actions=145
%Free Software Foundation=145
+2awk Functions	145
%awk^functions=145
%awk^abs=145
%awk^exp=146
%Euler, Leonhart=146
%awk^index=146
%awk^int=146
%awk^length=146
%awk^log=146
%awk^print=146
%awk^printf=146
%awk^sprintf=146
%awk^sqrt=146
%awk^substr=146
+2Printing with awk	146
%awk^printing=146
+2Redirecting Output	149
%awk^redirecting output=149
+2Assignment of Variables	149
%awk^assigning variables=149
%awk^arithmetic operators=150
+2Field Variables	150
%awk^field variables=150
+2Control Statements	150
%awk^control statements=150
%awk^if=150
%awk^else=150
%awk^while=151
%Shakespeare, William=151
%awk^for=151
%awk^break=152
%awk^continue=152
%awk^next=152
%awk^exit=152
+1Arrays	152
%awk^arrays=152
+2Initializing an Array	153
+2The for() Statement With Arrays	153
%awk^++=154
+1For More Information	154
.pn 154+1
%lex^tutorial=155
+0Introduction to lex, the Lexical Analyzer	155
%lex^program generator=155
+1How To Use lex	155
+2Translating Strings	155
%lex^lex specification=155
+2Remove Blanks From Input	155
+2Trimming Blanks	156
+1lex Specification Form	156
+2Simple Form	156
%lex^%%=156
%lex^definitions=156
%lex^rules=156
+2Rules in lex	156
%lex^pattern=156
%lex^action=156
+2Statements in lex	157
%lex^statements=157
%lex^yywrap=157
+2Groups of Statements	158
%lex^statements multiple=158
%lex^braces=158
%lex^repetitions, zero or more=158
%lex^yytext=159
+2Using the Same Action	159
%lex^rules, with same action=159
+1Patterns	159
%lex^patterns=159
%lex^regular expressions=159
+2Simple Patterns	159
+2Classes of Characters	160
%lex^patterns=160
%lex^dot=160
%lex^character classes=160
%lex^match, exception=160
%lex^exception=160
+2Repetition	161
%lex^repetition=161
%lex^+=161
%lex^repetion, zero or more=161
%lex^*=161
%lex^match, longest=161
%lex^?=162
%lex^optional match=162
%lex^match, optional=162
%lex^?=162
%lex^repetitions, zero or one=162
%lex^braces, in patterns=162
%lex^repetition, specific count=162
+2Choices and Grouping	162
%lex^alternatives=162
%lex^|=162
%lex^( and )=162
%lex^grouping, ()=162
+2Matching Non-Graphic Characters	163
%lex^non-graphic character=163
%lex^match, non-graphic characters=163
%lex^non-graphic characters=163
+1More Patterns	163
+2Line Context	163
%lex^end of line=163
%lex^beginning of line $=163
%lex^$=163
+2Context Matching	163
%lex^//=163
%lex^match, in context=163
%lex^context match=163
%lex^context match=164
%lex^match, in context=164
+2Macro Abbreviations	165
%lex^macro=165
%lex^abbreviations=165
%lex^macro=165
+2Context: Start Rules	165
%lex^context, start=165
%lex^rules, context start=165
%lex^start condition=165
%lex^definitions=165
%lex^%S=165
%lex^< >=165
%lex^angle brackets=165
%lex^BEGIN action=165
+2Separate Contexts	166
%lex^context, separate=166
%lex^context, switch=167
+1More About Writing Actions	167
+2ECHO	167
%lex^ECHO=167
+2Processing Overlapping Strings	168
%lex^REJECT=168
+2yylex	168
%lex^yylex=168
%lex^definitions section=168
%lex^sections, definitions=168
+2Header Section	169
%lex^header section=169
%lex^section, header=169
%lex^%{ %}=169
+2Additional Routines	169
%lex^yywrap=169
+1Using lex With yacc	169
%lex^yacc=169
%lex^tokens=169
+1Summary	170
.pn 170+1
%yacc^tutorial=171
+0Introduction to yacc	171
+1Examples	171
+2Phrases and Parentheses	171
+2Simple Expression Processing	173
+1Background	174
+2LR Parsing	174
%yacc^LR parsing=174
%yacc^left-to-right parsing=174
%yacc^LALR=174
+2Input Specification	174
%yacc^BNF=174
%yacc^Backus-Naur Form=174
%Backus-Naur Form=174
%BNF=174
+2Parser Operation	174
%yacc^yyparse=174
%yacc^library=174
%yacc^library, yacc=174
%yacc^push-down list=174
%yacc^stack=174
%yacc^parse actions=174
%yacc^shift=174
%yacc^action, shift=174
%yacc^reduce=174
%yacc^action, reduce=174
%yacc^error action=175
%yacc^action, error=175
%yacc^accept action=175
%yacc^action, accept=175
%yacc^start symbol=175
+1Form of yacc Programs	175
%yacc^%%=175
%yacc^definition section=175
%yacc^section, definition=175
%yacc^rules section=175
%yacc^section, rules=175
+2Definitions	175
%yacc^definitions section=175
%yacc^token definition=175
%yacc^terminals=175
%yacc^nonterminals=175
%yacc^start symbol=175
+2Rules	175
%yacc^{ }=175
+2User Code	176
%yacc^action statements=176
%yacc^user code=176
+1Rules	176
%yacc^rule, sections=176
%yacc^production=176
%yacc^reduction=176
+2General Form of Rules	176
%yacc^rule format=176
%yacc^comments, in rules=176
+2Suggested Style	176
%yacc^rule, format=176
%yacc^rule, style=176
%yacc^actions=176
+1Actions	177
%yacc^rule, actions=177
+2Basic Action Statements	177
+2Action Values	177
%yacc^rule, values=177
%yacc^token, value=178
+2Structured Values	179
%yacc^token, definition=180
%yacc^%token=180
%yacc^rule, type=180
%yacc^%token=180
%yacc^type, of nonterminal=180
%yacc^value, qualification=180
%yacc^default, action=180
+1Handling Ambiguities	180
%yacc^ambiguity=180
+2How yacc Reacts	181
%yacc^ambiguity, default handling=181
%yacc^shift-reduce conflicts=181
%yacc^shift=181
%yacc^reduce=181
%yacc^shift-reduce conflicts=181
+2Additional Control	181
%yacc^ambiguity, resolution=181
%yacc^associativity=181
%yacc^definition section=181
%yacc^associative, left=181
%yacc^associative, right=181
%yacc^%left=181
%yacc^%token=181
%yacc^%right=182
+2Precedence	182
%yacc^precedence=182
%yacc^%left=182
%yacc^precedence=182
%yacc^%left=182
%yacc^%right=182
%yacc^%prec=182
%yacc^rules, precedence=182
%yacc^precedence=182
%yacc^%nonassoc=182
%yacc^nonassociative=182
%yacc^%left=182
%yacc^right=182
+1Error Handling	183
%yacc^error, recovery=183
%yacc^error, token=183
%yacc^token, error=183
%yacc^error, recovery=183
%yacc^error, token=183
%yacc^token, error=183
%yacc^yyerrok=183
+1Summary	183
+2Helpful Hints	184
+1Where to Go From Here	184
.pn 184+1
%bc^tutorial=185
+0bc Desk Calculator Language	185
+2Entry and Exit	185
+2Example of Simple Use	185
%bc^exponentiation operator=185
+1Simple Statements	186
%bc^assignment=186
+1Numbers with Fractions	188
+2The Scale of Numbers	188
+2Addition and Subtraction	188
+2Scale During Multiplication	189
+2Setting the Scale of Results	189
+2Scale for Divisions	189
+2Scale From Exponentiation	190
+2What Is the Current Scale?	190
+1The if Statement	190
+2Using the if Statement	190
+2Comparisons	190
+2Grouped Statements	191
+2Many Statements Per Line	191
+1The while Statement	192
+2Abbreviations in the while Statement	193
+1The for Statement	193
+2Three Parts of the for Statement	193
+2Similarities Between the for and while Statements	194
+1Functions in bc	194
+2Example of Function Use	194
+2Functions Using Other Functions	195
+2Functions That Call Themselves	195
+2The auto Statement	196
+1Programs in a File	196
+2Using a Program From a File	196
+2Using Libraries	197
+2The bc Library	197
%bc^library=197
+1Summary	198
.pn 198+1
%m4^tutorial=199
+0Introduction to the m4 Macro Processor	199
%4GL=199
%fourth-generation language=199
+2Definitions and Syntax	199
%m4^quoted text=199
%m4^unquoted text=199
%m4^nestable quotes=199
%m4^quote marks^removing=199
+2Defining Macros	200
%m4^define=200
%m4^macro name recognition=200
%m4^argument=200
%m4^argument substitution=200
%m4^dumpdef=201
%m4^undefine=201
+2Input Control	201
%m4^changequote=201
%m4^dnl=201
%m4^extra newlines=201
%m4^decision-making macro=201
%m4^ifdef=201
%m4^sinclude=202
%m4^include=202
+2Output Control	202
%m4^output stream=202
%m4^divert=202
%m4^extra newlines=202
%m4^undivert=202
%m4^divnum=203
%m4^errprint=203
+2String Manipulation	203
%m4^substr=203
%m4^translit=203
+2Numeric Manipulation	204
%m4^incr=204
%m4^decr=204
%m4^eval=204
%m4^expression evaluation=204
%m4^eval=204
%m4^string length=204
%m4^index=204
%m4^repeat=205
%m4^ifelse=205
%m4^eval=205
%m4^decr=205
+1COHERENT System Interface	205
%m4^maketemp=205
%m4^syscmd=205
%m4^dumpdef=206
%m4^syscmd=206
+2Errors	207
%m4^endless loop=207
+1For More Information	207
.pn 208+1
%make^tutorial=209
+0The make Programming Discipline	209
+2How Does make Work?	209
+2Try make	210
+1Essential make	210
+2The makefile	210
%make^program, specification=210
%make^specification=210
%make^file=210
%make^colon=211
%make^command line=211
%make^macro, definition=211
%make^comment=211
+2Building a Simple makefile	211
+2Comments and Macros	211
+2Setting the Time	212
+2Building a Large Program	212
+1Command-Line Options	213
%make^command line, options=213
%make^command line=213
%make^-=213
%make^hyphen=213
%make^options=213
%make^debug option=213
%make^file option=213
%make^program, specification=213
%make^specification=213
%make^Makefile=213
%make^file=213
%make^ignore errors option=213
%make^error status=213
%make^command, error=213
%make^no execution option=213
%make^print option=213
%make^macro, printing=213
%make^target, printing=213
%make^printing=213
%make^macro, definition=213
%make^rules option=213
%make^no rules option=213
%make^/usr/lib/makemacros=213
%make^macros=213
%make^/usr/lib/makeactions=213
%make^actions=213
%make^silent option=213
%make^command, printing=213
%make^touch option=213
%make^modification time=213
%make^file modification time=213
+2Other Command Line Features	213
%make^command line, macro definition=213
%make^command line=213
%make^macro, definition=213
%make^macro=213
%make^command line, target specification=214
%make^command line=214
%make^target=214
%make^target, specification=214
%make^target, program=214
+1Advanced make	214
+2Default Rules	214
%make^default rules=214
%make^/usr/lib/makemacros=214
%make^macros=214
%make^/usr/lib/makeactions=214
%make^usr/lib/makeactions=214
%make^actions=214
%make^.SUFFIXES=214
%make^$<=214
%make^$*=214
%make^$?=215
%make^$@=215
%make^assembler=215
%make^yacc=215
%make^lex=215
+2Source File Path	215
+2Double-Colon Target Lines	215
%make^double colon=215
%make^target, line=215
%make^archive=215
%make^colon=215
+2Special Targets	216
%make^special targets=216
%make^target=216
%make^.DEFAULT=216
%make^.IGNORE=216
%make^.SILENT=216
%make^silent option=216
+2Errors	216
%make^errors=216
%make^command, error=216
%make^error status=216
%make^ignore errors option=216
%make^-=216
%make^interrupt=216
+2Exit Status	216
%make^exit status=216
%make^return value=216
+1Alternative Uses	216
+1Where To Go From Here	217
.pn 218+1
%nroff^tutorial=219
+0nroff, The Text-Formatting Language	219
+2What is nroff?	219
%Belloc, Hilaire=219
+2nroff Input and Output	219
+2Printing nroff Output	220
+2nroff Limitations	220
+2The ms Macro Package	220
%nroff^ms macros=220
%nroff^macro definition=221
+2Using this Tutorial	221
+1The ms Macro Package	221
%nroff^.PP macro=221
+2Text and Commands	221
%Dickens, Charles=221
%nroff^justify text=222
%nroff^margin, right=222
%nroff^breaking line=222
%nroff^hyphenation=222
%nroff^.sp primitive=222
%nroff^space, vertical=222
+2Command Names	222
%nroff^command, argument=222
%nroff^macro, name=222
+2Paragraphs	223
%nroff^paragraph=223
%nroff^.PP macro=223
%Austen, Jane=223
%nroff^.IP macro=223
%nroff^paragraph, indented=223
%nroff^tag on paragraph=223
%nroff^paragraph tag=223
%King Lear=224
%Shakespeare, William=224
%nroff^.RS macro=224
%nroff^.RE macro=224
%nroff^relative indent=224
%nroff^indentation, relative=224
%Bathsheba=224
%King David=224
%Uriah the Hittite=224
%nroff^quoted paragraph=226
%nroff^paragraph, quoted=226
%nroff^.QE macro=226
%nroff^.QS macro=226
%Twain, Mark=226
+2Section Headings	226
%nroff^.SH macro=226
%nroff^section heading=226
%nroff^headings, section=226
%nroff^.NH macro=226
%nroff^numbered heading=226
+2Title Page	227
%nroff^.TL macro=227
%nroff^.AU macro=227
%nroff^.AB macro=227
%nroff^.AE macro=227
%nroff^.AI macro=227
%nroff^.bp primitive=228
+2Headers and Footers	228
%nroff^header=228
%nroff^footer=228
%nroff^title=228
%nroff^LT string=228
%nroff^CT string=228
%nroff^RT string=228
%nroff^strings=228
%nroff^.ds primitive=228
%nroff^%, page number=228
%nroff^page number=228
+2Fonts	229
%nroff^fonts=229
%nroff^boldface=229
%nroff^italic=229
%nroff^Roman=229
+2Special Characters	230
%nroff^characters, special=230
+2Footnotes	230
%nroff^footnote=230
%nroff^.FS macro=230
%nroff^.FE macro=230
%MicroKVETCH Electronic Nag=230
+2Displays and Keeps	230
%nroff^display=230
%nroff^keep=230
%nroff^.DS macro=230
%nroff^.DE macro=230
%nroff^centered display=231
%nroff^display, block-centered=231
%nroff^block-centered display=231
%nroff^display indented=231
%nroff^indented, display=231
%nroff^.CD macro=231
%nroff^centered display=231
%nroff^display, centered=231
%nroff^.BD macro=231
%nroff^block-centered display=231
%nroff^display, block-centered=231
%nroff^.LD macro=231
%nroff^display, left=231
%nroff^left display=231
%nroff^.ID macro=231
%nroff^indented display=231
%nroff^display, indented=231
%Blake, William=231
%Burma Shave=231
%nroff^.KE macro=231
%nroff^.KS macro=231
%nroff^keep=231
+2Other Commands	231
%nroff^.bp primitive=231
%nroff^.sp primitive=231
%nroff^skip lines=231
%nroff^begin page=231
%nroff^new page=231
%nroff^page, break=231
+1Introducing nroff's Primitives	232
+2Page Format	232
%nroff^line, length=232
%nroff^.ll primitive=232
%nroff^.po primitive=232
%nroff^page, offset=232
%nroff^command, line length=232
%nroff^command, page offset=232
%hoop snake=232
%Aunt Patsy=232
%Twain, Mark=232
%nroff^specification=232
%nroff^comments=232
+2Breaks	233
%nroff^break=233
%nroff^margins=233
%nroff^break=233
%nroff^.br primitive=233
%nroff^command, break=233
+2Fill and Adjust Modes	233
%nroff^fill=233
%nroff^justify=233
%nroff^adjust=233
%nroff^command, fill=233
%nroff^.fi primitive=233
%nroff^.nf primitive=233
%nroff^.ad primitive=233
%nroff^.na primitive=233
%nroff^no-fill=233
%nroff^.ad primitive=233
%Boswell, James=233
%Johnson, Samuel=233
%Hume, David=233
%nroff^.ad primitive=234
%nroff^.na primitive=234
%nroff^.fi primitive=234
+2Defining Paragraphs	235
%nroff^paragraph=235
%nroff^break=235
%nroff^.sp primitive=235
+2Centering	235
%nroff^.ce primitive=235
+2Tabs	235
%nroff^tab=235
%nroff^.ta primitive=235
%nroff^.tc primitive=236
+2Page Breaks	236
%nroff^.bp primitive=236
%nroff^.pl primitive=236
%nroff^traps=236
+1Macros and Traps	236
%nroff^macro=236
+2What Is a Macro?	236
%nroff^.br primitive=236
%nroff^.ti primitive=236
%nroff^.sp primitive=236
%Twain, Mark=236
%Adam=236
%peach=236
%almond=236
%cabbage=236
%cauliflower=236
%nroff^.PP macro=237
%Puddnhead Wilson=237
+2Introducing Traps	238
%nroff^.wh primitive=238
%nroff^.hd primitive=238
%nroff^command, when=238
%nroff^.FO macro=238
%nroff^measurement, units=238
+2Headers and Footers	238
%nroff^% number register=238
%nroff^footer=238
%nroff^.tl primitive=238
%nroff^.lt primitive=239
%nroff^command, title length=239
%nroff^traps=239
+2Macro Arguments	239
%nroff^macro, arguments=239
%nroff^macro, definition=239
%nroff^macro, arguments=240
+2Double vs. Single Backslashes	240
+1Designing and Installing Macros	241
+1Strings	243
%nroff^string=243
%nroff^.ds primitive=243
%Aristotle=243
+2Strings Within Strings	244
%nroff^string, within strings=244
+1Number Registers	244
%nroff^register, number=244
%nroff^.nr primitive=244
+2Incrementing and Decrementing	246
%nroff^expression=247
%nroff^units=247
%nroff^measurement, units=247
+2Units of Measurement	247
%nroff^.ll primitive=247
%nroff^measurement=248
%nroff^unit, default=248
%nroff^.po primitive=249
+1Conditional Input	249
%nroff^command, conditional=249
%nroff^conditional input=249
%nroff^.ie primitive=249
%nroff^.el primitive=249
%nroff^measurement, absolute=250
%Bierce, Ambrose=250
%guillotine=251
%gallows=251
+1Environments and Diversions	252
%nroff^.ev primitive=252
%nroff^command, environment=252
%nroff^command, page offset=252
%nroff^.po primitive=252
%Book of Proverbs=252
%nroff^.ls primitive=253
%nroff^command, line space=253
%nroff^stack, environment=253
%Book of Proverbs=253
+1Buffers	254
+2Headers and Footers	255
%nroff^.lt primitive=255
+2More About Fonts	255
%nroff^.ft primitive=255
+2Diversions	256
%nroff^diversion=256
%nroff^.di primitive=256
%Book of Proverbs=256
%nroff^command, divert=256
%nroff^.di primitive=256
%nroff^.da primitive=257
%Bierce, Ambrose=257
%Bierce, Ambrose=257
+2A Footnote Macro	258
%Burton, Sir Richard=258
+1Command Line Options	258
%nroff^/usr/lib/tmac=259
+1For Further Information	259
.pn 260+1
%UUCP^tutorial=261
+0UUCP, Remote Communications Utility	261
+2Contents of This Tutorial	261
+1An Overview of UUCP	261
+2Implementations of UUCP	262
%Taylor, Ian=262
+2Programs	262
%Soviet Union=262
+1Files and Directories	263
troff: /v/doc/coherent/text/uucp.r: 281: cannot find font r 
troff: /v/doc/coherent/text/uucp.r: 281: cannot find font r 
%UUCP^lock files=264
%lck files=264
%lock files=264
%file locking, UUCP=264
+1Attaching a Modem to Your Computer	265
+2Selecting Site and Domain Names	266
+1Set Up a UUCP Site by Hand	266
+2port:  Describe a Serial Port	266
+2dial: Describe a Modem	267
%chat script=267
+2sys: Individual System Configuration	269
%mwcbbs^telephone numbers=270
%Chesson, Greg=271
+1Simplifying a UUCP Configuration With uuinstall	273
+2Invoking uuinstall	273
+2The Port File	274
+2The Dial File	275
+2The sys File	276
+2Modifying an Existing Entry	278
+1Configuring UUCP for Dial-in Access	278
+2Giving a Remote UUCP Site a Login	279
+2Configuring a Spooling Directory for Remote UUCP Access	279
+2Configuring UUCP Files	279
+2One Last, Loose Thread	280
+1Requesting Files From a Remote UUCP System	280
+1Sending Files to a Remote UUCP System	280
+1UUCP Administration	281
+1Networks	281
+2Services	281
%UUCP^domain-name service=281
%UUCP^mail forwarding=281
+2Available Networks	282
%UUCP^UUNET=282
%UUNET=282
%UUCP^UUCP=282
%UUCP^UseNet=282
%UseNet=282
%Internet=282
%UUCP^Internet=282
%TCP/IP=282
%UUCP^TCP/IP=282
+1Debugging UUCP Problems	282
+2Define the Problem Exactly	282
+2Enabling and Disabling Ports	282
%enable=282
%disable=282
%uucico^permissions=282
%chmod=283
+2Stale Requests and Multiple Requests	283
+2Problems With Lock Files	283
+2Enabling Ports, /etc/ttys Problems	283
+2Permission Problems	283
+2UUCP Cannot Find Its Own Files	284
+2Modem Configuration	284
+2The Modem Does Not Respond	285
+2The Modem Responds But Does Not Dial	285
+2The Modem Dials But No Connection Made	285
+2The Modem Dials, Carrier Is Established, Nothing Else Happens	285
+2uulog Shows Lost Packets	286
+2uulog Shows Incorrect Response	286
+2Files Refuse To Be Sent or Cannot Be Received	286
+2File Transfers Fail With imsg Statements	286
+2Files are Being Lost	286
+2Non-COHERENT UUCP Site Problems	286
+1Where to Go From Here	286
.pn 286+1
+0The Lexicon	287
%Lexicon^introduction=287
.pn 288+1
%#=289
+1#	String-ize operator	289
%operator, stringize=289
%string-ize operator=289
%token pasting=290
%operator, token-pasting=290
%token-pasting operator=290
%##=290
+1##	Token-pasting operator	290
%manifest constant=290
%identifier, define as macro=290
%#define=290
+1#define	Define an identifier as a macro	290
%include code conditionally=292
%#elif=292
+1#elif	Include code conditionally	292
%include code conditionally=292
%#else=292
+1#else	Include code conditionally	292
%#endif=292
+1#endif	End conditional inclusion of code	292
%conditional inclusion of code, end=292
%inclusion of code, conditional, end=292
%code, conditional inclusion, end=292
%end conditional inclusion of code=292
%code, include conditionally=292
%include code conditionally=292
%#if=292
+1#if	Include code conditionally	292
%code, include code conditionally=293
%include code conditionally=293
%#ifdef=293
+1#ifdef	Include code conditionally	293
%code, include code conditionally=293
%include code conditionally=293
%#ifndef=293
+1#ifndef	Include code conditionally	293
%preprocessing directive, include source file=293
%source file inclusion=293
%include source file=293
%file, source, include=293
%header, copy into program=293
%copy header into program=293
%#include=293
+1#include	Read another file and include it	293
%line control=294
%preprocessing directive, reset line number=294
%line numbering, reset=294
%reset line number=294
%#line=294
+1#line	Reset line number	294
%#pragma=294
+1#pragma	Perform implementation-specific preprocessing	294
%pragma=294
%Intel Binary Compatibility Standard=294
%Binary Compatibility Standard=294
%macro, undefine=295
%undefine a macro=295
%#undef=295
+1#undef	Undefine a macro	295
%translation, date=295
%date of translation=295
%__DATE__=295
+1__DATE__	Date of translation	295
%__FILE__=296
+1__FILE__	Source file name	296
%source file name=296
%current line within source file=296
%source file, current line=296
%__LINE__=296
+1__LINE__	Current line within a source file	296
%translator, mark conforming=296
%conforming translator, mark=296
%mark a conforming translator=296
%__STDC__=296
+1__STDC__	Mark a conforming translator	296
%source file, time translated=297
%time source file is translated=297
%__TIME__=297
+1__TIME__	Time source file is translated	297
%_exit()=297
+1_exit()	Terminate a program	297
%_getwd()=298
+1_getwd()	Get current working directory name	298
%_tolower()=298
+1_tolower()	Convert characters to lower case	298
%_toupper()=298
+1_toupper()	Convert characters to upper case	298
.pn 299+1
grep: /v/PostScript/bbl/paris_hours/pha.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/a.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/pha.bnd
%a.out.h=300
+1a.out.h	Include all COFF header files	300
%abort()=300
+1abort()	End program immediately	300
%abs()=300
+1abs()	Return the absolute value of an integer	300
%/usr/adm/wtmp=301
%login accounting=301
%ac=301
+1ac	Summarize login accounting information	301
%accept()=301
+1accept()	Accept a connection on a socket	301
%access()=302
+1access()	Check if a file can be accessed in a given mode	302
%uninstd.h=302
%acct()=303
+1acct()	Enable/disable process accounting	303
%acct.h=304
+1acct.h	Format for process-accounting file	304
%process accounting, file format=304
%file format, processing accounting=304
%accton=304
+1accton	Enable/disable process accounting	304
%/usr/adm/acct=304
%acos()=305
+1acos()	Calculate inverse cosine	305
%add_history()=306
+1add_history()	Add a line to history buffer	306
%address=306
+1address	306
%Administering COHERENT=306
+1Administering COHERENT	306
%/usr/lib/mail/aliases=308
%/etc/boottime=308
%/etc/brc=308
%/etc/checklist=308
%/usr/lib/mail/config=308
%/usr/lib/uucp/config=308
%/usr/spool/mlp/controls=308
%/etc/cron=308
%/etc/d_passwd=308
%/usr/lib/uucp/dial=308
%/etc/dialups=308
%/usr/lib/mail/directors=308
%/etc/domain=308
%/etc/drvld.all=308
%$HOME/.forward=309
%/etc/getty=309
%/etc/group=309
%/etc/hosts=309
%/etc/hosts.equiv=309
%/etc/hosts.lpd=309
%/usr/lib/hpd=309
%/etc/inetd.conf=309
%/etc/init=309
%$HOME/.kshrc=309
%$HOME/.lastlogin=309
%/etc/default/login=309
%/usr/adm/loginlog=309
%/etc/logmsg=309
%/usr/lib/lpd=309
%/etc/conf/mdevice=309
%/etc/mnttab=309
%/etc/motd=309
%/etc/mount.all=309
%/etc/conf/mtune=310
%/etc/networks=310
%/etc/nologin=310
%/etc/passwd=310
%/usr/lib/mail/paths=310
%/usr/lib/uucp/port=310
%/etc/profile=310
%$HOME/.profile=310
%/etc/protocols=310
%/usr/bin/ramdisk=310
%/etc/rc=310
%/usr/lib/mail/routers=310
%/etc/conf/sdevice=310
%/etc/serialno=310
%/etc/services=310
%/etc/shadow=310
%/etc/conf/stune=310
%/usr/lib/uucp/sys=310
%/etc/termcap=310
%terminfo=310
%/usr/lib/mail/transports=310
%/etc/trustme=311
%/etc/ttys=311
%/etc/update=311
%/etc/usrtime=311
%/etc/utmp=311
%/etc/uucpname=311
%/etc/default/welcome=311
%/usr/adm/wtmp=311
%alarm()=311
+1alarm()	Set a timer	311
%alias=312
+1alias	Set an alias	312
%aliases=312
+1aliases	File of users' aliases	312
%smail=312
%mailing lists=313
%alignment=314
+1alignment	Alignment or packing of fields within a structure	314
%packing=314
%alloc.h=314
+1alloc.h	Define the allocator	314
%alloca()=314
+1alloca()	Dynamically allocate space on the stack	314
%almanac=315
+1almanac	Print an almanac entry for this date	315
%ANSI=315
+1ANSI	Standards for information	315
%ANSI X3.4-1977=315
%ANSI X3.64-1979=315
%ANSI X3.159-1989=315
%apropos=316
+1apropos	Find manual pages on a given topic	316
%MANPATH=316
%environmental variables^MANPATH=316
%ar=316
+1ar	The librarian/archiver	316
%ARHEAD=317
%ARTAIL=317
%ar.h=317
+1ar.h	Format for archive files	317
%file format^archive file=317
%archive file, format=317
%arcoff.h=318
+1arcoff.h	COFF archive-file header	318
%arena=319
+1arena	319
%argc=319
+1argc	Argument passed to main()	319
%argv=320
+1argv	Argument passed to main()	320
%ARHEAD=320
+1ARHEAD	Append options to beginning of ar command line	320
%array=320
+1array	320
%initialization=320
%aggregate=321
%flexible arrays=321
%ARTAIL=321
+1ARTAIL	Append options to end of ar command line	321
%as=321
+1as	i80386 assembler	321
%ASHEAD=322
%ASTAIL=322
%Binary Compatibility Standard=328
troff: /v/doc/coherent/lx/as: 1294: cannot open file "/v/fwb/coherent/text/opcodes.as"
%ASCII=337
+1ASCII	337
%ANSI X3.4-1977=337
%asctime()=339
+1asctime()	Convert time structure to ASCII string	339
%asfix=340
+1asfix	Convert assembly-language programs into 80386 format	340
%ASHEAD=340
+1ASHEAD	Append options to beginning of as command line	340
%asin()=341
+1asin()	Calculate inverse sine	341
%ASKCC=341
+1ASKCC	Force prompting for CC names	341
%assert()=341
+1assert()	Check assertion at run time	341
%run time, check assertion=341
%assertion, check at run time=341
%check assertion at run time=341
%assert.h=342
+1assert.h	Define assert()	342
%ASTAIL=342
+1ASTAIL	Append options to end of as command line	342
%asy=342
+1asy	Device driver for asynchronous serial lines	342
%/dev/com=342
%COM port=342
%serial port=342
%port^serial, driver=342
%port^COM=342
%card^serial=342
%serial cards=342
%dumb serial cards=342
%intelligent serial cards=342
%driver^serial port=342
%device driver^serial port=342
%/etc/default/async=343
%async=343
%UART=345
%asymkdev=345
+1asymkdev	Create nodes for asynchronous devices	345
%asypatch=346
+1asypatch	Patch a kernel file for an asynchronous configuration	346
%at=346
+1at	Drivers for hard-disk partitions	346
%/dev/at*=346
%/dev/rat*=346
%hard disk^driver, AT=346
%AT hard disk^driver=346
%driver^AT hard disk=346
%device driver^AT hard disk=346
%IDE drives^characteristics=346
%polling^ATSREG=347
%kernel^ATSREG variable=347
%Conner drives=347
%at=347
+1at	Execute commands at given time	347
%atrun=348
%atan()=349
+1atan()	Calculate inverse tangent	349
%atan2()=349
+1atan2()	Calculate inverse tangent	349
%ATclock=349
+1ATclock	Read or set the AT realtime clock	349
%atexit()=350
+1atexit()	Register a function to be called when the program exits	350
%atof()=351
+1atof()	Convert ASCII strings to floating point	351
%atoi()=351
+1atoi()	Convert ASCII strings to integers	351
%atol()=352
+1atol()	Convert ASCII strings to long integers	352
%atrun=352
+1atrun	Execute commands at a preset time	352
%auto=353
+1auto	Note an automatic variable	353
%awk=353
+1awk	Pattern-scanning language	353
.pn 354+1
grep: /v/PostScript/bbl/paris_hours/phb.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/b.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phb.bnd
%backups=355
+1backups	Strategies for backing up COHERENT	355
%backups^strategies=355
%wall=356
%who=356
%fsck=356
%mount.all=356
%sync=357
%reboot=357
%restore^files=357
%file^restoring=357
%floppy tape=358
%QIC-40=358
%QIC-80=358
%bad=359
+1bad	Maintain list of bad blocks	359
%badscan=360
+1badscan	Build bad block list	360
%banner=361
+1banner	Print large letters	361
%basename=361
+1basename	Strip path information from a file name	361
%bc=361
+1bc	Interactive calculator with arbitrary precision	361
%lib.b=361
%library^bc=361
%/usr/lib/lib.b=363
%bcmp()=363
+1bcmp()	Compare two chunks of memory	363
%bcopy()=363
+1bcopy()	Berkeley function to copy memory	363
%bind()=364
+1bind()	Bind a name to a socket	364
%bit=365
+1bit	365
%word=365
%bit-fields=365
+1bit-fields	365
%bit_count()=365
+1bit_count()	Count bits in a bit-mask	365
%bit map=366
+1bit map	366
%block=366
+1block	366
%block-special device=366
%character-special device=366
%boot=366
+1boot	Boot block for hard-disk partition/nine-sector diskette	366
%mboot=366
%master boot program=366
%booting^master boot program=366
%boot.fha=367
+1boot.fha	Boot block for floppy disk	367
%booting=367
+1booting	How booting works	367
%device^boot=367
%boot^device=367
%device^root=367
%root^device=367
%bootstrap=367
%boot^secondary=367
%bootstrap=367
%boot^tertiary=367
%tboot=367
%secondary boot=367
%tertiary boot=367
%/tboot=367
%/autoboot=367
%/coherent=367
%libmisc=368
%fifo()=368
%idle=368
%brc=368
%init=368
%single-user mode=368
%single-user mode=368
%multiuser mode=368
%/etc/rc=368
%update=368
%sync=368
%/etc/update=368
%/etc/utmp=368
%/etc/wtmp=368
%floppy disk^bootable=369
%bootable floppy disk=369
%Norton Utilities=371
%partition table^rearranging=371
%boottime=371
+1boottime	File that holds time system was last booted	371
%brc=371
+1brc	Perform maintenance chores, single-user mode	371
%beep^remove=372
%break=372
+1break	Exit from shell construct	372
%break=372
+1break	Exit from loop or switch statement	372
%brk()=372
+1brk()	Change size of data area	372
%break value, definition=372
%bsearch()=372
+1bsearch()	Search an array	372
%binary search=372
%array, search=372
%search an array=372
%buf.h=374
+1buf.h	Buffer header	374
%buffer=374
+1buffer	374
%build=374
+1build	Install ‘(SCCOHERENT‘P onto a hard disk	374
%builtin=375
+1builtin	Execute a command as a built-in command	375
%byte=375
+1byte	375
%dollar=375
%byte ordering=375
+1byte ordering	Machine-dependent ordering of bytes	375
%bzero()=376
+1bzero()	Initialize memory to NUL	376
.pn 376+1
grep: /v/PostScript/bbl/paris_hours/phc.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/c.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phc.bnd
%c=377
+1c	Print multi-column output	377
%C keywords=377
+1C keywords	377
%entry, former C keyword=377
%readonly, former C keyword=377
%class, C++ keyword=377
%inline, C++ keyword=377
%private, C++ keyword=377
%protected, C++ keyword=377
%public, C++ keyword=377
%C language=378
+1C language	378
%Fiterman, Charles=380
%cpp=380
%C preprocessor=380
+1C preprocessor	380
%__COHERENT__=381
%__MWC__=381
%__IEEE__=381
%__I386__=381
%cabs()=382
+1cabs()	Complex absolute value function	382
%cal=383
+1cal	Print a calendar	383
%calendar=383
+1calendar	Reminder service	383
%calling conventions=384
+1calling conventions	384
grep: /v/PostScript/manual/callingcon.eps: No such file or directory
troff: /v/doc/coherent/lx/calling_c: 133: .rb: cannot open file /v/PostScript/manual/callingcon.eps
%calloc()=385
+1calloc()	Allocate dynamic memory	385
%cancel=386
+1cancel	Cancel a print job	386
%canon.h=387
+1canon.h	Portable layout of binary data	387
%captoinfo=387
+1captoinfo	Convert termcap data to terminfo form	387
%Viduya, Robert=387
%case=387
+1case	Execute commands conditionally according to pattern	387
%case=388
+1case	Introduce entry in switch statement	388
%cast=388
+1cast	388
%cat=388
+1cat	Concatenate the contents of a file to the standard output	388
%caveat utilitor=389
+1caveat utilitor	389
%cc=389
+1cc	C compiler	389
%a.out=391
%ISO namespace, compliance=391
%CCHEAD=393
%CCTAIL=393
%UNIX^compilation environment=393
%LIBPATH=393
%floating-point arithmetic^hardware=393
%cc0=402
+1cc0	402
%cc1=402
+1cc1	402
%cc2=402
+1cc2	402
%cc3=402
+1cc3	402
%CCHEAD=402
+1CCHEAD	Append options to beginning of cc command line	402
%CCTAIL=403
+1CCTAIL	Append options to end of cc command line	403
%cd=403
+1cd	Change directory	403
%CD-ROM=403
+1CD-ROM	COHERENT support for read-only compact disk devices	403
%NEC/Toshiba=405
%CD-ROM^NEC/Toshiba=405
%cdmp=405
+1cdmp	Dump COFF files into a readable form	405
%cdump=405
%cdplayer=405
+1cdplayer	Play audio CDs	405
%/dev/cdrom=405
%Buckaway, Mark=406
%Linux=406
%cdrom.h=406
+1cdrom.h	Definitions for CD-ROM drives	406
%cdu31=406
+1cdu31	Driver for the Sony CD-ROM drives	406
%/dev/cdrom=406
%/dev/rscd0=406
%cdv=406
+1cdv	Interface to CD-ROM devices	406
%CD-ROM=406
%Hilton Chris=407
%cdview=407
+1cdview	Read a file from a CD-ROM	407
%ISO-9660=407
%CD-ROM=407
%ceil()=407
+1ceil()	Set numeric ceiling	407
%cfgetispeed()=408
+1cfgetispeed()	Get terminal input speed	408
%cfgetospeed()=408
+1cfgetospeed()	Get terminal output speed	408
%cfsetispeed()=409
+1cfsetispeed()	Set terminal input speed	409
%cfsetospeed()=409
+1cfsetospeed()	Set terminal output speed	409
%cgrep=410
+1cgrep	Pattern search for C source programs	410
%char=411
+1char	Data type	411
%chase=411
+1chase	Highly amusing video game	411
%chdir()=412
+1chdir()	Change working directory	412
%check=412
+1check	Check file system	412
%checkerr=412
+1checkerr	Check the mail system for errors	412
%postmaster=412
%checklist=413
+1checklist	File systems to check when booting ‘(SCCOHERENT‘P	413
%boottime, check file system=413
%chgrp=413
+1chgrp	Change the group owner of a file	413
%/etc/group=413
%chmod=413
+1chmod	Change the modes of a file	413
%sticky bit=413
%chmod()=414
+1chmod()	Change file-protection modes	414
%chmog=415
+1chmog	Change mode, owner, and group simultaneously	415
%chmod=415
%chown=415
%chgrp=415
%chown=415
+1chown	Change the owner of files	415
%chown()=415
+1chown()	Change ownership of a file	415
%chreq=416
+1chreq	Change priority, lifetime, or printer for a job	416
%chroot=416
+1chroot	Change root directory	416
%chroot()=416
+1chroot()	Change the root directory	416
%chsize()=416
+1chsize()	Change the size of a file	416
%file^change size=416
%file^truncate=416
%file^enlarge=416
%truncate a file=416
%enlarge a file=416
%block^sparse=417
%file^sparse=417
%sparse block=417
%sparse file=417
%kermit, interactive=417
%ckermit=417
+1ckermit	Interactive inter-system communication and file transfer	417
%Columbia University=421
%clear=421
+1clear	Clear the screen	421
%clearerr()=421
+1clearerr()	Present stream status	421
%clist.h=422
+1clist.h	Character-list structures	422
%clock=422
+1clock	Read the system clock	422
%/dev/clock=422
%kernel^system clock=422
%driver^system clock=422
%device driver^system clock=422
%BCD=422
%ATclock=422
%clock()=422
+1clock()	Get processor time	422
%time^measure amount needed to execute program=422
%program^return time needed to execute=422
%close()=423
+1close()	Close a file	423
%closedir()=423
+1closedir()	Close a directory stream	423
%Gwynn, D.=423
%clri=423
+1clri	Clear i-node	423
%i-node^clear/remove=423
%clear an i-node=423
%cmos=424
+1cmos	Device for reading CMOS	424
%CMOS=424
%/dev/cmos=424
%kernel^CMOS=424
%driver^CMOS=424
%device driver^CMOS=424
%cmp=426
+1cmp	Compare bytes of two files	426
%coff.h=426
+1coff.h	Format for COFF objects	426
%Common Object File Format=426
%COFF^definition=426
%protected mode^definition=426
%Gircys, Gintaras R.=427
%coffnlist()=427
+1coffnlist()	Symbol table lookup, COFF format	427
%coh_intro=429
+1coh_intro	Tour the ‘(SCCOHERENT‘P file system	429
%coherent.h=429
+1coherent.h	Miscellaneous useful definitions	429
%COHERENT=429
+1COHERENT	Principles of the ‘(SCCOHERENT‘P System	429
%COHERENT^uninstall=430
%uninstall COHERENT=430
%Microsoft^technical support=431
%bootstap^uninstall=431
%uninstall bootstrap=431
%cohtune=431
+1cohtune	Set a variable within a device driver	431
%Space.c=431
%hai=431
%col=432
+1col	Remove reverse and half-line motions	432
%comm=433
+1comm	Print common lines	433
%commands=433
+1commands	433
%compress=436
+1compress	Compress a file	436
%compression=437
+1compression	Programs used to compress text	437
%.Z=438
%.tar.z=438
%.z=438
%.tar.z=438
%.tgz=438
%.gtz=438
%.taz=438
%con.h=438
+1con.h	Configure device drivers	438
%config=438
+1config	File that configures smail	438
%/usr/lib/mail/config=438
%config=438
%/usr/lib/uucp/config=438
%smtpd=444
%config=445
+1config	File that configures UUCP	445
%/usr/lib/uucp/config=445
%/usr/spool/uucp/.Admin/audit.local=447
%/usr/spool/uucp/.Log=447
%/usr/spool/uucp/.Admin/xferstats=447
%xferstats=447
%connect()=448
+1connect()	Connect to a socket	448
%console=449
+1console	Console device driver	449
%/dev/console=449
%driver^console=449
%device driver^console=449
%ANSI X3.64-1979=449
%ANSI X3.4-1977=449
%DEC VT-100=449
%DEC VT-220=449
%VT-100=449
%VT-220=449
%color^setting on console=451
%console^IBM graphics characters=451
%CON_BEEP_SHIFT=455
%kernel^CON_BEEP_SHIFT=455
%console^beeping=455
%beeping^turn off=455
%SEP_SHIFT=455
%kernel^SEP_SHIFT=455
%type qualifier, not modifiable=455
%not modifiable, type qualifier=455
%const=455
+1const	Qualify an identifier as not modifiable	455
%const.h=455
+1const.h	Declare machine-dependent constants	455
%continue=456
+1continue	Terminate current iteration of shell construct	456
%continue=456
+1continue	Force next iteration of a loop	456
%controls=456
+1controls	Data base for the lp print spooler	456
%conv=459
+1conv	Numeric base converter	459
%core=459
+1core	Format of a core-dump file	459
%core dump file format=459
%file format^core dump=459
%core.h=460
+1core.h	Declare structure of a core file	460
%cos()=460
+1cos()	Calculate cosine	460
%cosh()=461
+1cosh()	Calculate hyperbolic cosine	461
%Lal, Sanjay=461
%cp=462
+1cp	Copy a file	462
%cpdir=462
+1cpdir	Copy directory hierarchy	462
%cpio=463
+1cpio	Archiving/backup utility	463
%cpp=463
+1cpp	C preprocessor	463
%CPPHEAD=464
%CPPTAIL=464
%cpp^error messages=464
%C preprocessor^error messages=464
%error messages^cpp=464
%CPPHEAD=467
+1CPPHEAD	Append options to beginning of cpp command line	467
%CPPTAIL=467
+1CPPTAIL	Append options to end of cpp command line	467
%creat()=467
+1creat()	Create/truncate a file	467
%cron=468
+1cron	Execute commands periodically	468
%crontab=469
+1crontab	Copy a command file into the crontab directory	469
%succotash=470
%crypt=471
+1crypt	Encrypt/decrypt text	471
%crypt()=471
+1crypt()	Encryption using rotor algorithm	471
%ct=472
+1ct	Controlling terminal driver	472
%/dev/tty=472
%terminal, controlling=472
%controlling terminal=472
%driver^controlling terminal=472
%device driver^controlling terminal=472
%ctags=472
+1ctags	Generate tags and refs files for vi editor	472
%Kirkendall, Steve=472
%ctermid()=473
+1ctermid()	Name the terminal device that controls the current process	473
%L_ctermid=473
%ctime()=473
+1ctime()	Convert system time to an ASCII string	473
%ctype.h=474
+1ctype.h	Header file for data tests	474
%cu=475
+1cu	‘(SCUNIX‘P-compatible communications utility	475
%rz=476
%sz=476
%Taylor, Ian=478
%curses.h=478
+1curses.h	Define functions and macros in curses library	478
%cut=479
+1cut	Select portions of each line of its input	479
%cvmail=480
+1cvmail	Convert mail from ‘(SCCOHERENT‘P 3.X format to SV format	480
%CWD=480
+1CWD	Current working directory	480
.pn 480+1
grep: /v/PostScript/bbl/paris_hours/phd2.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/d.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phd2.bnd
%d_passwd=481
+1d_passwd	Give passwords for devices	481
%/etc/d_passwd=481
%remacc=481
%daemon=481
+1daemon	481
%data formats=482
+1data formats	482
%word^definition=482
%iBCS2=482
%data types=482
+1data types	482
%comp_t=483
%flock_t=483
%sig_atomic_t=483
%ptrdiff_t=483
%fpos_t=483
%div_t=483
%ldiv_t=483
%comp_t=483
%cmap_t=483
%ddi_init_t=483
%ddi_start_t=483
%ddi_exit_t=483
%ddi_halt_t=483
%fd_t=483
%lkinfo_t=483
%sleep_t=483
%rwlock_t=483
%cseg_t=483
%mzattr_t=483
%event_t=483
%rlim_t=483
%scsi_work_t=483
%scsi_cmd_t=483
%cseg_t=483
%n_sigset_t=483
%o_sigset_t=483
%frtn_t=483
%vaddr_t=483
%minor_t=483
%major_t=483
%uio_t=484
%date=484
+1date	Print/set the date and time	484
%db=485
+1db	Assembler-level symbolic debugger	485
%db^setting registers=487
%dbm.h=491
+1dbm.h	Header file for DBM routines	491
%dbm_clearerr()=491
+1dbm_clearerr()	Clear an error condition on an NDBM data base	491
%dbm_error(),=491
%ndbm.h,=491
%libgdbm=491
%dbm_close()=491
+1dbm_close()	Close an NDBM data base	491
%dbm_open(),=491
%ndbm.h,=491
%libgdbm=491
%dbm_delete()=492
+1dbm_delete()	Delete records from an NDBM data base	492
%ndbm.h,=492
%libgdbm=492
%dbm_dirfno()=492
+1dbm_dirfno()	Return the file descriptor for an NDBM .dir file	492
%dbm_open(),=492
%dbm_pagfno(),=492
%ndbm.h,=492
%libdgm=492
%dbm_error()=492
+1dbm_error()	Check a NDBM data base for an error	492
%dbm_clearerr(),=492
%ndbm.h,=492
%libgdbm=492
%dbm_fetch()=492
+1dbm_fetch()	Fetch a record from an NDBM data base	492
%dbm_open(),=493
%ndbm.h,=493
%libgdbm=493
%dbm_firstkey()=493
+1dbm_firstkey()	Retrieve the first key from an NDBM data base	493
%dbm_nextkey(),=493
%ndbm.h,=493
%libgdbm=493
%dbm_nextdbm()=493
+1dbm_nextdbm()	Retrieve the next key from an NDBM data base	493
%dbm_firstkey(),=493
%dbm_open(),=493
%ndbm.h,=493
%libgdbm=493
%dbm_open()=493
+1dbm_open()	Open an NDBM data base	493
%dbm_close(),=493
%ndbm.h,=493
%libgdbm=493
%dbm_pagfno()=494
+1dbm_pagfno()	Return the file descriptor for an NDBM .pag file	494
%dbm_dirfno(),=494
%dbm_open(),=494
%ndbm.h,=494
%libdgm=494
%dbm_rdonly()=494
+1dbm_rdonly()	Set an NDBM data base into read-only mode	494
%dbm_open(),=494
%ndbm.h,=494
%libgdbm=494
%dbm_store()=494
+1dbm_store()	Store a record into an NDBM data base	494
%dbm_close(),=494
%dbm_open(),=494
%ndbm.h,=494
%libgdbm=494
%dbmclose()=495
+1dbmclose()	Close a DBM data base	495
%dbm.h,=495
%dbminit(),=495
%libgdbm=495
%dbminit()=495
+1dbminit()	Open a DBM data base	495
%dbm.h,=495
%libgdbm=495
%dc=495
+1dc	Desk calculator	495
%dcheck=496
+1dcheck	Check directory consistency	496
%dd=497
+1dd	Convert the contents of a file	497
%decvax_d()=498
+1decvax_d()	Convert a double from IEEE to DECVAX format	498
%double^convert from IEEE to DECVAX format=498
%decvax_f()=498
+1decvax_f()	Convert a float from IEEE to DECVAX format	498
%float^convert from IEEE to DECVAX format=498
%default=498
+1default	Default label in switch statement	498
%defined=499
+1defined	Perform an action if a macro is defined	499
%deftty.h=499
+1deftty.h	Define default tty settings	499
%delete()=499
+1delete()	Delete a record from a DBM data base	499
%dbm.h,=499
%libgdbm=499
%deroff=499
+1deroff	Remove text formatting control information	499
%detab=500
+1detab	Replace tab characters with spaces	500
%TABSIZE=500
%device drivers=500
+1device drivers	500
%major device number=501
%minor device number=501
%device driver^add a new one=501
%widget=501
%df=503
+1df	Measure free space on disk	503
%dial=503
+1dial	File that tells UUCP how to dial a system	503
%dialups=506
+1dialups	Name every device that may need an additional password	506
%/etc/dialups=506
%diff=506
+1diff	Compare two files	506
%diff3=507
+1diff3	Summarize differences among three files	507
%difftime()=508
+1difftime()	Calculate difference between two times	508
%time^calculate difference between two times=508
%directors=508
+1directors	Describe how to resolve local mail addresses	508
%/usr/lib/mail/directors=508
%directors=508
%postmaster=510
%Postmaster=510
%mailer-daemon=510
%Mailer-Daemon=510
%directory=513
+1directory	513
%dirent.h=513
+1dirent.h	Define directory-related data elements	513
%dirname=513
+1dirname	Extract a directory name	513
%dirs=514
+1dirs	Print the contents of the directory stack	514
%disable=514
+1disable	Disable a port	514
%div()=514
+1div()	Perform integer division	514
%div_t=514
%division, integer=514
%integer division=514
%do=515
+1do	Introduce a loop	515
%domain=515
+1domain	Set your system's mail domain	515
%file, transfer to/from MS-DOS=515
%hard disk, copy MS-DOS files to/from=515
%floppy disk, copy MS-DOS files to/from=515
%MS-DOS^copy files to/from=515
%dos=515
+1dos	Manipulate files on MS-DOS file systems	515
%doscat=517
+1doscat	Concatenate a file on an ‘(SCMS-DOS‘P file system	517
%MS-DOS^concatenate a file=518
%doscp=518
+1doscp	Copy files to/from an ‘(SCMS-DOS‘P file system	518
%MS-DOS^copy files=518
%doscpdir=520
+1doscpdir	Copy a directory to/from an ‘(SCMS-DOS‘P file system	520
%MS-DOS^copy directories=520
%dosdel=521
+1dosdel	Delete a file from an ‘(SCMS-DOS‘P file system	521
%MS-DOS^delete a file from=521
%dosdir=521
+1dosdir	List contents of an ‘(SCMS-DOS‘P directory	521
%MS-DOS^list contents of directories=521
%dosformat=522
+1dosformat	Build an MS-DOS file system	522
%MS-DOS^build file system on a floppy disk=522
%file system^build MS-DOS system on a disk=522
%doslabel=523
+1doslabel	Label an ‘(SCMS-DOS‘P floppy disk	523
%MS-DOS^label a floppy disk=523
%dosls=523
+1dosls	List files on an ‘(SCMS-DOS‘P file system	523
%MS-DOS^list contents=523
%dosmkdir=523
+1dosmkdir	Create a directory in an ‘(SCMS-DOS‘P file system	523
%MS-DOS^make a directory=523
%dosrm=524
+1dosrm	Remove a file from an ‘(SCMS-DOS‘P file system	524
%MS-DOS^remove a file=524
%dosrmdir=524
+1dosrmdir	Remove a directory from an ‘(SCMS-DOS‘P file system	524
%MS-DOS^remove a directory=524
%double=525
+1double	Data type	525
%dpac=525
+1dpac	De-fragment a COHERENT file system	525
%Wright, Randy=525
%drand48()=525
+1drand48()	Return a 48-bit pseudo-random number as a double	525
%boottime, load loadable drivers=526
%drvld.all=526
+1drvld.all	Load loadable drivers at boot time	526
%du=526
+1du	Summarize disk usage	526
%backup files=526
%file^backup=526
%dump=526
+1dump	File-system backup utility	526
%dumpdate=527
+1dumpdate	Print dump dates	527
%dumpdir=527
+1dumpdir	Print the directory of a dump	527
%dumptape.h=528
+1dumptape.h	Define data structures used on dump tapes	528
%dup()=528
+1dup()	Duplicate a file descriptor	528
%dup2()=528
+1dup2()	Duplicate a file descriptor	528
.pn 529+1
grep: /v/PostScript/bbl/paris_hours/phe.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/e.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phe.bnd
%echo=530
+1echo	Repeat/expand an argument	530
%ecvt()=530
+1ecvt()	Convert floating-point numbers to strings	530
%ed=531
+1ed	Interactive line editor	531
%EDITOR=534
+1EDITOR	Name editor to use by default	534
%egrep=534
+1egrep	Extended pattern search	534
%else=536
+1else	Introduce a conditional statement	536
%elvis=536
+1elvis	Clone of Berkeley-standard screen editor	536
%.exrc=538
%color, setting on terminal=539
%regular expression=544
%.exrc=545
%elvis.rc=545
%.exfilerc=545
%.exrc=547
%.exfilerc=547
%elvis.rc=547
%Kirkendall, Steve=548
%elvprsv=548
+1elvprsv	Preserve the modified version of a file after a crash	548
%Kirkendall, Steve=548
%elvrec=548
+1elvrec	Recover the modified version of a file after a crash	548
%Kirkendall, Steve=549
%em87=549
+1em87	Perform/emulate hardware floating-point operations	549
%hardware floating point=549
%floating point^hardware, module=549
%emacs=549
+1emacs	‘(SCCOHERENT‘P screen editor	549
%enable=549
+1enable	Enable a port	549
%endgrent()=550
+1endgrent()	Close group file	550
%endhostent()=550
+1endhostent()	Close file /etc/hosts	550
%endnetent()=550
+1endnetent()	Close network file	550
%endprotoent()=551
+1endprotoent()	Close protocols file	551
%endpwent()=551
+1endpwent()	Close password file	551
%endservent()=551
+1endservent()	Close protocols file	551
%endspent()=551
+1endspent()	Close the shadow-password file	551
%endutent()=551
+1endutent()	Close the login logging file	551
%utmp=552
%enum=552
+1enum	Declare a type and identifiers	552
%ENV=552
+1ENV	File read to set environment	552
%env=552
+1env	Execute a command in an environment	552
%environ=553
+1environ	Process environment	553
%environmental variables=553
+1environmental variables	553
%envp=554
+1envp	Argument passed to main()	554
%end-of-file indicator=554
%file, indicate end of=554
%EOF=554
+1EOF	Indicate end of a file	554
%printer=554
%epson=554
+1epson	Prepare files for Epson printer	554
%erand48()=555
+1erand48()	Return a 48-bit pseudo-random number as a double	555
%errno=555
+1errno	External integer for return of error status	555
%errno.h=556
+1errno.h	Error numbers used by errno()	556
%EPERM=556
%ENOENT=556
%ESRCH=556
%EINTR=556
%EIO=556
%ENXIO=556
%E2BIG=556
%ENOEXEC=556
%EBADF=556
%ECHILD=556
%EAGAIN=556
%ENOMEM=556
%EACCES=556
%EFAULT=556
%ENOTBLK=556
%EBUSY=556
%EEXIST=557
%EXDEV=557
%ENODEV=557
%ENOTDIR=557
%EISDIR=557
%EINVAL=557
%ENFILE=557
%EMFILE=557
%ENOTTY=557
%ETXTBSY=557
%EFBIG=557
%ENOSPC=557
%ESPIPE=557
%EROFS=557
%EMLINK=557
%EPIPE=557
%EDOM=557
%ERANGE=557
%EIDRM=558
%EDEADLK=558
%ENOLCK=558
%ENOSTR=558
%ENODATA=558
%ETIME=558
%ENOSR=558
%ENOPKG=558
%EPROTO=558
%EBADMSG=558
%ENAMETOOLONG=558
%EOVERFLOW=558
%ENOTUNIQ=558
%EBADFD=558
%EREMCHG=558
%ELIBACC=558
%ELIBBAD=558
%ELIBSCN=558
%ELIBMAX=558
%ELIBEXEC=558
%EILSEQ=558
%ENOSYS=558
%ELOOP=558
%EUSERS=558
%ENOTSOCK=558
%EDESTADDRREQ=558
%EMSGSIZE=558
%EPROTOTYPE=558
%ENOPROTOOPT=558
%EPROTONOSUPPORT=558
%ESOCKTNOSUPPORT=558
%EOPNOTSUPP=558
%EPFNOSUPPORT=559
%EAFNOSUPPORT=559
%EADDRINUSE=559
%EADDRNOTAVAIL=559
%ENETDOWN=559
%ENETUNREACH=559
%ENETRESET=559
%ECONNABORTED=559
%ECONNRESET=559
%ENOBUFS=559
%EISCONN=559
%ENOTCONN=559
%ESHUTDOWN=559
%ETIMEDOUT=559
%ECONNREFUSED=559
%EHOSTDOWN=559
%EHOSTUNREACH=559
%EALREADY=559
%EINPROGRESS=559
%ESTALE=559
%eval=559
+1eval	Evaluate arguments	559
%ex=560
+1ex	Berkeley-style line editor	560
%Kirkendall, Steve=560
%exec=560
+1exec	Execute command directly	560
%execl()=560
+1execl()	Execute a load module	560
%execle()=561
+1execle()	Execute a load module	561
%execlp()=561
+1execlp()	Execute a load module	561
%execlpe()=561
+1execlpe()	Execute a load module	561
%execution=562
+1execution	562
%program execution=562
%load-module execution=562
%#!=562
%execv()=563
+1execv()	Execute a load module	563
%execve()=563
+1execve()	Execute a load module	563
%execvp()=564
+1execvp()	Execute a load module	564
%execvpe()=564
+1execvpe()	Execute a load module	564
%exit=565
+1exit	Exit from a shell	565
%exit()=565
+1exit()	Terminate a program gracefully	565
%program^indicate failure=565
%EXIT_FAILURE=565
+1EXIT_FAILURE	Indicate program failed to execute successfully	565
%program^indicate success=565
%EXIT_SUCCESS=565
+1EXIT_SUCCESS	Indicate program executed successfully	565
%exp()=566
+1exp()	Compute exponent	566
%export=566
+1export	Add a shell variable to the environment	566
%expr=567
+1expr	Compute a command-line expression	567
%extern=568
+1extern	Declare storage class	568
.pn 568+1
grep: /v/PostScript/bbl/paris_hours/phf.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/f.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phf.bnd
%fabs()=569
+1fabs()	Compute absolute value	569
%factor=569
+1factor	Factor a number	569
%false=569
+1false	Unconditional failure	569
%fc=569
+1fc	Edit and re-execute one or more previous commands	569
%FCEDIT=570
+1FCEDIT	Editor used by fc command	570
%fclose()=570
+1fclose()	Close a stream	570
%fcntl()=570
+1fcntl()	Control open files	570
%locking^file=570
%file^locking=570
%flock=570
%fcntl.h=570
%fcntl.h=571
+1fcntl.h	Manifest constants for file-handling functions	571
%fcvt()=571
+1fcvt()	Convert floating-point numbers to strings	571
%fd=572
+1fd	Floppy disk driver	572
%/dev/fd=572
%/dev/fha=572
%/dev/fva=572
%/dev/rfd=572
%/dev/rfha=572
%/dev/rfva=572
%floppy disk^driver=572
%driver^floppy disk=572
%device driver^floppy disk=572
%Lal, Sanjay=573
%fd.h=574
+1fd.h	Declare file-descriptor structure	574
%fdformat=575
+1fdformat	Low-level format a floppy disk	575
%fdioctl.h=575
+1fdioctl.h	Control floppy-disk I/O	575
%fdisk=576
+1fdisk	Hard-disk partitioning utility	576
%MS-DOS^on same hard drive as COHERENT=577
%COHERENT^on same hard drive as MS-DOS=577
%fdisk.h=577
+1fdisk.h	Fixed-disk constants and structures	577
%disk^fixed=577
%fdopen()=577
+1fdopen()	Open a stream for standard I/O	577
%feof()=578
+1feof()	Discover stream status	578
%ferror()=579
+1ferror()	Discover stream status	579
%fetch()=580
+1fetch()	Fetch a record from a DBM data base	580
%dbm.h,=580
%dbminit(),=580
%libgdbm=580
%fflush()=580
+1fflush()	Flush output stream's buffer	580
%ffs()=581
+1ffs()	Translate a bit mask into an integer value	581
%fgetc()=581
+1fgetc()	Read character from stream	581
%fgetpos()=582
+1fgetpos()	Get value of file-position indicator	582
%file-position indicator^get value=582
%fgets()=583
+1fgets()	Read line from stream	583
%fgetw()=584
+1fgetw()	Read integer from stream	584
%field=584
+1field	584
%file=585
+1file	The way to access bits	585
%file=585
+1file	Guess a file's type	585
%FILE=586
+1FILE	Descriptor for a file stream	586
%binary files=586
%ASCII file=586
%file=586
%file descriptor=586
+1file descriptor	586
%fileno()=586
+1fileno()	Get file descriptor	586
%file descriptor^get from FILE structure=586
%filsys.h=587
+1filsys.h	Structures and constants for super block	587
%super block=587
%filter=587
+1filter	587
%find=587
+1find	Search for files satisfying a pattern	587
%findmouse=589
+1findmouse	Examine a port to see if a mouse is plugged into it	589
%firstkey()=589
+1firstkey()	Retrieve the first record from a DBM data base	589
%dbm.h,=589
%libgdbm,=589
%nextkey()=589
%fixterm()=589
+1fixterm()	Set the terminal into program mode	589
%float=590
+1float	Data type	590
grep: /v/PostScript/manual/float1.eps: No such file or directory
troff: /v/doc/coherent/lx/float: 52: .rb: cannot open file /v/PostScript/manual/float1.eps
grep: /v/PostScript/manual/float2.eps: No such file or directory
troff: /v/doc/coherent/lx/float: 88: .rb: cannot open file /v/PostScript/manual/float2.eps
grep: /v/PostScript/manual/float3.eps: No such file or directory
troff: /v/doc/coherent/lx/float: 146: .rb: cannot open file /v/PostScript/manual/float3.eps
%BCD format=590
%binary coded decimal=590
%packed decimal=590
grep: /v/PostScript/manual/float4.eps: No such file or directory
troff: /v/doc/coherent/lx/float: 190: .rb: cannot open file /v/PostScript/manual/float4.eps
grep: /v/PostScript/manual/float5.eps: No such file or directory
troff: /v/doc/coherent/lx/float: 211: .rb: cannot open file /v/PostScript/manual/float5.eps
%cpp^floating-point numbers=591
%floating-point numbers^inclusion=591
%_DECVAX=591
%_IEEE=591
%float.h=591
+1float.h	Define constants for floating-point numbers	591
%DBL_DIG=591
%DBL_EPSILON=592
%DBL_MANT_DIG=592
%DBL_MAX=592
%DBL_MAX_EXP=592
%DBL_MAX_10_EXP=592
%DBL_MIN=592
%DBL_MIN_EXP=592
%DBL_MIN_10_EXP=592
%FLT_DIG=592
%FLT_EPSILON=592
%FLT_MANT_DIG=592
%FLT_MAX=592
%FLT_MAX_EXP=592
%FLT_MAX_10_EXP=592
%FLT_MIN=592
%FLT_MIN_EXP=592
%FLT_MIN_10_EXP=592
%FLT_RADIX=592
%FLT_ROUNDS=592
%LDBL_DIG=593
%LDBL_EPSILON=593
%LDBL_MANT_DIG=593
%LDBL_MAX=593
%LDBL_MAX_EXP=593
%LDBL_MAX_10_EXP=593
%LDBL_MIN=593
%LDBL_MIN_EXP=593
%LDBL_MIN_10_EXP=593
%floor()=593
+1floor()	Set a numeric floor	593
%floppy disks=593
+1floppy disks	593
%disk^floppy=593
%MS-DOS^reading floppy=594
%XENIX file system, mounting=595
%MS-DOS^file system, mounting=595
%file system^mounting non-COHERENT=595
%non-COHERENT file system^mounting=595
%interleave=595
%Smith, Fred=596
%fmap=597
+1fmap	Measure fragmentation of the free list	597
%Wright, Randy=597
%fmod()=598
+1fmod()	Calculate modulus for floating-point number	598
%floating-point^modulus=598
%fmt=598
+1fmt	Adjust the length of lines in a file of text	598
%fnkey=598
+1fnkey	Set/print function keys for the console	598
%function keys=598
%keys, function=598
%fnmatch()=599
+1fnmatch()	Match a string with a normal expression	599
%fnmatch.h=599
+1fnmatch.h	Constants used with function fnmatch()	599
%fopen()=599
+1fopen()	Open a stream for standard I/O	599
%stdio.h=600
%FOPEN_MAX=600
%FILE^maximum open at once=600
%for=601
+1for	Execute commands for tokens in list	601
%for=601
+1for	Control a loop	601
%fork()=601
+1fork()	Create a new process	601
%fortune=602
+1fortune	Print randomly selected, hopefully humorous, text	602
%.forward=602
+1.forward	Set a forwarding address for mail	602
%fpathconf()=603
+1fpathconf()	Get a file variable by file descriptor	603
%fperr.h=604
+1fperr.h	Constants used with floating-point exception codes	604
%fprintf()=604
+1fprintf()	Print formatted output into file stream	604
%stream^print formatted text=604
%text^print formatted into stream=604
%print formatted text into stream=604
%fputc()=605
+1fputc()	Write character into file stream	605
%fputs()=605
+1fputs()	Write string into file stream	605
%fputw()=606
+1fputw()	Write an integer into a stream	606
%fread()=606
+1fread()	Read data from file stream	606
%free()=606
+1free()	Return dynamic memory to free memory pool	606
%freemem=607
+1freemem	Device that indicates amount of memory that is free	607
%kernel^free memory=607
%memory, free, estimate=607
%/dev/freemem=607
%driver^read free memory=607
%device driver^read free memory=607
%freopen()=607
+1freopen()	Open file stream for standard I/O	607
%frexp()=608
+1frexp()	Separate fraction and exponent	608
%from=609
+1from	Generate list of numbers, for use in loop	609
%fscanf()=609
+1fscanf()	Format input from a file stream	609
%fsck=610
+1fsck	Check and repair file systems interactively	610
%lost+found=610
%fseek()=614
+1fseek()	Seek on file stream	614
%fsetpos()=615
+1fsetpos()	Set file-position indicator	615
%file-position indicator^set=615
%fstat()=615
+1fstat()	Find attributes of an open file	615
%fstatfs()=616
+1fstatfs()	Get information about a file system	616
%ft=616
+1ft	Floppy-tape driver	616
%/dev/ft=616
%floppy tape^driver=616
%tape, floppy^driver=616
%driver^floppy tape=616
%device driver^floppy tape=616
%Reed-Solomon error correction=616
%error-correction code^Reed-Solomon=616
%ftbad=617
+1ftbad	Manipulate bad-block list on a floppy-tape cartridge	617
%floppy tape^manipulate bad-block list=617
%tape, floppy^manipulate bad-block list=617
%ftell()=618
+1ftell()	Return current position of file pointer	618
%ftime()=618
+1ftime()	Get the current time from the operating system	618
%ftok()=619
+1ftok()	Generate keys for interprocess communication	619
%function=620
+1function	620
%fwrite()=620
+1fwrite()	Write into file stream	620
%fwtable=620
+1fwtable	Build font-width table	620
%font^PostScript=620
%PCL=620
%.usp=620
%.tfm=620
%.afm=620
%AFM=620
.pn 621+1
grep: /v/PostScript/bbl/paris_hours/phg.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/g.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phg.bnd
%gawk=622
+1gawk	Pattern-scanning and -processing language	622
%Aho, Alfred=634
%Kernighan, Brian=634
%Weinberger, Peter=634
%Rubin, Paul=634
%Fenlason, Jay=634
%gcd()=634
+1gcd()	Set variable to greatest common divisor	634
%gcvt()=635
+1gcvt()	Convert floating-point numbers to strings	635
%gdbm.h=635
+1gdbm.h	Header file for GDBM routines	635
%dbm.h,=635
%gdbmerrno.h,=635
%header files,=635
%libgdbm,=635
%ndbm.h=635
%gdbm_close()=636
+1gdbm_close()	Close a GDBM data base	636
%gdbm.h,=636
%gdbm_open(),=636
%libgdbm=636
%gdbm_delete()=636
+1gdbm_delete()	Delete a record from a GDBM data base	636
%gdbm.h,=636
%gdbm_open(),=636
%libgdbm=636
%gdbm_exists()=636
+1gdbm_exists()	Check whether a GDBM data base contains a given record	636
%gdbm.h,=636
%gdbm_open(),=636
%libgdbm=636
%gdbm_fetch()=636
+1gdbm_fetch()	Retrieve a record from a GDBM data base	636
%gdbm.h,=637
%gdbm_open(),=637
%libgdbm=637
%gdbm_firstkey()=637
+1gdbm_firstkey()	Return the first record from a GDBM data base	637
%gdbm.h,=637
%gdbm_nextkey(),=637
%gdbm_open(),=637
%libgdbm=637
%gdbm_nextkey()=637
+1gdbm_nextkey()	Return the next record from a GDBM data base	637
%gdbm.h,=637
%gdbm_firstkey(),=637
%gdbm_open(),=637
%libgdbm=637
%gdbm_open()=638
+1gdbm_open()	Open a GDBM data base	638
%gdbm.h,=639
%gdbmerrno.h,=639
%libgdbm=639
%gdbm_reorganize()=639
+1gdbm_reorganize()	Reorganize a GDBM data base	639
%gdbm.h,=639
%gdbmerrno.h,=639
%gdbm_open(),=639
%libgdbm=639
%gdbm_setopt()=639
+1gdbm_setopt()	Set GDBM options	639
%gdbm.h,=640
%gdbmerrno.h,=640
%gdbm_open(),=640
%libgdbm=640
%gdbm_store()=640
+1gdbm_store()	Add records to a GDBM data base	640
%gdbm.h,=640
%gdbm_open(),=640
%libgdbm=640
%gdbm_strerror()=640
+1gdbm_strerror()	Translate a GDBM error code into text	640
%gdbm.h,=640
%gdbmerrno.h,=640
%libgdbm=640
%gdbm_sync()=640
+1gdbm_sync()	Flush buffered GDBM data into its data base	640
%gdbm.h,=641
%gdbm_close(),=641
%gdbm_open(),=641
%libgdbm=641
%gdbmerrno.h=641
+1gdbmerrno.h	Define error messages used by GDBM routines	641
%gdbm.h,=642
%header files,=642
%libgdba=642
%getc()=642
+1getc()	Read character from file stream	642
%getchar()=643
+1getchar()	Read character from standard input	643
%getcwd()=643
+1getcwd()	Get current working directory name	643
%getdents()=644
+1getdents()	Read directory entries	644
%Gwynn, D.=644
%getdtablesize()=644
+1getdtablesize()	Get the number of files a process can open	644
%getegid()=645
+1getegid()	Get effective group identifier	645
%getenv()=645
+1getenv()	Read environmental variable	645
%geteuid()=645
+1geteuid()	Get effective user identifier	645
%getgid()=646
+1getgid()	Get real group identifier	646
%getgrent()=646
+1getgrent()	Get group file information	646
%getgrgid()=646
+1getgrgid()	Get group file information, by group id	646
%getgrnam()=647
+1getgrnam()	Get group file information, by group name	647
%getgroups()=647
+1getgroups()	Read the supplemental group-access list	647
%supplemental group-access list=647
%gethostbyaddr()=647
+1gethostbyaddr()	Retrieve host information by address	647
%Internet^address=647
%gethostbyname()=648
+1gethostbyname()	Retrieve a host IP address by name	648
%gethostname()=648
+1gethostname()	Get the name of the local host	648
%getlogin()=649
+1getlogin()	Get login name	649
%getmap=649
+1getmap	De-archive Usenet map articles	649
%shar file=649
%getmsg()=649
+1getmsg()	Get the next message from a stream	649
%getnetbyaddr()=651
+1getnetbyaddr()	Get a network entry by address	651
%getnetbyname()=651
+1getnetbyname()	Get a network entry by address	651
%getnetent()=652
+1getnetent()	Fetch a network entry	652
%getopt()=653
+1getopt()	Get option letter from argv	653
%getopts=653
+1getopts	Parse command-line options	653
%OPTIND=653
%OPTARG=653
%getpass()=654
+1getpass()	Get password with prompting	654
%getpeername()=654
+1getpeername()	Get name of connected peer	654
%getpgrp()=655
+1getpgrp()	Get process-group identifier	655
%getpid()=655
+1getpid()	Get process identifier	655
%process^id=655
%getppid()=655
+1getppid()	Get process identifier of parent process	655
%process^id=655
%getprotobyname()=655
+1getprotobyname()	Get protocol entry by protocol name	655
%getprotobynumber()=656
+1getprotobynumber()	Get protocol entry by protocol number	656
%getprotoent()=657
+1getprotoent()	Get protocol entry	657
%getpw()=657
+1getpw()	Search password file	657
%getpwent()=657
+1getpwent()	Get password file information	657
%getpwnam()=659
+1getpwnam()	Get password file information, by name	659
%getpwuid()=659
+1getpwuid()	Get password file information, by id	659
%gets()=660
+1gets()	Read string from standard input	660
%getservbyname()=660
+1getservbyname()	Get a service entry by name	660
%getservbyport()=661
+1getservbyport()	Get a service entry by port number	661
%getservent()=662
+1getservent()	Get a service entry	662
%getsockname()=662
+1getsockname()	Get the name of a socket	662
%getsockopt()=663
+1getsockopt()	Read a socket option	663
%getspent()=663
+1getspent()	Get a shadow-password record	663
%getspnam()=664
+1getspnam()	Get a shadow-password record, by user name	664
%gettimeofday()=664
+1gettimeofday()	Berkeley time function	664
%getty=664
+1getty	Terminal initialization	664
%getuid()=665
+1getuid()	Get real user identifier	665
%getutent()=666
+1getutent()	Read an entry from a login logging file	666
%utmp=666
%getutid()=666
+1getutid()	Find a record in login logging file by login identifier	666
%utmp=666
%getutline()=667
+1getutline()	Find a record in login logging file by device	667
%utmp=667
%getw()=667
+1getw()	Read word from file stream	667
%GMT=667
+1GMT	667
%UTC=667
%gmtime()=668
+1gmtime()	Convert system time to calendar structure	668
%gnucpio=668
+1gnucpio	Archiving/backup utility	668
%Free Software Foundation=672
%goto=672
+1goto	Unconditionally jump within a function	672
%grep=672
+1grep	Pattern search	672
%group=673
+1group	Define groups of users	673
%chmod=674
%chgrp=674
%grp.h=674
+1grp.h	Declare group structure	674
%group structure=674
%structure^group=674
%group=674
%gtar=675
+1gtar	Archiving/backup utility	675
%Free Software Foundation=680
%gtty()=680
+1gtty()	Device-dependent control	680
%guess=680
+1guess	Extraordinarily amusing guessing game	680
%gunzip=680
+1gunzip	GNU utility to uncompress files	680
%GNU=680
%Free Software Foundation=681
%copyleft=681
%gzip=681
+1gzip	GNU utility to compress files	681
%GNU=681
%Lempel-Ziv algorithm=682
%Huffman coding=682
%Free Software Foundation=683
%copyleft=683
.pn 683+1
grep: /v/PostScript/bbl/paris_hours/phh.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/h.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phh.bnd
%hai=684
+1hai	Host adapter‰independent SCSI driver	684
%/dev/sd*=684
%/dev/rsd*=684
%driver^SCSI devices=684
%device driver^SCSI devices=684
%SCSI devices^device driver=684
%kernel variable^HAI154X_BASE=684
%kernel variable^HAI154X_INTR=684
%kernel variable^HAI154X_DMA=684
%HAI154X_BASE=684
%HAI154X_INTR=684
%HAI154X_DMA=684
%kernel variable^HAI154X_XFERSPEED=684
%kernel variable^HAI154X_BUSOFFTIME=684
%kernel variable^HAI154X_BUSONTIME=684
%HAI154X_XFERSPEED=684
%HAI154X_BUSOFFTIME=684
%HAI154X_BUSONTIME=684
%kernel variable^HAISS_TYPE=685
%HAISS_TYPE=685
%kernel variable^HAISS_INTR=685
%HAISS_INTR=685
%HAISS_BASE=685
%kernel variable^HAISS_BASE=685
%HAISS_SLOWDEV=685
%kernel variable^HAISS_SLOWDEV=685
%HAI_TAPE_SPEC=685
%kernel variables^HAI_TAPE_SPEC=685
%HAI_TAPE_CACHE=685
%kernel variables^HAI_TAPE_CACHE=685
%HAI_CDROM_SPEC=686
%kernel variables^HAI_CDROM_SPEC=686
%NEC CDR-74=686
%NEC CDR-84=686
%CD-ROM^NEC CDR-74=686
%CD-ROM^NEC CDR-84=686
%kernel variable^HAI_DISK_SPEC=686
%HAI_DISK_SPEC=686
%Adaptec controller=687
%aha=687
%ss=687
%device driver^aha=687
%device driver^ss=687
%Hilton, Chris=687
%hard disk=687
+1hard disk	687
%hard disk^enable or disable=687
%hard disk^partitioning=687
%fdisk=687
grep: /v/PostScript/manual/harddisk.eps: No such file or directory
troff: /v/doc/coherent/lx/hard_disk: 92: .rb: cannot open file /v/PostScript/manual/harddisk.eps
grep: /v/PostScript/manual/harddisk2.eps: No such file or directory
troff: /v/doc/coherent/lx/hard_disk: 119: .rb: cannot open file /v/PostScript/manual/harddisk2.eps
%badscan=688
%chmod=688
%chown=688
%fsck=688
%mkfs=688
%mount=688
%shutdown=688
%sync=688
%mkfs=688
%mkdir=688
%mount.all=688
%checklist=689
%hard disk^adding another=689
%partition^root, changing size of=689
%root partition^changing size of=689
%Norton Utilities=689
%partition table^rearranging=689
%hash=689
+1hash	Add a command to the shell's hash table	689
%hdioctl.h=689
+1hdioctl.h	Control hard-disk I/O	689
%ide_info=690
%head=691
+1head	Print the beginning of a file	691
%header files=691
+1header files	691
%include file=691
%header files^feature tests=692
%feature tests=692
%_DDI_DKI=692
%DDI/DKI=692
%_KERNEL=692
%_POSIX_SOURCE=693
%_POSIX_C_SOURCE=693
%_STDC_SOURCE=693
%_SUPPRESS_BSD_DEFINITIONS=693
%_SYS3=693
%_SYSV4=693
%help=693
+1help	Print concise description of command	693
%hmon=694
+1hmon	Monitor the COHERENT System	694
%Pulley, Harry C.=696
%HOME=696
+1HOME	User's home directory	696
%hosts=696
+1hosts	Names and addresses of hosts on the local network	696
%/etc/hosts=696
%hosts.equiv=697
+1hosts.equiv	Name equivalent hosts	697
%/etc/hosts.equiv=697
%hosts.lpd=697
+1hosts.lpd	Local system name and domain	697
%/etc/hosts.lpd=697
%hp=697
+1hp	Prepare files for Hewlett-Packard LaserJet printer	697
%printer=697
%hpd=698
+1hpd	Spooler daemon for laser printer	698
%hpr=698
+1hpr	Spool a job for printing on the laser printer	698
%LaserJet printer=698
%printer^laser=698
%hpskip=699
+1hpskip	Abort/restart current job on Hewlett-Packard LaserJet	699
%hypot()=699
+1hypot()	Compute hypotenuse of right triangle	699
grep: /v/PostScript/manual/hypot.eps: No such file or directory
troff: /v/doc/coherent/lx/hypot: 24: .rb: cannot open file /v/PostScript/manual/hypot.eps
%Seidel, Brent=700
.pn 700+1
grep: /v/PostScript/bbl/paris_hours/phi.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/i.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phi.bnd
%i-node=701
+1i-node	‘(SCCOHERENT‘P system file identifier	701
%icheck=701
+1icheck	i-node consistency check	701
%id=701
+1id	Print user and group IDs and names	701
%idbld=702
+1idbld	Reconfigure the COHERENT kernel	702
%/etc/conf/bin/idbld=702
%ideinfo=702
+1ideinfo	Display information of an IDE hard-disk drive	702
%idenable=702
+1idenable	Enable or disable a device driver	702
%idmkcoh=702
%sdevice=703
%/etc/conf/sdevice=703
%idle=703
+1idle	Device that returns system's idle time	703
%system idle time, estimate=703
%/dev/idle=703
%driver^system idle time, estimate=703
%device driver^system idle time, estimate=703
%idmkcoh=704
+1idmkcoh	Build a new kernel	704
%idtune=704
+1idtune	Set a tunable system value	704
%idmkcoh=704
%stune=704
%/etc/conf/stune=704
%mtune=704
%/etc/conf/mtune=704
%ieee_d()=705
+1ieee_d()	Convert a double from DECVAX to IEEE format	705
%double^convert from DECVAX to IEEE format=705
%ieee_f()=705
+1ieee_f()	Convert a float from DECVAX to IEEE format	705
%float^convert from DECVAX to IEEE format=705
%if=705
+1if	Execute a command conditionally	705
%if=706
+1if	Introduce a conditional statement	706
%IFS=706
+1IFS	Characters recognized as white space	706
%index()=706
+1index()	Find a character in a string	706
%inet_addr()=707
+1inet_addr()	Transform an IP address from text to binary	707
%IP dot notation=707
%dot notation=707
%inet_network()=707
+1inet_network()	Transform an IP address from text to an integer	707
%IP dot notation=707
%dot notation=707
%inetd.conf=708
+1inetd.conf	Configure the Internet daemons	708
%infocmp=708
+1infocmp	De-compile a terminfo file	708
%terminfo^de-compile binary=708
%TERMINFO=708
%environmental variables^TERMINFO=708
%Curtis, Pavel=708
%Munk, Udo=708
%init=708
+1init	System initialization	708
%wtmp=708
%/usr/adm/wtmp=708
%console=708
%/dev/console=708
%brc=708
%/etc/brc=708
%fsck=708
%rc=708
%/etc/rc=708
%accton=708
%ttys=709
%/etc/ttys=709
%getty=709
%passwd=709
%/etc/passwd=709
%shadow=709
%/etc/shadow=709
%utmp=709
%/usr/adm/utmp=709
%/etc/profile=709
%profile=709
%SIGQUIT=709
%SIGHUP=709
%SIGKILL=709
%kill=709
%initgroups()=710
+1initgroups()	Initialize the supplementary group-access list	710
%initialization=710
+1initialization	710
%ino.h=712
+1ino.h	Constants and structures for disk i-nodes	712
%inode.h=712
+1inode.h	Constants and structures for memory-resident i-nodes	712
%install=713
+1install	Install a software update onto COHERENT	713
%update=713
%software, preparing releases=713
%software, installing under COHERENT=713
%releases, software, preparing=713
%/etc/install.u=713
%install.u=713
%int=714
+1int	Data type	714
%interrupt=715
+1interrupt	715
%io.h=715
+1io.h	Constants and structures used by I/O	715
%ioctl()=715
+1ioctl()	Device-dependent control	715
%CD-ROM=715
%free memory, read=716
%system^free memory, read=716
%/dev/freemem=716
%idle time, read=716
%system^idle time, read=716
%/dev/idle=716
%Munk, Udo=718
%signal()^example=718
%termio^example=718
%ipc.h=720
+1ipc.h	Definitions for interprocess communications	720
%ipcrm=720
+1ipcrm	Remove an interprocess-communication memory item	720
%ipcs=721
+1ipcs	Display a snapshot of interprocess communications	721
%IRQ=723
+1IRQ	Interrupts on the IBM PC	723
%interrupts=723
%isalnum()=724
+1isalnum()	Check if a character is a number or letter	724
%isalpha()=724
+1isalpha()	Check if a character is a letter	724
%isascii()=724
+1isascii()	Check if a character is an ASCII character	724
%isatty()=725
+1isatty()	Check if a device is a terminal	725
%iscntrl()=725
+1iscntrl()	Check if a character is a control character	725
%isdigit()=725
+1isdigit()	Check if a character is a numeral	725
%character, check if printable=725
%check if character is printable=725
%isgraph()=725
+1isgraph()	Check if a character is printable	725
%islower()=726
+1islower()	Check if a character is a lower-case letter	726
%ispos()=726
+1ispos()	Return if variable is positive or negative	726
%isprint()=726
+1isprint()	Check if a character is printable	726
%ispunct()=726
+1ispunct()	Check if a character is a punctuation mark	726
%isspace()=727
+1isspace()	Check if a character prints white space	727
%isupper()=727
+1isupper()	Check if a character is an upper-case letter	727
%hexadecimal numeral, check if character is=727
%isxdigit()=727
+1isxdigit()	Check if a character is a hexadecimal numeral	727
%itom()=728
+1itom()	Create a multiple-precision integer	728
.pn 728+1
grep: /v/PostScript/bbl/paris_hours/phj.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/j.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phj.bnd
%j0()=729
+1j0()	Compute Bessel function	729
%j1()=729
+1j1()	Compute Bessel function	729
%jn()=730
+1jn()	Compute Bessel function	730
%jobs=730
+1jobs	Print information about jobs	730
%join=730
+1join	Join two data bases	730
%jrand48()=731
+1jrand48()	Return a 48-bit pseudo-random number as a long integer	731
.pn 731+1
grep: /v/PostScript/bbl/paris_hours/phk.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/k.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phk.bnd
%kb.h=732
+1kb.h	Define keys for loadable keyboard driver	732
%kernel=732
+1kernel	Master program of the COHERENT system	732
%kernel^tune=732
%tune the COHERENT kernel=732
%configure the COHERENT kernel=732
%devices.h=733
%keeplist=733
%/etc/conf/install_conf/keeplist=733
%patch=733
%/conf/patch=733
%buffer cache^change size=733
%life, principles of=733
%arena=734
%freelist=734
%keyboard=735
+1keyboard	How COHERENT handles the console keyboard	735
%keyboard^alter driver=735
%kill=735
+1kill	Signal a process	735
%process group=735
%kill()=736
+1kill()	Kill a system process	736
%ksh=736
+1ksh	The Korn shell	736
%Korn shell=736
%shell^Korn=736
%command, definition=736
%token, definition=736
%pipeline, definition=736
%background, execution in=736
%multiprocessing execution=736
%here document=736
%|=736
%pipe=736
%;=737
%&=737
%background process=737
%&&=737
%success, execute upon=737
%||=737
%failure, execute upon=737
%>=737
%redirect standard output=737
%>>=738
%redirect standard output and append=738
%<=738
%redirect standard input=738
%<<=738
%here document=738
%2>=738
%redirect standard error=738
%<&=738
%redirect file stream=738
%>&=738
%duplicate stream=738
%#!=741
%unset=743
%Forsyth, C.=752
%Gisin, E.=752
%Natalie, R.=752
%Robbins, A.=752
%Gwynn, D.=752
%Baalbergen, E.=752
%KSH_VERSION=752
+1KSH_VERSION	List current version of Korn shell	752
%.kshrc=752
+1.kshrc	Set personal environment for Korn shell	752
%ktty.h=753
+1ktty.h	Kernel portion of tty structure	753
.pn 753+1
grep: /v/PostScript/bbl/paris_hours/phl.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/l.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phl.bnd
%l=754
+1l	List directory's contents in long format	754
%l.out.h=754
+1l.out.h	Format for COHERENT 286 objects	754
%l3tol()=754
+1l3tol()	Convert file system block number to long integer	754
%LASTERROR=755
+1LASTERROR	Program that last generated an error	755
%.lastlogin=755
+1.lastlogin	Record of last login	755
%Latin 1=755
+1Latin 1	755
%ISO Latin 1=755
%ISO 8859.1=755
%ISO 646=755
%ASCII=755
%lc=757
+1lc	List directory's contents in columnar format	757
%lcasep=758
+1lcasep	Convert text to lower case	758
%lcong48()=758
+1lcong48()	Initialize values from which 48-bit random numbers are computed	758
%ld=758
+1ld	Link relocatable object modules	758
%COFF^linking=759
%alignment=759
%LISTSEP=760
%LDHEAD=760
%LDTAIL=760
%linker-defined symbols=760
%symbols^linker-defined=760
%__end_text=760
%__end_data=760
%__end_bss=760
%__end=760
%stack^alter size of=762
%ldexp()=762
+1ldexp()	Combine fraction and exponent	762
%LDHEAD=762
+1LDHEAD	Append options to beginning of ld command line	762
%ldiv()=762
+1ldiv()	Perform long integer division	762
%ldiv_t=762
%division, integer=762
%integer division=762
%LDTAIL=763
+1LDTAIL	Append options to end of ld command line	763
%let=763
+1let	Evaluate an expression	763
%lex=763
+1lex	Lexical analyzer generator	763
%libl=763
%library^lex=763
%Lexicon=765
+1Lexicon	Format of the COHERENT manual pages	765
%lf=765
+1lf	List directory's contents in columnar format	765
%libc=765
+1libc	Standard C library	765
%standard C library=765
%library^standard C=765
%random numbers=766
%regular expressions=767
%expression, regular=767
%system calls=768
%libcurses=769
+1libcurses	Library of screen-handling functions	769
%library^curses=769
%curses=769
%LINES=773
%COLS=773
%xterm=773
%xvt=773
%TERMINFO=778
%Curtis, Pavel=781
%Munk, Udo=781
%libedit=781
+1libedit	Routines to gather and edit user input	781
%Turner, Simmule R.=782
%Salz, Rich=782
%libgdbm=782
+1libgdbm	Library for GNU DBM functions	782
%/usr/lib/libgdbm.a=782
%DBM=782
%GDBM=782
%NDBM=782
%Nelson, Philip A.=784
%Gaumond, Pierre=784
%libm=784
+1libm	‘(SCCOHERENT‘P mathematics library	784
%mathematics library=784
%library^mathematics=784
%libmisc=784
+1libmisc	Library of miscellaneous functions	784
%library^miscellaneous functions=784
%alloc()=785
%approx()=785
%ask()=785
%banner()=785
%bedaemon()=785
%daemon^definition=785
%crc16()=785
%fatal()=785
%getline()=785
%Julian date=786
%jday_to_tm()=786
%jday_to_time()=786
%splitter()=786
%is_fs()=786
%lcase()=786
%match()=786
%metaphone()=786
%soundex=786
%Knuth, Donald=786
%newcpy()=786
%pathn()=786
%LIBPATH=786
%regexp()=786
%regexec()=786
%regsub()=786
%regerror()=787
%regexp.h=787
%regular expression=787
%pattern=787
%randl()=787
%replace()=787
%showflag()=787
%skip()=787
%span()=787
%srandl()=787
%strchtr()=787
%strcmpl()=787
%time_to_jday()=787
%tm_to_jday()=787
%trim()=787
%ucase()=787
%usage()=788
%xdump()=788
%xopen()=788
%yn()=788
%vinit()=788
%vshutdown()=788
%vopen()=788
%vfind()=788
%LCK files=788
%lock files=788
%lockit()=789
%lockexist()=789
%locknrm()=789
%lockntty()=789
%lockrm()=789
%locktty()=789
%lockttyexist()=789
%unlockntty()=789
%unlocktty()=789
%unlockit()=789
%picture()=789
%libmp=790
+1libmp	Library for multiple-precision mathematics	790
%library^multiple-precision mathematics=790
%mathematics^multiple-precision=790
%LIBPATH=792
+1LIBPATH	Directories that hold compiler phases and libraries	792
%libraries=793
+1libraries	793
%libsocket=793
+1libsocket	Library of communications routines	793
%Dhuse, John=794
%libterm=799
+1libterm	Functions to read termcap descriptions	799
%library^termcap=799
%limits.h=799
+1limits.h	Define numerical limits	799
%CHAR_BIT=799
%CHAR_MAX=799
%CHAR_MIN=799
%INT_MAX=799
%INT_MIN=799
%LONG_MAX=799
%LONG_MIN=800
%MB_LEN_MAX=800
%OPEN_MAX=800
%SCHAR_MAX=800
%SCHAR_MIND=800
%SHRT_MAX=800
%SHRT_MIN=800
%UCHAR_MAX=800
%UINT_MAX=800
%ULONG_MAX=800
%USHRT_MAX=800
%lines=800
+1lines	Highly amusing board game	800
%link()=801
+1link()	Create a link	801
%listen()=802
+1listen()	Listen for a connection on a socket	802
%lmail=802
+1lmail	Deliver mail on your local system	802
%ln=802
+1ln	Create a link to a file	802
%localtime()=803
+1localtime()	Convert system time to calendar structure	803
%lockf()=804
+1lockf()	Lock a file or a section of a file	804
%locking^file=804
%file^locking=804
%flock=804
%fcntl.h=804
%unistd.h=804
%log()=805
+1log()	Compute natural logarithm	805
%half life=805
%radiation=805
%Lal, Sanjay=805
%log10()=805
+1log10()	Compute common logarithm	805
%login=806
+1login	Log in a user	806
%getty=806
%kibitzers=806
%/etc/default/login=806
%HZ=807
%PATH=807
%SUPATH=807
%TZ=807
%ULIMIT=807
%UMASK=807
%loginlog=808
%/usr/adm/loginlog=808
%failed=808
%/usr/adm/failed=808
%nologin=808
%/etc/nologin=808
%trustme=808
%/etc/trustme=808
%usrtime=808
%/etc/usrtime=808
%passwd=808
%/etc/passwd=808
%shadow=808
%/etc/shadow=808
%dialups=808
%/etc/dialups=808
%d_passwd=808
%/etc/d_passwd=808
%lastlogin=808
%$HOME/.lastlogin=808
%motd=808
%/etc/motd=808
%LOGNAME=808
%profile=808
%/etc/profile=808
%umask=808
%timezone=809
%/etc/timezone=809
%newusr=809
%fortune=809
%Field, Tony=810
%Doering, Uwe=810
%Pulley, Harry=810
%Munk, Udo=810
%login=810
+1login	Set default values for logging in	810
%/etc/default/login=810
%loginlog=810
+1loginlog	Log of failed login attempts	810
%/usr/adm/loginlog=810
%/usr/adm/failed=810
%logmsg=810
+1logmsg	Hold ‘(SCCOHERENT‘P Login Message	810
%login message=810
%beep^remove from login message=810
%bell^remove from login message=810
%LOGNAME=810
+1LOGNAME	Name user's identifier	810
%long=811
+1long	Data type	811
%longjmp()=811
+1longjmp()	Perform a non-local goto	811
%jmp_buf=811
%look=811
+1look	Find matching lines in a sorted file	811
%lp=812
+1lp	Spool a file for printing	812
%printer^job=812
%job=812
%lp=813
+1lp	Driver for parallel ports	813
%/dev/lp=813
%/dev/rlp=813
%parallel port^driver=813
%port^parallel, driver=813
%driver^parallel port=813
%device driver^parallel port=813
%lpadmin=813
+1lpadmin	Administer the lp print-spooler system	813
%lpd=814
+1lpd	Spooler daemon for line printer	814
%lpioctl.h=814
+1lpioctl.h	Definitions for line-printer I/O control	814
%lpr=814
+1lpr	Spool a job for printing on the line printer	814
%printer^line=814
%line printer=814
%lpsched=815
+1lpsched	Print jobs spooled with command lp; turn on printer daemon	815
%lpshut=816
+1lpshut	Turn off the printer daemon despooler	816
%lpskip=816
+1lpskip	Abort/restart current job on line printer	816
%lpstat=817
+1lpstat	Give status of printer or job	817
%lr=817
+1lr	List subdirectories' contents in columnar format	817
%lrand48()=817
+1lrand48()	Return a 48-bit pseudo-random number as a non-negative long integer	817
%ls=818
+1ls	List directory's contents	818
%lseek()=819
+1lseek()	Set read/write position	819
%sparse file=819
%file^sparse=819
%ltol3()=820
+1ltol3()	Convert long integer to file system block number	820
%lvalue=820
+1lvalue	820
%lx=820
+1lx	List directory's contents in columnar format	820
.pn 820+1
grep: /v/PostScript/bbl/paris_hours/phm.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/m.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phm.bnd
%m4=821
+1m4	Macro processor	821
%machine.h=822
+1machine.h	Machine-dependent definitions	822
%macro=823
+1macro	823
%madd()=823
+1madd()	Add multiple-precision integers	823
%mail=823
+1mail	Send or read mail	823
%.signature=824
%mail=825
+1mail	Electronic mail system	825
%mailer=825
%mailbox=825
%.signature=826
%file^mailing=826
%rmail=826
%smail=826
%passwd=826
%.forward=826
%setuid=827
%lmail=827
%uux=827
%Internet=827
%America Online=828
%Applink=828
%ATTMail=828
%BITNET=828
%CompuServe=828
%MCIMail=828
%UUNet=828
%FidoNet=828
%Internet=828
%mailq=829
+1mailq	Display information about spooled mail	829
%main()=829
+1main()	Introduce program's main function	829
%major number=829
+1major number	Device numbering	829
%make=830
+1make	Program-building discipline	830
%makeboot=835
+1makeboot	Create a bootable floppy disk	835
%/etc/mount=835
%makedepend=836
+1makedepend	Generate list of dependencies for a makefile	836
%Brunhoff, Todd=837
%malloc()=837
+1malloc()	Allocate dynamic memory	837
%malloc.h=838
+1malloc.h	Definitions for memory-allocation functions	838
%man=839
+1man	Manual macro package	839
%tmac.an=839
%man=840
+1man	Display Lexicon entries	840
%MANPATH=841
%environmental variables^MANPATH=841
%manual^discrepancies with on-line documentation=841
%manifest constant=841
+1manifest constant	841
%math.h=842
+1math.h	Declare mathematics functions	842
%multibyte character, largest size in locale=842
%largest size of a multibyte character in locale=842
%MB_CUR_MAX=842
+1MB_CUR_MAX	Largest size of a multibyte character in current locale	842
%mboot=842
+1mboot	Master boot block for hard disk	842
%mcd=842
+1mcd	Device driver for Mitsumi CD-ROM drives	842
%/dev/cdrom=842
%/dev/rmcd0=842
%mcmp()=843
+1mcmp()	Compare multiple-precision integers	843
%mcopy()=843
+1mcopy()	Copy a multiple-precision integer	843
%mdevice=843
+1mdevice	Describe drivers that can be linked into kernel	843
%mdiv()=845
+1mdiv()	Divide multiple-precision integers	845
%MicroEMACS=845
%me=845
+1me	‘(SCM‘Picro‘(SCEMACS‘P screen editor	845
%MicroEMACS^f option=849
%MicroEMACS^.emacs.rc=849
%.emacs.rc=849
%TABSIZE=850
%mem=851
+1mem	Physical memory file	851
%/dev/mem=851
%memory manager=851
%kernel^memory manager=851
%driver^memory manager=851
%device driver^memory manager=851
%memccpy()=851
+1memccpy()	Copy a region of memory up to a set character	851
%character, copy=851
%region of memory, copy=851
%memory, copy=851
%copy a region of memory=851
%memchr()=852
+1memchr()	Search a region of memory for a character	852
%region of memory, search for character=852
%character, search for in region of memory=852
%search for character in region of memory=852
%Floyd, Bob=852
%memcmp()=853
+1memcmp()	Compare two regions	853
%regions, compare=853
%compare two regions=853
%memcpy()=853
+1memcpy()	Copy one region of memory into another	853
%character, copy=853
%region of memory, copy=853
%memory, copy=853
%copy a region of memory=853
%memmove()=854
+1memmove()	Copy region of memory into area it overlaps	854
%character, copy=854
%region of memory, copy=854
%memory, copy=854
%copy a region of memory=854
%memok()=855
+1memok()	Test if the arena is corrupted	855
%memset()=855
+1memset()	Fill an area with a character	855
%fill an area with a character=855
%character, fill an area with=855
%mesg=856
+1mesg	Permit/deny messages from other users	856
%min()=856
+1min()	Read multiple-precision integer from stdin	856
%minit()=856
+1minit()	Condition global or auto multiple-precision integer	856
%minor number=857
+1minor number	Device numbering	857
%mintfr()=857
+1mintfr()	Free a multiple-precision integer	857
%mitom()=857
+1mitom()	Reinitialize a multiple-precision integer	857
%mkdbm=857
+1mkdbm	Build a data base for smail	857
%sendmail=857
%Yellow Pages=858
%mkdir=858
+1mkdir	Create a directory	858
%mkdir()=859
+1mkdir()	Create a directory	859
%mkfifo()=859
+1mkfifo()	Create a FIFO	859
%FIFO=859
%mkfnames=859
+1mkfnames	Generate data base of user names	859
%fullnames=860
%/usr/lib/mail/fullnames=860
%mkfs=860
+1mkfs	Make a new file system	860
%mkhpath=862
+1mkhpath	Build a pathalias data base from a hosts table	862
%mkline=863
+1mkline	Fold an alias file, paths file, or mailing list into one-line records	863
%mklost+found=864
+1mklost+found	Make an enlarged lost+found directory	864
%mknod=864
+1mknod	Make a special file or named pipe	864
%mknod()=865
+1mknod()	Create a special file	865
%mkpath=865
+1mkpath	Create a pathalias output file	865
%mksort=866
+1mksort	Sort the standard input, allowing arbitrarily long lines	866
%mktemp()=867
+1mktemp()	Generate a temporary file name	867
%temporary file^generate name=867
%file^generate name for temporary file=867
%name^generate for temporary file=867
%mktime()=867
+1mktime()	Turn broken-down time into calendar time	867
%calendar time^create from broken-down time=867
%MLP_COPIES=868
+1MLP_COPIES	Set default number of copies to print	868
%MLP_FORMLEN=868
+1MLP_FORMLEN	Set default page length	868
%pica=868
%units=868
%MLP_LIFE=869
+1MLP_LIFE	Set default life for print jobs	869
%MLP_PRIORITY=869
+1MLP_PRIORITY	Set default priority for print spooling	869
%MLP_SPOOL=869
+1MLP_SPOOL	Pass user-specific information to print spooler	869
%mmu.h=869
+1mmu.h	Definitions for memory-management unit	869
%mneg()=870
+1mneg()	Negate multiple-precision integer	870
%mnttab=870
+1mnttab	Mount table	870
%mnttab.h=870
+1mnttab.h	Structure for mount table	870
%modem=870
+1modem	870
%modem^adding=870
%adding a modem=870
%remote communication=870
%UUCP=870
%Schofield, Robert=871
%Hayes modem=874
%modem^Hayes=874
%Trailblazer modem=874
%modem^Trailblazer=874
%modf()=874
+1modf()	Separate integral part and fraction	874
%modulus=875
+1modulus	875
%mon.h=876
+1mon.h	Read profile output files	876
%moo=876
+1moo	Greatly amusing numeric guessing game	876
%mastermind=876
%DOOM=876
%more=876
+1more	Display text one page at a time	876
%Nudleman, Mark=878
%motd=878
+1motd	File that holds message of the day	878
%message of the day=878
%mount=878
+1mount	Mount a file system	878
%mount.h=879
+1mount.h	Define the mount table	879
%mount()=879
+1mount()	Mount a file system	879
%mount.all=880
+1mount.all	Mount file systems at boot time	880
%boottime, mount file system=880
%mout()=880
+1mout()	Write multiple-precision integer to stdout	880
%mprec.h=880
+1mprec.h	Multiple-precision arithmetic	880
%mrand48()=880
+1mrand48()	Return a 48-bit pseudo-random number as a long integer	880
%ms=880
+1ms	Manuscript macro package	880
%tmac.s=880
%MS-DOS=882
+1MS-DOS	That other operating system	882
%MS-DOS^equivalent COHERENT commands=882
%MS-DOS^differences from COHERENT=882
%background^running programs in=882
%multi-user, definition=882
%multi-tasking, definition=882
%MS-DOS^version 6.0=886
%Stacker=886
%dos=886
%msg=886
+1msg	Kernel module for messages	886
%kernel^messages=886
%interprocess communication=886
%msg=886
+1msg	Send a brief message to other users	886
%msg.h=887
+1msg.h	Definitions for message facility	887
%msgctl()=887
+1msgctl()	Message control operations	887
%interprocess communication^messages=887
%message passing^msgctl()=887
%msgget()=888
+1msgget()	Create or get a message queue	888
%message passing^msgget()=888
%msgrcv()=891
+1msgrcv()	Receive a message	891
%message passing^msgrcv()=891
%msgs=892
+1msgs	Read messages intended for all ‘(SCCOHERENT‘P users	892
%msgsnd()=893
+1msgsnd()	Send a message	893
%message passing^msgsnd()=893
%msig.h=894
+1msig.h	Machine-dependent signals	894
%msqrt()=894
+1msqrt()	Compute square root of multiple-precision integer	894
%msub()=895
+1msub()	Subtract multiple-precision integers	895
%mtab.h=895
+1mtab.h	Currently mounted file systems	895
%mtioctl.h=895
+1mtioctl.h	Magnetic-tape I/O control	895
%mtoi()=895
+1mtoi()	Convert multiple-precision integer to integer	895
%mtos()=895
+1mtos()	Convert multiple-precision integer to string	895
%mtune=896
+1mtune	Define tunable kernel variables	896
%buffer cache, resize=896
%virtual consoles, set=896
%kernel^tunable variables=896
%tunable variables=896
%mtype()=896
+1mtype()	Return symbolic machine type	896
%mtype.h=897
+1mtype.h	List processor code numbers	897
%mult()=897
+1mult()	Multiply multiple-precision integers	897
%mv=897
+1mv	Rename files or directories	897
%mvdir=897
+1mvdir	Rename a directory	897
%mvfree()=898
+1mvfree()	Free multiple-precision integer	898
%mwcbbs=898
+1mwcbbs	Download files from the Mark Williams bulletin board	898
.pn 900+1
grep: /v/PostScript/bbl/paris_hours/phn.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/n.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phn.bnd
%n.out.h=901
+1n.out.h	Define n.out file structure	901
%label names=901
%tags=901
%structure members=901
%members, structure=901
%ordinary identifiers=901
%name space=901
+1name space	C name-space rules	901
%named pipe=903
+1named pipe	903
%FIFO=903
%nap()=904
+1nap()	Sleep briefly	904
%ncheck=904
+1ncheck	Print file names corresponding to i-node	904
%ndbm.h=904
+1ndbm.h	Header file for NDBM routines	904
%dbm.h,=905
%gdbm.h,=905
%header files,=905
%libgdbm=905
%netdb.h=905
+1netdb.h	Define structures used to describe networks	905
%networks=905
+1networks	Name remote networks	905
%/etc/networks=905
%gateway=905
%Network Information Control Center=905
%NIC^definition=905
%newaliases=905
+1newaliases	Build the smail aliases data base from an ASCII source file	905
%mkaliases=906
%newgrp=906
+1newgrp	Change to a new group	906
%newusr=906
+1newusr	Add new user to ‘(SCCOHERENT‘P system	906
%login identifier=906
%nextkey()=907
+1nextkey()	Retrieve the next record from a DBM data base	907
%dbm.h,=907
%firstkey(),=907
%libgdbm=907
%nm=907
+1nm	Print a program's symbol table	907
%nohup=908
+1nohup	Run a command immune to hangups and quits	908
%nologin=909
+1nologin	Lock out logins	909
%/etc/nologin=909
%rc=909
%/etc/rc=909
%notmem()=909
+1notmem()	Check whether memory is allocated	909
%nptx=909
+1nptx	Generate permutations of users' full names	909
%nrand48()=910
+1nrand48()	Return a 48-bit pseudo-random number as a non-negative long integer	910
%nroff=910
+1nroff	Text-formatting language	910
%pica=914
%point=914
%NUL=919
+1NUL	919
%NULL=920
+1NULL	920
%null=920
+1null	The `bit bucket'	920
%/dev/null=920
%bit bucket=920
%kernel^bit bucket=920
%driver^bit bucket=920
%device driver^bit bucket=920
%nybble=920
+1nybble	920
.pn 920+1
grep: /v/PostScript/bbl/paris_hours/pho.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/o.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/pho.bnd
%object format=921
+1object format	921
%od=921
+1od	Print an octal dump of a file	921
%offsetof()=921
+1offsetof()	Offset of a field within a structure	921
%field, offset within structure=921
%structure, offset of field within=921
%offset of field within structure=921
%alignment=922
%open()=922
+1open()	Open a file	922
%opendir()=924
+1opendir()	Open a directory stream	924
%Gwynn, D.=925
%operator=925
+1operator	925
%operator^precedence=926
%precedence,=926
.pn 926+1
grep: /v/PostScript/bbl/paris_hours/php.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/p.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/php.bnd
%PAGER=927
+1PAGER	Specify Output Filter	927
%param.h=927
+1param.h	Define machine-specific parameters	927
%passwd=927
+1passwd	Set/change login password	927
%passwd=927
+1passwd	Define system users	927
%chown=928
%paste=928
+1paste	Merge lines of files	928
%patch=929
+1patch	Patch a variable or flag within the kernel	929
%panic^redirect output=930
%register dump^redirect output=930
%cmn_err()=930
%console^redirect error messages=930
%error messages^redirect to a terminal=930
%messages^redirect to a terminal=930
%PATH=930
+1PATH	Directories that hold executable files	930
%path()=931
+1path()	Path name for a file	931
%path.h=932
+1path.h	Define/declare constants and functions used with PATH	932
%pathalias=932
+1pathalias	Generate a set of paths among computers	932
%pathconf()=935
+1pathconf()	Get a file variable by path name	935
%pathmerge=936
+1pathmerge	Merge sorted paths files	936
%paths=937
+1paths	Routing data base for mail	937
%pattern=938
+1pattern	938
%regular expression=938
%pause()=938
+1pause()	Wait for signal	938
%pclfont=938
+1pclfont	Prepare a PCL font for downloading via MLP	938
%pclose()=939
+1pclose()	Close a pipe	939
%perror()=939
+1perror()	System call error messages	939
%phone=939
+1phone	Print numbers and addresses from phone directory	939
%third-party vendors^phonebook=940
%software^third party=940
%pipe=940
+1pipe	940
%pipe()=940
+1pipe()	Open a pipe	940
%pnmatch()=942
+1pnmatch()	Match string pattern	942
%pointer=942
+1pointer	942
%initialization of pointers=942
%pointer type=942
%type, pointer=942
%referenced type=942
%type, referenced=942
%pointer type derivation=942
%function, pointer to=942
%null pointer=942
%wild pointer=942
%dereferencing, pointer=942
%pointer dereferencing=942
%pointer-type mismatch=942
%poll()=945
+1poll()	Query several I/O devices	945
%sleep^fraction of a second=946
%poll.h=946
+1poll.h	Define structures/constants used with polling devices	946
%popd=946
+1popd	Pop an item from the directory stack	946
%popen()=946
+1popen()	Open a pipe	946
%port=947
+1port	File that describes ports for UUCP	947
%seven-bit parity=948
%rlogin=949
%portability=949
+1portability	949
%POSIX Standard=950
+1POSIX Standard	950
%ISO=950
%International Standards Organization=950
%IEEE=950
%pow()=950
+1pow()	Raise multiple-precision integer to power	950
%pow()=950
+1pow()	Compute a power of a number	950
%pr=951
+1pr	Paginate and print files	951
%prep=951
+1prep	Produce a word list	951
%print=952
+1print	Echo text onto the standard output	952
%printer=952
+1printer	How to attach and run a printer	952
%soft fonts=952
%font^soft=952
%PCL=952
%PostScript=952
%Epson MX-80=952
%LaserJet=952
%Hewlett-Packard LaserJet=952
%spooling=952
%MLP spooler=952
%lpr=953
%printing^lpr=953
%lpr=953
%hpr=953
%ln=953
%pr=954
%nroff=954
%man=954
%epson=954
%PCL=954
%printing^PCL=954
%hp=954
%PostScript=955
%printing^PostScript=955
%prps=955
%lp=955
%printing^lp=955
%cancel=955
%chreq=955
%lp=955
%lpadmin=955
%lpsched=955
%lpshut=955
%lpstat=955
%reprint=955
%route=955
%MLP_COPIES=956
%MLP_FORMLEN=956
%MLP_LIFE=956
%MLP_PRIORITY=956
%MLP_SPOOL=956
%Magnetic Data Operations=957
%printf()=958
+1printf()	Print formatted text	958
%standard output stream^print formatted text=958
%text^print formatted into stream=958
%proc.h=961
+1proc.h	Define structures/constants used with processes	961
%process=961
+1process	961
%prof=961
+1prof	Print execution profile of a C program	961
%profile=961
+1profile	Set default environment at login	961
%.profile=962
+1.profile	Execute commands at login	962
%Programming COHERENT=962
+1Programming COHERENT	962
%protocols=966
+1protocols	Name communications protocols	966
%/etc/protocols=966
%prps=966
+1prps	Prepare files for PostScript-compatible printer	966
%PostScript=966
%ps=968
+1ps	Print process status	968
%/dev/ps=969
%ps=970
+1ps	Driver to return information about processes	970
%/dev/ps=970
%kernel^process table=970
%process table=970
%driver^process table=970
%device driver^process table=970
%PS1=970
+1PS1	User's default prompt	970
%PS2=970
+1PS2	Prompt when user continues command onto additional lines	970
%PSfont=971
+1PSfont	Cook an Adobe font into PostScript format	971
%Adobe=971
%font^PostScript=971
%PostScript=971
%ptrace()=971
+1ptrace()	Trace process execution	971
%ptrace.h=972
+1ptrace.h	Perform process tracing	972
%pty=972
+1pty	Device driver for pseudoterminals	972
%/dev/ptyp?=972
%pseudoterminal=972
%terminal^pseudo=972
%driver^pseudoterminal=972
%device driver^pseudoterminal=972
grep: /v/PostScript/manual/pty.eps: No such file or directory
troff: /v/doc/coherent/lx/pty: 30: .rb: cannot open file /v/PostScript/manual/pty.eps
%line discipline^definition=972
%pushd=973
+1pushd	Push an item onto the directory stack	973
%putc()=973
+1putc()	Write character into stream	973
%putchar()=974
+1putchar()	Write a character onto the standard output	974
%putenv()=974
+1putenv()	Add a string to the environment	974
%putmsg()=975
+1putmsg()	Place a message onto a stream	975
%putp()=976
+1putp()	Write a string into the standard window	976
%puts()=976
+1puts()	Write string onto standard output	976
%pututline()=977
+1pututline()	Write a record into a logging file	977
%utmp=977
%putw()=977
+1putw()	Write word into stream	977
%pwd=977
+1pwd	Print the name of the current directory	977
%pwd.h=978
+1pwd.h	Define password structure	978
.pn 978+1
grep: /v/PostScript/bbl/paris_hours/ph/paris_hours/phq.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/q.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/ph/paris_hours/phq.bnd
%qfind=979
+1qfind	Quickly find all files with a given name	979
%qpac=979
+1qpac	Map the file system	979
%Wright, Randy=979
%qsort()=979
+1qsort()	Sort arrays in memory	979
%Hoare, C.A.R.=979
%quot=980
+1quot	Summarize file-system usage	980
.pn 981+1
grep: /v/PostScript/bbl/paris_hours/phr.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/r.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phr.bnd
%raise()=982
+1raise()	Let a process send a signal to itself	982
%ram=983
+1ram	Driver for manipulating RAM	983
%/dev/ram?=983
%/dev/ram?close=983
%device driver^RAM=983
%driver^RAM=983
%ram1=983
%ramdisk=984
+1ramdisk	Script to create a RAM-disk	984
%rand()=985
+1rand()	Generate pseudo-random numbers	985
%Lets Make a Deal=985
%Piattelli-Palmarini, Massimo=985
%RAND_MAX=986
+1RAND_MAX	Largest size of a pseudo-random number	986
%random()=986
+1random()	Return a random number	986
%random access=986
+1random access	986
%RAM=986
%ranlib=987
+1ranlib	Create index for object library	987
%rc=987
+1rc	Perform standard maintenance chores	987
%boottime, standard chores=987
%read-only memory=987
+1read-only memory	987
%ROM=987
%read=987
+1read	Assign values to shell variables	987
%read()=988
+1read()	Read from a file	988
%readdir()=988
+1readdir()	Read a directory stream	988
%Gwynn, D.=989
%readline()=989
+1readline()	Read and edit a line of input	989
%Sanderson, David=991
%readonly=991
+1readonly	Mark a shell variable as read only	991
%readonly=991
+1readonly	Storage class	991
%realloc()=991
+1realloc()	Reallocate dynamic memory	991
%reboot=992
+1reboot	Reboot the ‘(SCCOHERENT‘P system	992
%recursion=992
+1recursion	992
%recv()=993
+1recv()	Receive a message from a connected socket	993
%recvfrom()=993
+1recvfrom()	Receive a message from a socket	993
%ref=994
+1ref	Display a C function header	994
%Kirkendall, Steve=994
%regcomp()=994
+1regcomp()	Compile a regular expression into a structure	994
%regerror()=995
+1regerror()	Return an error message from a regular-expression function	995
%regexec()=995
+1regexec()	Compare a string with a regular expression	995
%regexp.h=995
+1regexp.h	Header file for regular-expression functions	995
%regular expression^definition=996
%Spencer, Harry=996
%register=997
+1register	Storage class	997
%register declaration=997
%register variable=997
+1register variable	997
%regsub()=997
+1regsub()	Use regular expression to build a string	997
%remove()=998
+1remove()	Remove a file	998
%file, remove=998
%remove a file=998
%unlink()=998
%rename()=998
+1rename()	Rename a file	998
%file, rename=998
%reprint=999
+1reprint	Reprint a spooled print job	999
%resetterm()=999
+1resetterm()	Reset the terminal to its previous settings	999
%restor=999
+1restor	Restore file system	999
%return=1001
+1return	Return a value and control to calling function	1001
%rev=1001
+1rev	Print text backwards	1001
%Mandrake the Magician=1001
%rewind()=1002
+1rewind()	Reset file pointer	1002
%rewinddir()=1002
+1rewinddir()	Rewind a directory stream	1002
%Gwynn, D.=1002
%rindex()=1002
+1rindex()	Find rightmost occurrence of a character in a string	1002
%rm=1003
+1rm	Remove files	1003
%rmail=1004
+1rmail	Receive mail from remote sites	1004
%uuxqt=1004
%uux=1004
%rmdir=1005
+1rmdir	Remove directories	1005
%rmdir()=1005
+1rmdir()	Remove a directory	1005
%root=1005
+1root	1005
%route=1005
+1route	Show or reset a user's default printer	1005
%routers=1006
+1routers	Rules for resolving mail addresses to remote systems	1006
%/usr/lib/mail/routers=1006
%routers=1006
%/usr/lib/mail/paths=1007
%paths=1007
%rpow()=1008
+1rpow()	Raise multiple-precision integer to power	1008
%RS-232=1008
+1RS-232	Serial port wiring	1008
%modem^cabling=1008
%terminal^cabling=1008
%printer^cabling, serial=1008
%DB-9P connector=1008
%DB-25 connector=1008
%connector^RS-232C=1008
%connector^DB-9P=1008
%connector^serial=1008
%serial port=1008
%port^serial=1008
%pinout^RS-232=1008
%pinout^DB-9P=1008
%DTE=1009
%DCE=1009
%stupidity, IBM=1009
%rsmtp=1009
+1rsmtp	Run batched SMTP mail	1009
%SMTP=1009
%rubik=1010
+1rubik	Play Rubik's cube	1010
%runq=1010
+1runq	Periodically process the mail queue	1010
%rvalue=1010
+1rvalue	1010
.pn 1010+1
grep: /v/PostScript/bbl/paris_hours/phs.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/s.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phs.bnd
%sa=1011
+1sa	Print a summary of process accounting	1011
%savelog=1011
+1savelog	Save a mail log	1011
%sbrk()=1012
+1sbrk()	Increase a program's data space	1012
%scanf()=1013
+1scanf()	Accept and format input	1013
%scat=1015
+1scat	Print text files one screenful at a time	1015
%sched.h=1016
+1sched.h	Define constants used with scheduling	1016
%script=1016
+1script	Capture a terminal session into a file	1016
%terminal^capture session=1016
%log^terminal session=1016
%sdevice=1017
+1sdevice	Configure drivers included within kernel	1017
%sdiv()=1018
+1sdiv()	Divide multiple-precision integers	1018
%SECONDS=1018
+1SECONDS	Number of seconds since current shell started	1018
%security=1018
+1security	1018
%sed=1019
+1sed	Stream editor	1019
%seed48()=1021
+1seed48()	Initialize values from which 48-bit random numbers are computed	1021
%seekdir()=1022
+1seekdir()	Reset the position within a directory stream	1022
%Gwynn, D.=1022
%seg.h=1022
+1seg.h	Definitions used with segmentation	1022
%select()=1022
+1select()	Check if devices are ready for activity	1022
%FD_SETSIZE=1022
%file descriptors^maximum number=1022
%Dhuse, Jon=1023
%FD_ZERO=1023
%FD_SET=1023
%FD_CLR=1024
%FD_ISSET=1024
%sem=1024
+1sem	Kernel module for semaphores	1024
%kernel^semaphores=1024
%interprocess communication=1024
%sem.h=1024
+1sem.h	Definitions used by semaphore facility	1024
%semctl()=1024
+1semctl()	Control semaphore operations	1024
%interprocess communication^semaphores=1024
%semaphores^semctl()=1024
%semget()=1026
+1semget()	Create or get a set of semaphores	1026
%semaphores^semget()=1026
%semop()=1028
+1semop()	Perform semaphore operations	1028
%semaphores^semop()=1028
%send()=1029
+1send()	Send a message to a socket	1029
%sendto()=1030
+1sendto()	Send a message to a socket	1030
%serialno=1031
+1serialno	Hold the serial number of your system	1031
%serial number=1031
%services=1031
+1services	List supported TCP/IP services	1031
%/etc/services=1031
%set=1031
+1set	Set shell option flags and positional parameters	1031
%setbuf()=1033
+1setbuf()	Set alternative stream buffer	1033
%BUFSIZ=1033
%setgid()=1033
+1setgid()	Set group id and user id	1033
%group identifier^definition=1033
%setgrent()=1034
+1setgrent()	Rewind group file	1034
%setgroups()=1034
+1setgroups()	Set the supplemental group-access list	1034
%sethostent()=1034
+1sethostent()	Open and rewind file /etc/hosts	1034
%setjmp()=1035
+1setjmp()	Save machine state for non-local goto	1035
%setjmp.h=1035
+1setjmp.h	Define setjmp() and longjmp()	1035
%jmp_buf=1035
%setbuf()=1035
%setnetent()=1036
+1setnetent()	Open and rewind file /etc/networks	1036
%setpgid()=1036
+1setpgid()	Set the process-group identifier	1036
%setpgrp()=1036
+1setpgrp()	Make a process a process-group leader	1036
%setprotoent()=1037
+1setprotoent()	Open the protocols file	1037
%setpwent()=1037
+1setpwent()	Rewind password file	1037
%setservent()=1037
+1setservent()	Open the services file	1037
%setsid()=1037
+1setsid()	Set session identifier	1037
%setsockopt()=1038
+1setsockopt()	Set a socket option	1038
%setspent()=1039
+1setspent()	Rewind the shadow-password file	1039
%setuid()=1039
+1setuid()	Set user identifier	1039
%user identifier^definition=1039
%setupterm()=1040
+1setupterm()	Initialize a terminal	1040
%setutent()=1040
+1setutent()	Rewind the input stream for a login logging file	1040
%setvbuf()=1040
+1setvbuf()	Set alternative file-stream buffer	1040
%stream^set alternative buffer=1040
%buffer^set alternative for stream=1040
%sgtty=1041
+1sgtty	General terminal interface	1041
%terminal^functions=1041
%terminal^interface=1041
%baud rate^table=1042
%sgtty.h=1045
+1sgtty.h	Definitions used to control terminal I/O	1045
%sh=1045
+1sh	The Bourne shell	1045
%shell^Bourne=1045
%Bourne shell=1045
%command, definition=1045
%token, definition=1045
%pipeline, definition=1045
%background, execution in=1045
%multiprocessing execution=1045
%here document=1045
%|=1045
%pipe=1045
%;=1046
%&=1046
%background=1046
%&&=1046
%execute upon success=1046
%||=1046
%execute upon failure=1046
%>=1046
%redirect standard output=1046
%>>=1046
%redirect standard output and append=1046
%<=1046
%redirect standard input=1046
%<<=1047
%here document=1047
%2>=1047
%redirect standard error=1047
%<&=1047
%redirect file stream=1047
%>&=1047
%duplicate file stream=1047
%<&-=1047
%close standard input=1047
%>&-=1047
%close the standard output=1047
%unset=1050
%shell^library=1053
%/usr/lib/shell_lib.sh=1053
%basename=1053
%shell functions^basename=1053
%file_exists=1053
%shell functions^file_exists=1053
%find_file=1054
%shell functions^find_file=1054
%has_prefix=1054
%shell functions^has_prefix=1054
%is_empty=1054
%shell functions^is_empty=1054
%is_equal=1054
%shell functions^is_equal=1054
%is_numeric=1054
%shell functions^is_numeric=1054
%is_yes=1054
%shell_functions^is_yes=1054
%parent_of=1054
%shell_functions^parent_of=1054
%read_input=1054
%shell functions^read_input=1054
%require_yes_or_no=1054
%shell functions^require_yes_or_no=1054
%source_path=1054
%shell_functions^source_path=1054
%split_path=1054
%shell functions^split_path=1054
%val=1054
%shell functions^val=1054
%#!=1054
%shadow=1056
+1shadow	File that holds restricted passwords	1056
%shadow.h=1056
+1shadow.h	Definitions used with shadow passwords	1056
%SHELL=1057
+1SHELL	Name the default shell	1057
%shellsort()=1057
+1shellsort()	Sort arrays in memory	1057
%Shell, D.L.=1057
%shift=1058
+1shift	Shift positional parameters	1058
%shm=1058
+1shm	Kernel module for shared memory	1058
%kernel^shared memory=1058
%interprocess communication=1058
%shm.h=1058
+1shm.h	Definitions used with shared memory	1058
%shmat()=1058
+1shmat()	Attach a shared-memory segment to a process	1058
%shmctl()=1059
+1shmctl()	Manipulate shared memory	1059
%interprocess communication^shared memory=1059
%shared memory^shmctl()=1059
%shmdt()=1060
+1shmdt()	Detach a shared-memory segment from a process	1060
%shmget()=1060
+1shmget()	Create or get shared-memory segment	1060
%shared memory^shmget()=1060
%shared memory^example=1062
%short=1063
+1short	Data type	1063
%shutdown=1064
+1shutdown	Shut down the ‘(SCCOHERENT‘P system	1064
%single-user mode=1064
%Munk, Udo=1064
%shutdown()=1064
+1shutdown()	Replace function to shut down system	1064
%sigaction()=1065
+1sigaction()	Perform detailed signal management	1065
%sigaddset()=1065
+1sigaddset()	Add a signal to a set of signals	1065
%sigdelset()=1066
+1sigdelset()	Delete a signal from a set	1066
%sigemptyset()=1066
+1sigemptyset()	Initialize a set of signals	1066
%sigfillset()=1066
+1sigfillset()	Initialize a set of signals	1066
%sighold()=1067
+1sighold()	Place a signal on hold	1067
%sigignore()=1067
+1sigignore()	Tell the system to ignore a signal	1067
%sigismember()=1067
+1sigismember()	Check if a signal is a member of a set	1067
%siglongjmp()=1068
+1siglongjmp()	Perform a non-local goto and restore signal mask	1068
%signal()=1068
+1signal()	Specify action to take upon receipt of a given signal	1068
%signal.h=1070
+1signal.h	Define signals	1070
%signame=1070
+1signame	Array of names of signals	1070
%NSIG=1070
%_SIGNAL_MAX=1070
%sigpause()=1070
+1sigpause()	Pause until a given signal is received	1070
%sigpending()=1071
+1sigpending()	Examine signals that are blocked and pending	1071
%sigprocmask()=1071
+1sigprocmask()	Examine or change the signal mask	1071
%sigrelse()=1072
+1sigrelse()	Release a signal for processing	1072
%sigset()=1072
+1sigset()	Specify action to take upon receipt of a given signal	1072
%sigsetjmp()=1073
+1sigsetjmp()	Save machine state and signal mask for non-local jump	1073
%sigsuspend()=1073
+1sigsuspend()	Install a signal mask and suspend process	1073
%sin()=1074
+1sin()	Calculate sine	1074
%Gringauz, Dmitry=1074
%sinh()=1075
+1sinh()	Calculate hyperbolic sine	1075
%size=1075
+1size	Print size of an object file	1075
%sizeof=1075
+1sizeof	Return size of a data element	1075
%sleep=1076
+1sleep	Stop executing for a specified time	1076
%sleep()=1076
+1sleep()	Suspend execution for interval	1076
%Godot=1076
%smail=1076
+1smail	Mail delivery system	1076
%SMTP^definition=1077
%Simple Mail Transfer Protocol=1077
%mailq=1078
%rsmtp=1078
%smtpd=1078
%berkenet=1078
%rmail=1079
%/usr/lib/mail/routers=1080
%routers=1080
%/usr/lib/mail/transports=1080
%transports=1080
%TCP/IP=1080
%runq=1080
%sendmail=1080
%uuxqt=1081
%rmail=1081
%rsmtp=1081
%uux=1081
%routing^definition=1081
%alias resolution=1081
%alias expansion=1081
%RFC 822=1082
%MX record=1083
%/usr/lib/sendmail=1084
%sendmail=1084
%smtpd=1086
+1smtpd	SMTP daemon	1086
%SMTP=1086
%smult()=1086
+1smult()	Multiply multiple-precision integers	1086
%SOCKADDRLEN()=1086
+1SOCKADDRLEN()	Return length of an address	1086
%socket()=1087
+1socket()	Create a socket	1087
%socket.h=1088
+1socket.h	Define constants and structures with sockets	1088
%socketpair()=1088
+1socketpair()	Create a pair of sockets	1088
%sort=1088
+1sort	Sort lines of text	1088
%spac=1089
+1spac	Sort a file system	1089
%Wright, Randy=1089
%spell=1089
+1spell	Find spelling errors	1089
%spelling, looking up a word=1090
%split=1090
+1split	Split a text file into smaller files	1090
%spow()=1091
+1spow()	Raise multiple-precision integer to power	1091
%sprintf()=1091
+1sprintf()	Format output	1091
%sqrt()=1091
+1sqrt()	Compute square root	1091
%Young, Michael B.=1091
%srand()=1092
+1srand()	Seed random number generator	1092
%srand48()=1093
+1srand48()	Seed the 48-bit pseudo-random number routines	1093
%srandom()=1093
+1srandom()	Seed the random-number generator	1093
%srcpath=1093
+1srcpath	Find source files	1093
%sscanf()=1094
+1sscanf()	Format a string	1094
%stack=1094
+1stack	1094
%standard error=1094
+1standard error	1094
%standard input=1095
+1standard input	1095
%standard output=1095
+1standard output	1095
%stat()=1095
+1stat()	Find file attributes	1095
%stat.h=1096
+1stat.h	Definitions and declarations used to obtain file status	1096
%stat=1096
%statfs()=1097
+1statfs()	Get information about a file system	1097
%static=1097
+1static	Declare storage class	1097
%stdarg.h=1098
+1stdarg.h	Header for variable numbers of arguments	1098
%stddef.h=1099
+1stddef.h	Header for standard definitions	1099
%stderr=1099
+1stderr	1099
%stdin=1099
+1stdin	1099
%STDIO=1099
+1STDIO	1099
%stdio.h=1100
+1stdio.h	Declarations and definitions for I/O	1100
%FILE=1100
%type^FILE=1100
%stdin=1100
%stdout=1100
%stderr=1100
%BUFSIZ=1100
%EOF=1100
%FILENAME_MAX=1100
%FOPEN_MAX=1100
%stdlib.h=1100
+1stdlib.h	Declare/define general functions	1100
%stdout=1101
+1stdout	1101
%sticky bit=1101
+1sticky bit	1101
%stime()=1101
+1stime()	Set the time	1101
%storage class=1101
+1storage class	1101
%store()=1102
+1store()	Write a record into a DBM data base	1102
%dbm.h,=1102
%dbminit(),=1102
%libgdbm=1102
%strcasecmp()=1102
+1strcasecmp()	Case-insensitive string comparison	1102
%strcasencmp()=1102
+1strcasencmp()	Case-insensitive string comparison	1102
%strcat()=1102
+1strcat()	Concatenate two strings	1102
%strchr()=1103
+1strchr()	Find a character in a string	1103
%character, search for in string=1103
%search for character in a string=1103
%string, search for character in=1103
%index()=1103
%strcmp()=1103
+1strcmp()	Compare two strings	1103
%strcoll()=1103
+1strcoll()	Compare two strings, using locale-specific information	1103
%string, compare two=1103
%compare two strings=1103
%strcpy()=1104
+1strcpy()	Copy one string into another	1104
%strcspn()=1104
+1strcspn()	Return length a string excludes characters in another	1104
%string, comparison=1104
%compare strings=1104
%strdup()=1104
+1strdup()	Duplicate a string	1104
%stream=1105
+1stream	1105
%stream.h=1105
+1stream.h	Definitions for message facility	1105
%STREAMS=1105
+1STREAMS	COHERENT implementation of STREAMS	1105
%kernel^STREAMS=1105
%strerror()=1105
+1strerror()	Translate an error number into a string	1105
%error message, return text of=1105
%text of error message, return=1105
%strftime()=1106
+1strftime()	Format locale-specific time	1106
%time^format locale specific=1106
%string.h=1107
+1string.h	Declarations for string library	1107
%strings.h=1108
%strings=1108
+1strings	Print all character strings from a file	1108
%strip=1109
+1strip	Strip tables from executable file	1109
%strlen()=1109
+1strlen()	Measure a string	1109
%strncat()=1109
+1strncat()	Append one string onto another	1109
%strncmp()=1109
+1strncmp()	Compare two strings	1109
%strncpy()=1110
+1strncpy()	Copy one string into another	1110
%stropts.h=1111
+1stropts.h	User-level STREAMS routines	1111
%strpbrk()=1111
+1strpbrk()	Find first occurrence of a character from another string	1111
%search string for character=1111
%string, search for character=1111
%character, search string for=1111
%strrchr()=1111
+1strrchr()	Search for rightmost occurrence of a character in a string	1111
%character, search for in string=1111
%search for character in a string=1111
%string, search for character in=1111
%rindex()=1111
%character, reverse search for=1111
%reverse search for character in string=1111
%string, reverse search for character=1111
%strspn()=1112
+1strspn()	Return length a string includes characters in another	1112
%string, comparison=1112
%compare strings=1112
%find one string within another=1112
%string, find one within another=1112
%strstr()=1112
+1strstr()	Find one string within another	1112
%strtod()=1113
+1strtod()	Convert string to floating-point number	1113
%subject sequence=1113
%string, convert to floating-point number=1113
%floating-point number, create from string=1113
%convert string to floating-point number=1113
%strtok()=1114
+1strtok()	Break a string into tokens	1114
%string, break into tokens=1114
%break a string into tokens=1114
%token, break a string into sequence of=1114
%strtol()=1115
+1strtol()	Convert string to long integer	1115
%subject sequence=1115
%string, convert to long integer=1115
%long integer, create from string=1115
%convert string to long integer=1115
%strtoul()=1115
+1strtoul()	Convert string to unsigned long integer	1115
%hashing, example=1115
%linked list, example=1115
%subject sequence=1115
%string, convert to unsigned long integer=1115
%unsigned long integer, create from string=1115
%convert string to unsigned long integer=1115
%struct=1117
+1struct	Data type	1117
%structure=1118
+1structure	1118
%structure assignment=1118
+1structure assignment	1118
%strxfrm()=1118
+1strxfrm()	Transform a string using locale information	1118
%transform a string=1118
%string transformation, locale specific=1118
%locale-specific string transformation=1118
%stty=1119
+1stty	Set/print terminal modes	1119
%cooked devices=1119
%VMIN=1121
%VTIME=1122
%stty()=1123
+1stty()	Set terminal modes	1123
%stune=1123
+1stune	Set values of tunable kernel variables	1123
%su=1124
+1su	Substitute user id, become superuser	1124
%sum=1124
+1sum	Print checksum of a file	1124
%superuser=1124
+1superuser	1124
%swab()=1124
+1swab()	Swap a pair of bytes	1124
%switch=1125
+1switch	Test a variable against a table	1125
%sync=1125
+1sync	Flush system buffers	1125
%sync()=1126
+1sync()	Flush system buffers	1126
%sys=1126
+1sys	Data base for UUCP connections	1126
%SCO UNIX^accessing via UUCP=1128
%UUCP^dialing=1128
%Baker, Steven=1133
%Evans, Doug=1133
%Baker, Steven=1139
%sysconf()=1139
+1sysconf()	Get configurable system variables	1139
%ARG_MAX=1139
%CHILD_MAX=1139
%CLK_TCK=1139
%NGROUPS_MAX=1139
%OPEN_MAX=1140
%PASS_MAX=1140
%_POSIX_JOB_CONTROL=1140
%_POSIX_SAVED_IDS=1140
%_POSIX_VERSION=1140
%sysi86()=1141
+1sysi86()	Identify parts within Intel-based machines	1141
%system()=1142
+1system()	Pass a command to the shell for execution	1142
.pn 1142+1
grep: /v/PostScript/bbl/paris_hours/pht.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/t.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/pht.bnd
%tail=1143
+1tail	Print the end of a file	1143
%tan()=1143
+1tan()	Calculate tangent	1143
%Gringauz, Dmitry=1143
%Fresnel equation=1143
%tanh()=1144
+1tanh()	Calculate hyperbolic cosine	1144
%tape=1144
+1tape	Magnetic-tape devices	1144
%floppy tape=1144
%SCSI tape=1144
%device^cooked interface=1144
%device^raw interface=1144
%cooked interface=1144
%raw interface=1144
%tape=1146
+1tape	Manipulate a tape device	1146
%ftbad=1147
%backup files=1147
%file^backup=1147
%tar=1147
+1tar	Archiving/backup utility	1147
%tboot=1147
+1tboot	Describe the tertiary bootstrap	1147
%tertiary booting=1147
%booting^tertiary=1147
%tcdrain()=1148
+1tcdrain()	Drain output to a device	1148
%tcflow()=1148
+1tcflow()	Control flow on a terminal device	1148
%tcflush()=1149
+1tcflush()	Flush data being exchanged with a terminal	1149
%tcgetattr()=1149
+1tcgetattr()	Get terminal attributes	1149
%tcsendbreak()=1150
+1tcsendbreak()	Send a break to a terminal	1150
%tcsetattr()=1150
+1tcsetattr()	Set terminal attributes	1150
%tee=1151
+1tee	Copy input to multiple output streams	1151
%telldir()=1151
+1telldir()	Return the current position within a directory stream	1151
%Gwynn, D.=1151
%tempnam()=1151
+1tempnam()	Generate a unique name for a temporary file	1151
%temporary file^generate name=1151
%file^generate name for temporary file=1151
%name^generate for temporary file=1151
%TERM=1152
+1TERM	Name the default terminal type	1152
%term=1152
+1term	Format of compiled terminfo file	1152
%terminfo^file format=1152
%termcap=1153
+1termcap	Terminal-description language	1153
%libterm=1153
%library^termcap=1153
%libterm.a=1157
%library^terminal operations=1157
%ospeed=1157
%PC=1157
%terminal=1160
+1terminal	1160
%terminal^adding=1160
%adding a terminal=1160
%computer^connecting via serial port=1160
%null modem=1161
%terminal^raw=1162
%terminal^cooked=1162
%raw terminal=1162
%cooked terminal=1162
%Hough, Dave=1162
%terminfo=1162
+1terminfo	Terminal-description language	1162
%curses.h=1170
%term.h=1170
%terminfo.h=1170
%Curtis, Pavel=1170
%Munk, Udo=1170
%termio=1171
+1termio	General terminal interface	1171
%terminal^functions=1171
%terminal^interface=1171
%VMIN=1174
%VTIME=1174
%termio^delay settings=1176
%termio.h=1176
+1termio.h	Definitions used with terminal input and output	1176
%termios=1176
+1termios	POSIX extended terminal interface	1176
%termios.h=1178
+1termios.h	Definitions used with POSIX extended terminal interface	1178
%termios=1178
%test=1178
+1test	Evaluate conditional expression	1178
%tgetent()=1180
+1tgetent()	Read termcap entry	1180
%tgetflag()=1180
+1tgetflag()	Get termcap Boolean entry	1180
%tgetnum()=1181
+1tgetnum()	Get termcap numeric feature	1181
%tgetstr()=1181
+1tgetstr()	Get termcap string entry	1181
%tgoto()=1181
+1tgoto()	Read/interpret termcap cursor-addressing string	1181
%tic=1182
+1tic	Compile a terminfo description	1182
%terminfo^compile source file=1182
%Curtis, Pavel=1182
%Munk, Udo=1182
%time=1182
+1time	1182
%time_t=1182
%tm=1182
%TIMEZONE=1183
%tzset()=1183
%timezone=1183
%tzname=1183
%Gregorian calendar=1183
%Julian date=1183
%British Empire=1183
%time=1184
+1time	Time the execution of a command	1184
%time.h=1184
+1time.h	Give time-description structure	1184
%time()=1184
+1time()	Get current system time	1184
%time_t=1184
%timeb.h=1185
+1timeb.h	Define timeb structure	1185
%timeout.h=1185
+1timeout.h	Define the timer queue	1185
%times=1185
+1times	Print total user and system times	1185
%times.h=1185
+1times.h	Definitions used with times() system call	1185
%times()=1185
+1times()	Obtain process execution times	1185
%system startup^time since=1186
%TIMEZONE=1186
+1TIMEZONE	Time zone information	1186
%/etc/default/login=1187
%login=1187
%TZ=1187
%astrology=1187
%Doane, Doris Chase=1187
%TMPDIR=1187
+1TMPDIR	Directory that holds temporary files	1187
%tmpfile()=1187
+1tmpfile()	Create a temporary file	1187
%file^create a temporary file=1187
%temporary file^create=1187
%tmpnam()=1190
+1tmpnam()	Generate a unique name for a temporary file	1190
%temporary file^generate name=1190
%file^generate name for temporary file=1190
%name^generate for temporary file=1190
%toascii()=1191
+1toascii()	Convert characters to ASCII	1191
%tolower()=1191
+1tolower()	Convert characters to lower case	1191
%touch=1192
+1touch	Update modification time of a file	1192
%toupper()=1192
+1toupper()	Convert characters to upper case	1192
%tparm()=1192
+1tparm()	Output a parameterized string	1192
%tputs()=1193
+1tputs()	Read/decode leading padding information	1193
%tr=1193
+1tr	Translate characters	1193
%tr=1194
+1tr	Driver to read stored error messages	1194
%/dev/trace=1194
%driver^kernel traceback=1194
%device driver^kernel traceback=1194
%kernel^traceback, driver=1194
%transports=1194
+1transports	Describe mail transportation systems	1194
%/usr/lib/mail/transports=1194
%transports=1194
%mailx=1196
%Mail=1196
%trap=1199
+1trap	Execute command on receipt of signal	1199
%trigraph=1200
+1trigraph	1200
%ISO 646=1200
%??==1200
%??(=1200
%??/=1200
%??)=1200
%??'=1200
%??<=1200
%??!=1200
%??>=1200
%??-=1200
%troff=1201
+1troff	Extended text-formatting language	1201
%PCL=1204
%Printer Control Language=1204
%PostScript=1204
%font^PCL=1204
%font^PostScript=1206
%true=1209
+1true	Unconditional success	1209
%trustme=1209
+1trustme	List of trusted users	1209
%/etc/trustme=1209
%tsort=1209
+1tsort	Topological sort	1209
%ttt=1210
+1ttt	Play 3-D tic-tac-toe	1210
%tty=1210
+1tty	Print the user's terminal name	1210
%tty.h=1210
+1tty.h	Define flags used with tty processing	1210
%ttyname()=1210
+1ttyname()	Identify a terminal	1210
%ttys=1210
+1ttys	Describe terminal ports	1210
%/etc/ttys=1210
%ttyslot()=1212
+1ttyslot()	Return a terminal's line number	1212
%ttystat=1212
+1ttystat	Get terminal status	1212
%ttytype=1213
+1ttytype	Select a default terminal type for a port	1213
%type checking=1213
+1type checking	1213
%type promotion=1213
+1type promotion	1213
%typedef=1214
+1typedef	Define a new data type	1214
%types.h=1214
+1types.h	Define system-specific data types	1214
%typeset=1214
+1typeset	Set/list variables and their attributes	1214
%typo=1214
+1typo	Detect possible typographical and spelling errors	1214
%tzset()=1215
+1tzset()	Set the local time zone	1215
%settz()=1215
.pn 1215+1
grep: /v/PostScript/bbl/paris_hours/phu.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/u.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phu.bnd
%ulimit()=1216
+1ulimit()	Get/set limits for a process	1216
%ulimit.h=1216
+1ulimit.h	Define manifest constants used by system call ulimit()	1216
%ulimit()=1216
%umask=1216
+1umask	Set the file-creation mask	1216
%file-creation mask=1216
%mask, default=1216
%permissions^changing default=1216
%umask()=1217
+1umask()	Set file-creation mask	1217
%umount=1217
+1umount	Unmount file system	1217
%umount()=1218
+1umount()	Unmount a file system	1218
%unalias=1218
+1unalias	Remove an alias	1218
%uname=1218
+1uname	Print information about ‘(SCCOHERENT‘P	1218
%Schubert, Cy=1218
%uname()=1219
+1uname()	Get the name and version of ‘(SCCOHERENT‘P	1219
%version^COHERENT=1219
%COHERENT^find version number=1219
%uncompress=1219
+1uncompress	Uncompress a compressed file	1219
%unctrl.h=1219
+1unctrl.h	Define macro unctrl()	1219
%unctrl()=1219
%ungetc()=1220
+1ungetc()	Return character to input stream	1220
%union=1220
+1union	Multiply declare a variable	1220
%uniq=1220
+1uniq	Remove/count repeated lines in a sorted file	1220
%unistd.h=1221
+1unistd.h	Define constants for file-handling routines	1221
%units=1221
+1units	Convert measurements	1221
%unlink()=1222
+1unlink()	Remove a file	1222
%unpack=1222
+1unpack	GNU utility to uncompress files	1222
%GNU=1222
%unset=1223
+1unset	Unset an environment variable or shell function	1223
%unsigned=1223
+1unsigned	Data type	1223
%until=1223
+1until	Execute commands repeatedly	1223
%unzip=1223
+1unzip	Un-zip a zipped archive	1223
%upac=1224
+1upac	De-fragment a file system without sorting	1224
%Wright, Randy=1224
%update=1224
+1update	Update file systems periodically	1224
%uproc.h=1225
+1uproc.h	Definitions used with user processes	1225
%USER=1225
+1USER	Name user's identifier	1225
%Using COHERENT=1225
+1Using COHERENT	1225
%usleep()=1226
+1usleep()	Sleep briefly	1226
%usrtime=1226
+1usrtime	Times each user is permitted to log in	1226
%/etc/usrtime=1226
%ustat()=1228
+1ustat()	Get statistics on a file system	1228
%utime()=1228
+1utime()	Change file access and modification times	1228
%utime.h=1229
+1utime.h	Declare system call utime()	1229
%utmp=1229
+1utmp	File that notes login events that are active	1229
%/etc/utmp=1229
%utmp.h=1229
+1utmp.h	Login accounting information	1229
%utmp=1229
%/etc/utmp=1229
%wtmp=1229
%/usr/adm/wtmp=1229
%utmpname()=1230
+1utmpname()	Manipulate a login logging file other than /etc/utmp	1230
%utsname.h=1231
+1utsname.h	Define utsname structure	1231
%uuchk=1231
+1uuchk	Check UUCP configuation	1231
%uucico=1232
+1uucico	Communicate with a remote site	1232
%UUCP^logging data, file=1233
%/usr/spool/uucp/.Admin/audit.local=1233
%Taylor Ian Lance=1233
%uuconv=1233
+1uuconv	Convert UUCP configuration files to Taylor format	1233
%Taylor, Ian Lance=1234
%UUCP=1234
+1UUCP	Unattended communication with remote systems	1234
%/usr/lib/uucp=1234
%/usr/lib/uucp/config=1234
%config=1234
%/usr/lib/uucp/dial=1235
%dial=1235
%/usr/lib/uucp/port=1235
%port=1235
%/usr/spool/uucp=1235
%=1235
%.Admin=1235
%/usr/spool/uucp/.Admin/xferstats=1235
%xferstats=1235
%/usr/spool/uucp/.Admin/audit.local=1235
%audit.local=1235
%/usr/lib/uucp/.Log=1235
%.Log=1235
%/usr/spool/uucp/.Received=1235
%.Received=1235
%/usr/spool/uucp/.Sequence=1235
%.Sequence=1235
%/usr/spool/uucp/.Status=1235
%.Status=1235
%/usr/spool/uucp/.Temp=1235
%.Temp=1235
%/usr/spool/uucp/.Xqtdir=1235
%.Xqtdir=1235
%lock files=1236
%LCK..=1236
%chmod=1237
%Taylor, Ian Lance=1237
%Chalmers, Robert=1237
%uucp=1237
+1uucp	Spool files for transmission to other systems	1237
%uucp^logging file=1238
%/usr/spool/uucp/.Admin/audit.local=1238
%Taylor, Ian Lance=1239
%name of system=1239
%system name=1239
%uucpname=1239
+1uucpname	Set the system's ‘(SCUUCP‘P name	1239
%uudecode=1240
+1uudecode	Decode a binary file sent from a remote system	1240
%uuencode=1240
+1uuencode	Encode a binary file for transmission	1240
%uuinstall=1241
+1uuinstall	Install or modify ‘(SCUUCP‘P	1241
%uulog=1241
+1uulog	Read a ‘(SCUUCP‘P log	1241
%uumkdir=1242
+1uumkdir	Create UUCP directories	1242
%uumvlog=1242
+1uumvlog	Archive ‘(SCUUCP‘P log files	1242
%uuname=1242
+1uuname	List ‘(SCUUCP‘P names of known systems	1242
%uupick=1242
+1uupick	Pick up a file uploaded from a remote system	1242
%Taylor, Ian Lance=1243
%uurmlock=1243
+1uurmlock	Remove ‘(SCUUCP‘P lock files	1243
%UUCP^lock files=1243
%LCK files=1243
%lock files=1243
%file locking, UUCP=1243
%uusched=1244
+1uusched	Call all systems that have jobs waiting for them	1244
%uustat=1244
+1uustat	UUCP status inquiry and control	1244
%Taylor, Ian Lance=1246
%uuto=1246
+1uuto	Send a file to a remote system	1246
%uutouch=1246
+1uutouch	Touch a file to trigger ‘(SCUUCP‘P poll	1246
%uutry=1247
+1uutry	Debugging script for ‘(SCUUCP‘P	1247
%uux=1247
+1uux	Execute a command on a remote system	1247
%.Xqtdir=1249
%/usr/spool/uucp/.Xqtdir=1249
%Taylor, Ian Lance=1250
%uuxqt=1250
+1uuxqt	Execute commands requested by a remote system	1250
%Taylor, Ian Lance=1250
.pn 1250+1
grep: /v/PostScript/bbl/paris_hours/phv.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/v.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phv.bnd
%va_arg()=1251
+1va_arg()	Return pointer to next argument in argument list	1251
%va_end()=1251
+1va_end()	Tidy up after traversal of argument list	1251
%va_start()=1252
+1va_start()	Point to beginning of argument list	1252
%va_list=1252
%varargs.h=1252
+1varargs.h	Declare/define routines for variable arguments	1252
%varargs.h^vs. stdarg.h=1252
%stdarg.h^vs. varargs.h=1252
%vfprintf()=1253
+1vfprintf()	Print formatted text into stream	1253
%stream^print formatted text=1253
%text^print formatted into stream=1253
%print formatted text into stream=1253
%vi=1253
+1vi	Clone of Berkeley-style screen editor	1253
%Kirkendall, Steve=1253
%vidattr()=1254
+1vidattr()	Set the terminal's video attributes	1254
%vidputs()=1254
+1vidputs()	Write video attributes into a function	1254
%view=1254
+1view	Screen-oriented viewing utility	1254
%Kirkendall, Steve=1254
%virtual console=1254
+1virtual console	‘(SCCOHERENT‘P system of multiple virtual consoles	1254
%console^virtual=1254
%terminal^virtual=1254
%/dev/console=1255
%/dev/vcolor=1255
%/dev/vmono=1255
%/dev/mono=1255
%/dev/color=1255
%VTVGA=1255
%VTMONO=1255
%/etc/ttys=1255
%void=1256
+1void	Data type	1256
%volatile=1256
+1volatile	Qualify an identifier as frequently changing	1256
%vprintf()=1257
+1vprintf()	Print formatted text into standard output stream	1257
%standard output stream^print formatted text=1257
%text^print formatted into stream=1257
%vsh=1257
+1vsh	Interactive graphical shell	1257
%shell^visual=1257
grep: /v/PostScript/manual/vsh.eps: No such file or directory
troff: /v/doc/coherent/lx/vsh: 97: .rb: cannot open file /v/PostScript/manual/vsh.eps
%VT-100=1272
%Munk, Udo=1272
grep: /v/PostScript/manual/udo.eps: No such file or directory
troff: /v/doc/coherent/lx/vsh: 1986: .rb: cannot open file /v/PostScript/manual/udo.eps
%vsprintf()=1272
+1vsprintf()	Print formatted text into string	1272
%string^print formatted text=1272
%text^print formatted into string=1272
%vtkb=1273
+1vtkb	Non-configurable keyboard driver, virtual consoles	1273
%/dev/console=1273
%keyboard^non-configurable driver=1273
%virtual consoles^non-configurable keyboard driver=1273
%driver^virtual consoles, non-configurable keyboard=1273
%device driver^virtual consoles, non-configurable keyboard=1273
%vtnkb=1273
+1vtnkb	Configurable keyboard driver, virtual consoles	1273
%/dev/console=1273
%keyboard^configurable driver=1273
%virtual consoles^configurable keyboard driver=1273
%driver^virtual consoles, configurable keyboard=1273
%device driver^virtual consoles, configurable keyboard=1273
%/etc/drvld.all=1277
%drvld.all=1277
%<ctrl><alt><del>, disable=1277
%keyboard^<ctrl><alt><del>=1277
%keyboard^rebooting=1277
.pn 1277+1
grep: /v/PostScript/bbl/paris_hours/phw.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/w.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phw.bnd
%wait=1278
+1wait	Await completion of background process	1278
%wait.h=1278
+1wait.h	Define wait routines	1278
%wait()=1278
+1wait()	Await completion of a child process	1278
%waitpid()=1279
+1waitpid()	Wait for a process to terminate	1279
%wall=1279
+1wall	Send a message to all logged-in users	1279
%wc=1280
+1wc	Count words, lines, and characters in text files	1280
%welcome=1280
+1welcome	Welcome a new user	1280
%whence=1280
+1whence	List a command's type	1280
%whereis=1280
+1whereis	Locate source, binary, and manual files	1280
%which=1281
+1which	Locate executable files	1281
%while=1282
+1while	Execute commands repeatedly	1282
%while=1282
+1while	Introduce a loop	1282
%who=1282
+1who	Print who is logged in	1282
%wildcards=1282
+1wildcards	1282
%metacharacter=1282
%write=1283
+1write	Converse with another user	1283
%write()=1283
+1write()	Write to a file	1283
%wtmp=1284
+1wtmp	File that records past login events	1284
%/usr/adm/wtmp=1284
.pn 1284+1
grep: /v/PostScript/bbl/paris_hours/phx.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/x.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phx.bnd
%xargs=1285
+1xargs	Execute a command with many arguments	1285
%BUFSIZ=1285
%stdio.h=1285
%xgcd()=1285
+1xgcd()	Extended greatest-common-divisor function	1285
.pn 1285+1
grep: /v/PostScript/bbl/paris_hours/phy.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/y.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phy.bnd
%yacc=1286
+1yacc	Parser generator	1286
%liby=1286
%library^yacc=1286
%yes=1287
+1yes	Print infinitely many responses	1287
.pn 1287+1
grep: /v/PostScript/bbl/paris_hours/phz.bnd: No such file or directory
troff: /v/doc/coherent/alphabet/z.r: 1: .rb: cannot open file /v/PostScript/bbl/paris_hours/phz.bnd
%zcat=1288
+1zcat	Concatenate a compressed file	1288
%archive^extracting from compressed=1288
%zcmp=1288
+1zcmp	Compare compressed files	1288
%zdiff=1288
+1zdiff	Compare two compressed files	1288
%zerop()=1288
+1zerop()	Indicate if multi-precision integer is zero	1288
%zforce=1288
+1zforce	Force the suffix .gz onto every gzip file	1288
%zgrep=1289
+1zgrep	Search compressed files for a regular expression	1289
%zip=1289
+1zip	Zip files into a compressed archive	1289
%zmore=1290
+1zmore	Display compressed text one page at a time	1290
%znew=1290
+1znew	Recompress .Z files to .gz files	1290
.pn 1290+1
Final index page:  1291
Final contents page:  i
