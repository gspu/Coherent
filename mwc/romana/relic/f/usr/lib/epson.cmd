;	EPSON.CMD	This command file changes some function keys
;			to generate printer control sequences

20	store-macro
	insert-string "l"
!endm

bind-to-key execute-macro-20 FNx
