;	CPAGE.CMD:	C language MENU Page
;			for MicroEMACS 3.9e and above
;			(C)opyright 1988 by Daniel Lawrence
;

; set up the "clean" procedure
store-procedure clean
	delete-buffer "[Macro 10]"
	delete-buffer "[Macro 11]"
	delete-buffer "[Macro 12]"
	delete-buffer "[Macro 13]"
	delete-buffer "[Macro 14]"
	delete-buffer "[Macro 15]"
	delete-buffer "[Macro 16]"
	delete-buffer "[Macro 17]"
	delete-buffer "[Macro 18]"
	delete-buffer "[Macro 19]"
	delete-buffer "[drawbox]"
	delete-buffer "[setpoints]"
	delete-buffer "[findcom]"
	set $cmode %oldmode
	set $gmode %oldgmode
!endm

; make sure the function key window is up
	set %rcfkeys FALSE
	execute-macro-1
	write-message "Loading..."

; Write out the page instructions
	save-window
	1 next-window
	beginning-of-file
	set $curcol 25
	overwrite-string " F1 function             F2 if-then-else       "
	next-line
	set $curcol 25
	overwrite-string " F3 for                  F4 while              "
	next-line
	set $curcol 25
	overwrite-string " F5 do                   F6 switch             "
	next-line
	set $curcol 18
	overwrite-string "C   "
	set $curcol 25
	overwrite-string " F7 VALUE                F8 Reserved words UP  "
	next-line
	set $curcol 25
	overwrite-string " F9 chk comment matching F10 Comment block      "
	unmark-buffer
	beginning-of-file
	!force restore-window
	update-screen

;

;	Function
10	store-macro
	set %func @"Function Name: "
	set %type @"Return type: "
	insert-string &cat &cat "#if~t__USE_PROTO__~n" %type " ("
	insert-string &cat &cat %func ") ()~n#else~n" %type
	insert-string &cat &cat "~n" %func " __ARGS (())~n#endif~n{~n}~n"
	2 backward-character
	!force set $debug FALSE
!endm
bind-to-key execute-macro-10 S-FN1

;

11	store-macro
	set %cond @"Condition Expr: "
	insert-string &cat &cat "if (" %cond ") {"
	newline-and-indent
	newline-and-indent
	insert-string "} else {"
	newline-and-indent
	newline-and-indent
       	insert-string &cat &cat "}  /* if (" %cond ") */"
	1 previous-line
	end-of-line
	handle-tab
	2 previous-line
	end-of-line
	handle-tab
!endm
bind-to-key execute-macro-11 S-FN2

;

12	store-macro
	set %cond @"Condition Expr: "
	;set $debug TRUE
	insert-string &cat &cat "for (" %cond ") {"
	newline-and-indent
	newline-and-indent
	insert-string "}"
	1 previous-line
	end-of-line
	handle-tab
!endm
bind-to-key execute-macro-12 S-FN3
	
13	store-macro
	set %cond @"Condition Expr: "
	;set $debug TRUE
	insert-string &cat &cat "while (" %cond ") {"
	newline-and-indent
	newline-and-indent
	insert-string &cat &cat "}  /* while (" %cond ") */"
	1 previous-line
	end-of-line
	handle-tab
!endm
bind-to-key execute-macro-13 S-FN4

14	store-macro
	set %cond @"Condition Expr: "
	;set $debug TRUE
	set %curcol $curcol
	insert-string "do {"
	newline-and-indent
	newline-and-indent
	insert-string &cat &cat "} while (" %cond ");"
	1 previous-line
	handle-tab
	!force set $debug FALSE
!endm
bind-to-key execute-macro-14 S-FN5

15	store-macro
	set %expr @"Switch Expr: "
	;set $debug TRUE
	set %curcol $curcol
	insert-string &cat &cat "switch (" %expr ") {"
	newline-and-indent
	insert-string "}"
	previous-line
	end-of-line
	newline-and-indent
	handle-tab
	insert-string "case "
	!force set $debug FALSE
!endm
bind-to-key execute-macro-15 S-FN6

16	store-macro
;	set $debug TRUE
	set %curcol $curcol
	set %value @"Value: "
	insert-string &cat %value " : {~n"
	%curcol insert-string " "
	insert-string "    };~n"
	%curcol insert-string " "
	previous-line
	beginning-of-line
	open-line
	%curcol insert-string " "
	insert-string "       "
	!force set $debug FALSE
!endm
bind-to-key execute-macro-16 S-FN7

17	store-macro
;	set $debug TRUE
	set %curcol $curcol
	;save-file
	set-mark
	beginning-of-file
	write-message "[Replacing 'begin']
	replace-string "begin" "BEGIN"

	beginning-of-file
	write-message "[Replacing 'end']
	replace-string "end" "END"

	beginning-of-file
	write-message "[Replacing ' if']
	replace-string " if" " IF"

	beginning-of-file
	write-message "[Replacing ' else']
	replace-string " else" " ELSE"

	beginning-of-file
	write-message "[Replacing ' repeat']
	replace-string " repeat" " REPEAT"

	beginning-of-file
	write-message "[Replacing ' while']
	replace-string " while" " WHILE"

	exchange-point-and-mark
	!force set $debug FALSE
!endm
bind-to-key execute-macro-17 S-FN8

18	store-macro
	beginning-of-file
	run findcom
!endm
bind-to-key execute-macro-18 S-FN9

store-procedure	findcom
;	This hunts down mismatched comment problems in C

;	start from the current position in the file

	!force search-forward "/*"
	!if &seq $status FALSE
		!goto nend
	!endif

*nxtopen
	;record the position of the open comment
	update-screen
	set %oline $curline
	set %opos $curcol

	;find the first close comment
	!force search-forward "*/"
	!if &seq $status FALSE
		write-message "%%No close to this open comment"
		!return
	!endif

	;record the position of the close comment
	set %cline $curline
	set %cpos $curcol

	;go back to the open and find the next open
	set $curline %oline
	set $curcol %opos

	;and now find the next open
	!force search-forward "/*"
	!if &seq $status FALSE
		write-message "No errors"
		!return
	!endif
	set %nline $curline
	set %npos $curcol

	;compare the close to the next open
	!if &less %cline %nline
		!goto getnext
	!endif
	!if &and &equ %cline %nline &less %cpos %npos
		!goto getnext
	!endif

	;report a mismatch
	set $curline %oline
	set $curcol %opos
	write-message "%%This comment does not terminate properly"
	!return

*getnext
	set $curline %nline
	set $curcol %npos
	!goto nxtopen 
!endm

19	store-macro
	set %col $curcol
	set-mark
	insert-string "/*~n"
	%col insert-string " "
	insert-string " * ~n"
	%col insert-string " "
	insert-string " */"
	entab-region
	exchange-point-and-mark
	1 next-line
	end-of-line
!endm
bind-to-key execute-macro-19 S-FN0
bind-to-key execute-macro-19 A-FN0

; Set up CMODE
	set %oldmode $cmode
	set %oldgmode $gmode
	add-mode CMODE
	add-global-mode CMODE

write-message "[C MENU page loaded]"
