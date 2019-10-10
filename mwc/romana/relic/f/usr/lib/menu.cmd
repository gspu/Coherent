;	MENU.CMD:	Menu learning system for MicroEMACS 3.7
;
;			This file is executed to activate MicroEMACS's
;			menu interface code

;	setup windows for use

	add-global-mode "blue"
	1 split-current-window
	5 resize-window
	add-mode "red"
	view-file "menu1"
	name-buffer "menu window"
	change-file-name ""
	add-mode "view"
	next-window

;	Load menu routines as needed

;	Activate Main Menu

1	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<01"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key execute-macro-3	FN1
	bind-to-key execute-macro-4	FN2
	bind-to-key execute-macro-5	FN3
	bind-to-key execute-macro-6	FN4
	bind-to-key execute-macro-7	FN5
	bind-to-key execute-macro-8	FN6
	bind-to-key execute-macro-9	FN7
	bind-to-key execute-macro-2	FN8
	bind-to-key execute-macro-10	FN9
	bind-to-key exit-emacs		FN0
	clear-message-line
!endm

;	and bring that menu up

	execute-macro-1
	write-message "         [loading MENU system]"

;	set up the editor control menu

2	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<02"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key execute-macro-11	FN1
	bind-to-key execute-macro-12	FN2
	bind-to-key execute-macro-13	FN3
	bind-to-key execute-macro-14	FN4
	bind-to-key execute-macro-15	FN5
	bind-to-key execute-macro-16	FN6
	bind-to-key execute-macro-17	FN7
	bind-to-key execute-macro-18	FN8
	bind-to-key execute-macro-19	FN9
	bind-to-key execute-macro-1	FN0
	clear-message-line
!endm

;	Activate word case/screen control Menu

3	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<03"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key case-word-upper	FN1
	bind-to-key case-region-upper	FN2
	bind-to-key case-word-lower	FN3
	bind-to-key case-region-lower	FN4
	bind-to-key case-word-capitalize FN5
	unbind-key FN6
	bind-to-key clear-and-redraw	FN7
	bind-to-key set-mark		FN8
	bind-to-key redraw-display	FN9
	bind-to-key execute-macro-1	FN0
	clear-message-line
!endm

;	Activate paging/scrolling Menu

4	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<08"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key previous-page	FN1
	bind-to-key next-page		FN2
	bind-to-key move-window-down	FN3
	bind-to-key move-window-up	FN4
	bind-to-key scroll-next-up	FN5
	unbind-key 			FN6
	bind-to-key scroll-next-down	FN7
	unbind-key 			FN8
	bind-to-key exchange-point-and-mark FN9
	bind-to-key execute-macro-1	FN0
	clear-message-line
!endm

;	Activate cut & paste Menu

5	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<04"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key set-mark		FN1
	unbind-key FN2
	bind-to-key kill-region		FN3
	unbind-key FN4
	bind-to-key copy-region		FN5
	unbind-key FN6
	bind-to-key yank		FN7
	unbind-key FN8
	unbind-key FN9
	bind-to-key execute-macro-1	FN0
	clear-message-line
!endm

;	Activate Search & replace Menu

6	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<09"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key search-forward	FN1
	bind-to-key search-reverse	FN2
	bind-to-key hunt-forward	FN3
	bind-to-key hunt-backward	FN4
	bind-to-key incremental-search	FN5
	bind-to-key reverse-incremental-search FN6
	bind-to-key replace-string	FN7
	bind-to-key query-replace-string FN8
	unbind-key FN9
	bind-to-key execute-macro-1	FN0
	clear-message-line
!endm

;	Activate Deletion Menu

7	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<05"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key delete-previous-character FN1
	unbind-key FN2
	bind-to-key delete-next-character FN3
	unbind-key FN4
	bind-to-key kill-to-end-of-line	FN5
	unbind-key FN6
	bind-to-key delete-blank-lines	FN7
	unbind-key FN8
	unbind-key FN9
	bind-to-key execute-macro-1	FN0
	clear-message-line
!endm

;	Activate Word procesing Menu

8	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<10"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key previous-word	FN1
	bind-to-key next-word		FN2
	bind-to-key previous-paragraph	FN3
	bind-to-key next-paragraph	FN4
	bind-to-key fill-paragraph	FN5
	bind-to-key kill-paragraph	FN6
	bind-to-key delete-previous-word FN7
	bind-to-key delete-next-word	FN8
	bind-to-key count-words		FN9
	bind-to-key execute-macro-1	FN0
	clear-message-line
!endm

;	Activate Insertion Menu

9	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<06"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key open-line		FN1
	bind-to-key insert-string	FN2
	bind-to-key handle-tab		FN3
	bind-to-key quote-character	FN4
	bind-to-key insert-space	FN5
	bind-to-key transpose-characters FN6
	bind-to-key newline-and-indent	FN7
	unbind-key FN8
	bind-to-key newline		FN9
	bind-to-key execute-macro-1	FN0
	clear-message-line
!endm

;	Activate Cursor movement Menu

10	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<07"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key beginning-of-file	FN1
	bind-to-key previous-line	FN2
	bind-to-key backward-character	FN3
	bind-to-key forward-character	FN4
	bind-to-key end-of-file		FN5
	bind-to-key next-line		FN6
	bind-to-key beginning-of-line	FN7
	bind-to-key end-of-line		FN8
	bind-to-key execute-macro-21	FN9
	bind-to-key execute-macro-1	FN0
	clear-message-line
!endm

21	store-macro
	@"Line number to go to: " goto-line
!endm

;	Activate Buffer Menu

11	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<11"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key buffer-position	FN1
	bind-to-key unmark-buffer	FN2
	bind-to-key delete-buffer	FN3
	bind-to-key next-buffer		FN4
	bind-to-key list-buffers	FN5
	bind-to-key execute-macro-22	FN6
	bind-to-key name-buffer		FN7
	unbind-key FN8
	bind-to-key select-buffer	FN9
	bind-to-key execute-macro-2	FN0
	clear-message-line
!endm

22	store-macro
	filter-buffer @"Name of DOS filter: "
!endm
;	Macro Menu

12	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<11"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key begin-macro		FN1
	unbind-key FN2
	bind-to-key end-macro		FN3
	unbind-key FN4
	bind-to-key execute-macro	FN5
	unbind-key FN6
	unbind-key FN7
	unbind-key FN8
	unbind-key FN9
	bind-to-key execute-macro-2	FN0
	clear-message-line
!endm

;	Color change Menu

13	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<12"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key execute-macro-23	FN1
	unbind-key FN2
	bind-to-key execute-macro-24	FN3
	unbind-key FN4
	bind-to-key execute-macro-25	FN5
	unbind-key FN6
	bind-to-key execute-macro-26	FN7
	unbind-key FN8
	unbind-key FN9
	bind-to-key execute-macro-2	FN0
	clear-message-line
!endm

;	Set forground color

23	store-macro
	save-window
	1 next-window
	select-buffer "[color]"
	beginning-of-file
	insert-string @"Color to change to: "
	newline
	beginning-of-file
	case-word-upper
	beginning-of-file
	unmark-buffer
	select-buffer "menu window"
	1 redraw-display
	restore-window
	add-mode #"[color]"
	delete-buffer "[color]"
!endm

;	Set background color

24	store-macro
	save-window
	1 next-window
	select-buffer "[color]"
	beginning-of-file
	insert-string @"Color to change to: "
	newline
	beginning-of-file
	case-word-lower
	beginning-of-file
	unmark-buffer
	select-buffer "menu window"
	1 redraw-display
	restore-window
	add-mode #"[color]"
	delete-buffer "[color]"
!endm

;	Set global forground color

25	store-macro
	save-window
	1 next-window
	select-buffer "[color]"
	beginning-of-file
	insert-string @"Color to change to: "
	newline
	beginning-of-file
	case-word-upper
	beginning-of-file
	unmark-buffer
	select-buffer "menu window"
	1 redraw-display
	restore-window
	add-global-mode #"[color]"
	delete-buffer "[color]"
!endm

;	Set global background color

26	store-macro
	save-window
	1 next-window
	select-buffer "[color]"
	beginning-of-file
	insert-string @"Color to change to: "
	newline
	beginning-of-file
	case-word-lower
	beginning-of-file
	unmark-buffer
	select-buffer "menu window"
	1 redraw-display
	restore-window
	add-global-mode #"[color]"
	delete-buffer "[color]"
!endm

;	set Mode Menu

14	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<17"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key add-mode		FN1
	bind-to-key add-global-mode	FN2
	bind-to-key delete-mode		FN3
	bind-to-key delete-global-mode	FN4
	unbind-key FN5
	bind-to-key execute-macro-27	FN6
	unbind-key FN7
	unbind-key FN8
	bind-to-key select-buffer	FN9
	bind-to-key execute-macro-2	FN0
	clear-message-line
!endm

27	store-macro
	@"Column to fill to: " set-fill-column
!endm

;	DOS command Menu

15	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<13"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key shell-command	FN1
	unbind-key FN2
	bind-to-key pipe-command	FN3
	unbind-key FN4
	bind-to-key i-shell		FN5
	unbind-key FN6
	bind-to-key quick-exit		FN7
	unbind-key FN8
	bind-to-key exit-emacs		FN9
	bind-to-key execute-macro-2	FN0
	clear-message-line
!endm

;	Script Menu

16	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<18"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key execute-file	FN1
	bind-to-key execute-command-line FN2
	bind-to-key execute-buffer	FN3
	bind-to-key execute-named-command FN4
	unbind-key FN5
	unbind-key FN6
	unbind-key FN7
	unbind-key FN8
	unbind-key FN9
	bind-to-key execute-macro-2	FN0
	clear-message-line
!endm

;	File access Menu

17	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<14"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key find-file		FN1
	bind-to-key save-file		FN2
	bind-to-key view-file		FN3
	bind-to-key write-file		FN4
	bind-to-key read-file		FN5
	bind-to-key change-file-name	FN6
	bind-to-key insert-file		FN7
	unbind-key FN8
	unbind-key FN9
	bind-to-key execute-macro-2	FN0
	clear-message-line
!endm

;	Window Menu

18	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<19"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key split-current-window FN1
	bind-to-key delete-other-windows FN2
	bind-to-key resize-window	FN3
	bind-to-key delete-window	FN4
	bind-to-key shrink-window	FN5
	bind-to-key grow-window		FN6
	bind-to-key next-window		FN7
	bind-to-key previous-window	FN8
	unbind-key FN9
	bind-to-key execute-macro-2	FN0
	clear-message-line
!endm

;	key binding Menu

19	store-macro
	save-window
	1 next-window
	beginning-of-file
	search-forward "<<15"
	next-line
	1 redraw-display
	restore-window
	update-screen

;	***** Rebind the Function key group

	bind-to-key bind-to-key		FN1
	unbind-key FN2
	bind-to-key unbind-key		FN3
	unbind-key FN4
	bind-to-key describe-key	FN5
	unbind-key FN6
	bind-to-key describe-bindings	FN7
	unbind-key FN8
	unbind-key FN9
	bind-to-key execute-macro-2	FN0
	clear-message-line
!endm

	clear-message-line
