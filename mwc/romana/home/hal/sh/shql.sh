#!/bin/sh
# use /bin/sh or /bin/ksh
#
# shql - version 1.1
#

# DEFINE THESE
SHQL_ROOT="/u/shql"		# system-wide database location
EDITOR="${EDITOR:=/usr/bin/vi}" # default editor if EDITOR not defined
SHELL="${SHELL:=/bin/sh}" 	# default shell if SHELL not defined
AWK=mawk

# Unix table file postfixes:  @ is attrib, ~ is data, % is view

DEBUG="N"	# set to Y for debugging

[ "$DEBUG" = "Y" ] && set -x   		# uncomment for debugging
#set -v
UMASK=`umask`
umask 0000		# share database
trap "echo \"Goodbye\" ; \
    rm -f /tmp/$$ /tmp/$$row /tmp/$$join*" 0 1 2 3 15
set -h			# remember functions

if echo '\c' | grep -s c ; then		# to adapt to System V vs. BSD 'echo'
	NOCR1='-n'			# BSD
	NOCR2=""
else
	NOCR1=""			# System V
	NOCR2='\c'
fi
NL='
'
TAB='	'
export _IFS TABLE CMD NOCR1 NOCR2 NL TAB
_IFS="$IFS"

if [ "X$1" = "X-q" ] 
then	QUIET="Y"
	shift
fi

if [ "X$1" = "X" ]
then	echo "Missing database name." 1>&2
	echo "The database name must be a directory under $HOME/shql" 1>&2
	echo "	or a directory under $SHQL_ROOT" 1>&2
	exit 1
fi
echo "Database: $1"

if [ -d $HOME/shql/$1 ]
then	cd $HOME/shql/$1
elif [ -d $SHQL_ROOT/$1 ]
then	cd $SHQL_ROOT/$1
elif [ -d $HOME/$1 ]
then	cd $HOME/$1
elif [ -d $1 ]
then	cd $1
else 	echo "Unknown database ($1)" 1>&2
	echo "The database name must be a directory under $HOME/shql" 1>&2
	echo "	or a directory under $SHQL_ROOT" 1>&2
	exit 1
fi


#
#**************************************************************************
# syntax
#**************************************************************************
syntax(){
	case "$1" in
		create)	cat <<"END"
CREATE TABLE table_name (
	column_name column_width 
	{, ...} 
) 	
or
CREATE VIEW view_name (
	table_or_view1.column1 = table_or_view2.column2
)
END
return 0
;;
		delete) cat <<"END"
DELETE 
FROM table_name	
{ WHERE where_clause }
END
return 0
;;
		drop) cat <<"END"
DROP TABLE table_name
or
DROP VIEW view_name
END
return 0
;;
		edit) cat <<"END"
EDIT table_name
is a non-standard method of changing a table's field names or display widths.
END
return 0
;;
		help)	cat <<"END"
HELP ALL
or
HELP TABLES 
or
HELP VIEWS
or
HELP COMMANDS
or
HELP [CREATE | DELETE | DROP | INSERT | SELECT | UPDATE | WHERE | PRINT | EDIT]
or
HELP table_name
Commands must appear in lower case.
END
return 0
;;
		insert) cat <<"END"
INSERT INTO table_name 
	{ ( column_name, ... ) }
VALUES ( expression, ...)
or		
INSERT INTO table_name 
	{ ( column_name, ... ) }
subselect
END
return 0
;;
		print) cat <<"END"
PRINT table_name 
is a non-standard synonym for SELECT * FROM table_name.
END
return 0
;;
		select) cat <<"END"
SELECT { DISTINCT } 
	[ column_name {,...} | * ]
FROM [ table_name | view_name ]
{ WHERE where_clause }
{ ORDER BY column_name { NUM } { ASC | DESC } {, ... }
{ UNION select statement }
'NUM' is a non-standard method for sorting numeric fields.
END
return 0
;;
		update) cat <<"END"
UPDATE table_name
SET column_name = expression {, ... }
{ WHERE where_clause }
END
return 0
;;
		where) cat <<"END"
WHERE [ column_name | value ] [ =, !=, >, <, >=, <=, and, or, not, in ]
      [ column_name | value | subselect ]
Parentheses may be used to group expressions.  
END
return 0
;;
		syntax)	syntax commands; echo
			syntax create; echo
			syntax delete; echo
			syntax drop; echo
			syntax insert; echo
			syntax select; echo
			syntax update; echo
			syntax where; echo
			syntax print; echo
			syntax edit; echo
			return 0
			;;
	esac
	return 1
}

#
#**************************************************************************
# lookup_field
#**************************************************************************
lookup_field(){
	if [ ! -f $TABLE% ]
	then	RESULT="`grep -n \"^$1	\" $TABLE@`"
	else 	RESULT="`grep -n \"^$1	\" $TABLE@ | sed 1q`"
	fi
	if [ ! "$RESULT" ] 
	then 	OUTFIELD="$1"
		return 1
	else	OUTFIELDNUM="`expr "$RESULT" : '\([^:]*\)'`"
		OUTFIELD="\$$OUTFIELDNUM" 
		return 0
	fi
}

#
#**************************************************************************
# do_aggreg
#**************************************************************************
do_aggreg(){
	if 	[ "X$1" = 'Xsum' ]
	then	AGGREG='total'
	elif 	[ "X$1" = 'Xavg' ]
	then 	AGGREG='(total/cnt)'
	elif 	[ "X$1" = 'Xcount' ]
	then 	AGGREG='cnt'
	elif 	[ "X$1" = 'Xmin' ]
	then 	AGGREG='min'
	elif 	[ "X$1" = 'Xmax' ]
	then 	AGGREG='max'
	else	return 1
	fi
	[ "X$2" != "X(" -o "X$4" != "X)" ] && \
		echo "Bad aggregate syntax" 1>&2 && syntax select && return 1
	AGGFIELD="$3"
	shift 4
	lookup_field "$AGGFIELD"
	[ "$?" -ne 0 ] && echo "Bad field name ($1)" 1>&2 && return 1
	while [ $# -ne 0 ]
	do	
		[ "X$1" = "Xwhere" ] && break;
		[ "X$1" = "Xorder" ] && break;
		[ "X$1" = "Xunion" ] && break;
		shift
	done

	OUTFIELD=`( SUBSELECT="Y" ; AGGREGATE="Y"; \
	    select_ "select" "$AGGFIELD" "from" "$TABLE" "$@") | \
	    $AWK -F"	" \
		'NR == 1 { min = $1; max = $1 }
			 { cnt += 1; total += $1 }
		$1 < min { min = $1 }
		$1 > max { max = $1 }
		END	 { printf "%s%s%s", "\"", '$AGGREG', "\"" }'`
	if [ `expr "$RESULT" : '[^	]*	\(.*\)'` -lt 10 ]
	then	RESULT="$AGGFIELD	10"
	fi
	return 0
}

#
#**************************************************************************
# do_join 
#**************************************************************************
do_join(){
	update_view "$1"
	TABLE="$1"
	lookup_field "$2" 
	[ "$?" -ne 0 ] && echo "Bad view specifcation ($1.$2)" 1>&2 && return 1
	JFIELD1="$OUTFIELDNUM"
	JFIELD1L1="`expr $JFIELD1 - 1`"
	update_view "$3"
	TABLE="$3"
	lookup_field "$4" 
	[ "$?" -ne 0 ] && echo "Bad view specifcation ($3.$4)" 1>&2 && return 1
	JFIELD2="$OUTFIELDNUM"
	JFIELD2L1="`expr $JFIELD2 - 1`"

	( grep "^$2	" $1@ ;
	  grep -v "^$2	" $1@ ;
	  grep -v "^$4	" $3@ ) > $5@
	sort -t\	 +$JFIELD2L1 $3~ > /tmp/$$
	sort -t\	 +$JFIELD1L1 $1~ | \
		join -t\	 -j1 $JFIELD1 -j2 $JFIELD2 \
						- /tmp/$$ > $5~
}

#
#**************************************************************************
# update_view
#**************************************************************************
update_view(){
	[ ! -f "$1%" ] && return 1
	( do_join `cat $1%` )
}

#
#**************************************************************************
# where
#**************************************************************************
where(){
	shift
	while [ $# -gt 0 -a "$1" != "order" -a "$1" != "union" ]
	do
		if [ "X$1" = "Xselect" ]
		then
			set X `( SUBSELECT="Y" ;select_ "$@")`
			if [ "$?" -eq 0 ]
			then 	shift
			else 	return 1
			fi
		fi
		case "$1" in
			and) 	WHERE="$WHERE && ";;
			or)	WHERE="$WHERE || ";;
			not)	WHERE="$WHERE !" ;;
			=)	WHERE="$WHERE == ";;
			'in') 	shift
				set X `( SUBSELECT='Y';select_ "$@" )`
				if [ "$?" -eq 0 ]
				then 	shift
				else 	return 1
				fi
				INWHERE=""
				COMP="=="
				LOGIC="||"
				[ "X$LAST" = "Xnot" ] && COMP="=" && LOGIC="&&"
				for VALUE
				do
					[ "X$INWHERE" != "X" ] && 
						INWHERE="$INWHERE $LOGIC"
					INWHERE="$INWHERE ($WHERE$COMP $VALUE) "
				done
				WHERE="$INWHERE"
				break;;
			*)	lookup_field "$1"
				WHERE="$WHERE $OUTFIELD";;
		esac
		LAST="$1"
		shift
	done 
	[ "$WHERE" ] && WHERE=" ( $WHERE ) " && return 0
	echo "Missing 'where' clause" 1>&2
	syntax where
	return 1
}

#
#**************************************************************************
# help
#**************************************************************************
help(){
	if [ ! "$2" ]
	then	echo "Ambiguous syntax, try:" 1>&2 ; syntax help
	elif [ "$2" = "all" ]
	then	ls *@ *% 2>/dev/null | cut -d@ -f1 | cut -d% -f1 | uniq
	elif [ "$2" = "tables" ] 
	then	ls *@ *% 2>/dev/null | cut -d@ -f1 | cut -d% -f1 | uniq -u 
	elif [ "$2" = "views" ] 
	then	ls *% 2>/dev/null | cut -d% -f1 
	elif [ "$2" = "commands" ]
	then	cat << "END"
/p is print
/g is go(execute)
/q is quit
/e is edit
/i is include
/w is write
/r is reset(clear)
/s is shell
/p/g print and go
The number sign(#) may be used at the start of a line for comments.
END
	else	syntax $2 && return
		TABLE="$2"
		update_view "$TABLE"
		if [ -f "$2@" ] 
		then	echo "$NL <$2>" && cat "$2@"
			[ -f "${2}%" ] &&echo $NOCR1 "$NL View:	$NOCR2" && 
				set X `cat $2%` && shift &&
				echo "$1.$2 = $3.$4"
			echo "$NL Rows:	"`cat $TABLE~ | wc -l`
		else 	echo "$TABLE does not exist." 1>&2
			syntax help
		fi
	fi
}

#
#**************************************************************************
# create
#**************************************************************************
create(){
	shift
	if [ -f "$2@" -o -f "$2%" ]
	then	echo "Table already exists." 1>&2
	elif [ "X$1" = "Xview" -a $# -gt 2 ]
	then	shift
		if [ $# -ne 6 ]
		then 	syntax create
		else 	
			[ "X$2" != "X(" ] && echo "Bad syntax" 1>&2 && 
							syntax create && return
			TABLE1="`expr $3 : '\([^\.]*\)'`"
			FIELD1="`expr $3 : '[^\.]*.\(.*\)'`"
			TABLE="$TABLE1"
			lookup_field "$FIELD1" 
			[ "$?" -ne 0 ] && echo "Bad table or field name" 1>&2 &&
									return
			[ "X$4" != "X=" ] && echo "Bad syntax" 1>&2 && 
							syntax create && return
			TABLE2="`expr $5 : '\([^\.]*\)'`"
			FIELD2="`expr $5 : '[^\.]*.\(.*\)'`"
			TABLE="$TABLE2"
			lookup_field "$FIELD2" 
			[ "$?" -ne 0 ] && echo "Bad table or field name" 1>&2 &&
									return
			[ "X$2" != "X(" ] && echo "Bad syntax" 1>&2 && 
							syntax create && return
			echo "$TABLE1 $FIELD1 $TABLE2 $FIELD2 $1" > $1%
			update_view "$1"			
		fi
		echo "OK"
	elif [ "X$1" = "Xtable" -a $# -ge 5 ] 
	then
		[ "X$3" != "X(" ] && echo "Bad syntax" 1>&2 && 
							syntax create && return
		TABLE="$2"
		shift 3
		> $TABLE@
		> $TABLE~
		while [ $# -ge 2 ]
		do
			echo "$1	$2" >> $TABLE@
			shift 2
		done
		[ "X$1" != "X)" ] && echo "Bad syntax" 1>&2 && 
					rm -f $TABLE[@~] && syntax create && return
		echo "OK"
	else 
		echo "Improper syntax ($1)" 1>&2
		syntax create
	fi
	return
}

#
#*************************************************************************
# drop
#**************************************************************************
drop(){
	[ "$2" != "table" -a "$2" != "view" ] && 
		echo "Syntax error." 1>&2 && syntax drop && return
	[ "$2" = "table" -a -f "$3%" ] &&
		echo "Can not drop, $2 is a view, not a table" 1>&2 && return
	[ "$2" = "view" -a ! -f "$3%" ] &&
		echo "Can not drop, $2 is not a view" 1>&2 && return
	if [ -f "$3@" -o -f "$3%" ] 
	then	rm -f $3@ $3~ $3%
		echo "OK"
	else  	echo "No such table" 1>&2
	fi
}

#
#**************************************************************************
# insert
#**************************************************************************
insert(){
	shift
	[ "X$1" != "Xinto" ] && echo "Improper syntax ($1)" 1>&2 && 
		syntax insert && return
	shift
	TABLE="$1"
	update_view "$TABLE" && echo "Can not insert into a view" 1>&2 && return
	[ ! -f "$TABLE@" ] && echo "Table does not exist" 1>&2 && return
	shift
	ATTRIB="`cat $TABLE@ | wc -l`"
	XASGN=""
	XECHO="echo \""
	if [ $# -gt 0 -a "X$1" = "X(" ]
	then	ATTRIB2="0"
		shift
		while [ $# -gt 0 -a "X$1" != "X)" ]
		do
			lookup_field "$1" 
			[ "$?" -ne 0 ] && echo "Bad field name. ($1)" 1>&2 && 
									return 
			XASGN="$XASGN X$OUTFIELDNUM=\`eval echo \$1\` ; shift;"
			shift
			ATTRIB2=`expr $ATTRIB2 + 1`
		done
		[ "X$1" != "X)" ] && echo "Syntax error ($1)" 1>&2 && 
						syntax insert && return
		shift
		POS="1"
		while [ "$POS" -le "$ATTRIB" ]
		do
			eval X$POS=""
			[ "$POS" != "1" ] && XECHO="$XECHO\$TAB"
			XECHO="$XECHO\$X$POS"
			POS=`expr $POS + 1`
		done
		XECHO="$XECHO\""
		ATTRIB="$ATTRIB2"
	fi	
	if [ "X$1" = "Xselect" ]
	then 	eval set X "`( SUBSELECT='Y' ; select_ "$@" )` \)"
		shift
	elif [ "X$1" != "Xvalues" -o "X$2" != 'X(' ]  
		then	 echo "Improper syntax ($1)" 1>&2 && syntax insert && 
									return
	else	shift 2
	fi
	for LAST do 
	: ; done
	[ "X$LAST" != "X)" ] && 
		echo "Improper syntax" 1>&2 && syntax insert && return
	if [ "`expr \( $# - 1 \) % $ATTRIB`" -ne 0 ]
	then 	echo "Incorrect number of values." 1>&2
	else	ROWS="`expr \( $# - 1 \) / $ATTRIB`"
		while [ $# -gt 1 ]
		do	
			if [ "$XASGN" = "" ]
			then 	
				echo $NOCR1 "`eval echo $1`$NOCR2" >> $TABLE~ 
				shift
				while [ "`expr \( $# - 1 \) % $ATTRIB`" -ne 0 ]
				do
					echo $NOCR1 "$TAB`eval echo $1`$NOCR2"\
							 	>> $TABLE~
					shift
				done
				echo "" >> $TABLE~
			else	eval $XASGN
				eval $XECHO >> $TABLE~
			fi
		done
		echo "($ROWS rows)"			
	fi
}

#
#*************************************************************************
# delete
#**************************************************************************
delete(){
	shift
	TABLE="$2"
	update_view "$TABLE" && echo "You can not delete from a view." 1>&2 &&
									return  
	[ ! -f "$TABLE@" ] && echo "$TABLE does not exit." 1>&2 && return
	WHERE=""
	if [ "X$3" = "Xwhere" ]
	then 	shift 2
		where "$@" && 
		$AWK -F"	" "! $WHERE { cnt += 1 ; print } 
			END { printf \"( %1d rows.)\\n\", (NR - cnt) \
			>\"/tmp/$$row\" }" $TABLE~ > /tmp/$$ && 
			mv /tmp/$$ $TABLE~ && cat /tmp/$$row
	else	echo '('`cat $TABLE~ | wc -l`' rows)' 
		> $TABLE~
	fi
}

#
#*************************************************************************
# update
#**************************************************************************
update(){
	TABLE="$2"
	update_view "$TABLE" && echo "Can not update a view." 1>&2 && return
	[ ! -f "$TABLE@" ] && echo "$TABLE does not exit." 1>&2 && return
	[ "X$3" != "Xset" ] && echo "Improper syntax." 1>&2 && syntax update && 
									return
	shift 3
	ASSIGN=""
	while [ $# -gt 0 -a "X$1" != "Xwhere" ]
	do
		lookup_field "$1" && [ "X$2" = "X=" ] && ASSIGN="$ASSIGN ; "
		ASSIGN="$ASSIGN $OUTFIELD"
		shift
	done
	WHERE=""
	if [ "X$1" = "Xwhere" ] 
	then 	where "$@" || return 
	fi
	$AWK -F"	" "BEGIN { OFS = \"	\" }
		$WHERE 	{ $ASSIGN; cnt += 1 }
			{ print } 
		END 	{ printf \"( %1d rows)\\n\", cnt >\"/tmp/$$row\" }" \
		$TABLE~ > /tmp/$$ && 
			mv /tmp/$$ $TABLE~ && cat /tmp/$$row
}

#
#**************************************************************************
# select_
#**************************************************************************
select_(){
[ "$DEBUG" = "Y" ] && set -x   		# uncomment for debugging
	UNION="Y"
	while [ "$UNION" != "" ]
	do
		INAGG=""
		FROM=""
		UNION=""
		TABLE=""
		for ATABLE
		do
		    [ "X$ATABLE" = "Xwhere" ] && break
		    [ "X$ATABLE" = "Xorder" ] && break
		    [ "X$ATABLE" = "Xunion" ] && break
		    [ "X$ATABLE" = "Xfrom" ] && FROM="Y" && continue
		    if [ "$FROM" ]
		    then
			[ ! -f "$ATABLE@" ] && \
			echo "$ATABLE does not exist." 1>&2 && return 1
			if [ ! "$TABLE" ]
			then 	TABLE="$ATABLE"
			else	JTABLE="$TABLE"
				PREV=""
				PPREV=""
				FOUND=""
				for GETJ
				do
				    if [ "$PREV" = "=" ]
				    then
					TABLE="$JTABLE"
					lookup_field "$PPREV" &&
					TABLE="$ATABLE" &&
					lookup_field "$GETJ" &&
					FOUND="Y1" &&
					break
					TABLE="$ATABLE"
					lookup_field "$PPREV" &&
					TABLE="$JTABLE" &&
					lookup_field "$GETJ" &&
					FOUND="Y2" &&
					break
				    fi
				    PPREV="$PREV"
				    PREV="$GETJ"
				done
				[ ! "$FOUND" ] &&
				echo "Join not found, \c" &&
				echo "try reordering tables." 1>&2 && return 1
				if [ "$FOUND" = "Y1" ]
				then
	echo "$JTABLE	$PPREV	$ATABLE	$GETJ	/tmp/$$join2" >/tmp/$$join2%
				else
	echo "$ATABLE	$PPREV	$JTABLE	$GETJ	/tmp/$$join2" >/tmp/$$join2%
				fi
				update_view /tmp/$$join2
				mv /tmp/$$join2~ /tmp/$$join~
				mv /tmp/$$join2@ /tmp/$$join@
				expr "$RESULT" : '[^:]:*\(.*\)' >>/tmp/$$join@
				cut -d\	 -f1 /tmp/$$join~ | \
					paste /tmp/$$join~ - >/tmp/$$
				mv /tmp/$$ /tmp/$$join~
				TABLE="/tmp/$$join"
			fi
		    fi
		done
		[ ! "$FROM" ] && echo "Syntax error." 1>&2 && syntax select &&
								return 1
		update_view "$TABLE"
 		shift
		DISTINCT=""
		[ "X$1" = "Xdistinct" ] && DISTINCT="Y" && shift
		FIELDS=""
		PRINTF=""
		while [ "X$1" != "Xfrom" ]
		do
			if [ "X$1" = 'X*' ]
			then	shift
				set X `cat $TABLE@ | cut -d\	 -f1` "$@" 
				shift
			else	lookup_field "$1"
				if [ "$?" -ne 0 ]
				then 	do_aggreg "$@"
					if [ "$?" -eq 0 ]
					then	INAGG="Y"
						shift 3
					else 
					  echo "Bad field name ($1)" 1>&2
					  return 1
					fi
				fi
				[ "$FIELDS" ] && FIELDS="$FIELDS,"
				FIELDS="$FIELDS $OUTFIELD"
				if [ "$SUBSELECT" = "" ]
				then 	[ ! "$PRINTF" ] && PRINTF="|"
					WIDTH=`expr "$RESULT" : \
						'[^	]*	\(.*\)'`
					PRINTF="$PRINTF%-$WIDTH.${WIDTH}s|"
				else	if [ ! "$AGGREGATE" ]
					then	PRINTF="$PRINTF\\\"%s\\\" "
					else	PRINTF="$PRINTF%s\n"
					fi
				fi
				shift
			fi
		done
		shift 2
		WHERE=""
		SORT=""
		while [ $# -ne 0 ]
		do	
			if [ "X$1" = "Xwhere" ]
			then
				where "$@"
				[ "$?" -ne 0 ] && return 1 
				WHERE="$WHERE || NR == 1"
				shift
			elif [ "X$1" = "Xorder" ]
			then 	[ "X$2" != "Xby" ] && 
					echo "Syntax error ($2)" 1>&2 && 
					syntax select && return 1
				shift 2
				while [ $# -gt 0 -a "$1" != "union" ]
				do
					if [ 	"X$1" != "Xasc" -a \
						"X$1" != "Xdesc" -a \
						"X$1" != "Xnum" ] 
					then	lookup_field "$1" 
						[ "$?" -ne 0 ] &&
				echo "Bad field name ($1)" 1>&2 && return 1 
						[ "$SORT" = "" ] && 
							SORT="sort -t\"	\" "
						SORTL="`expr $OUTFIELDNUM - 1`"
						SORT="$SORT +$SORTL"
						[ "X$2" = "Xnum" ] && 
							SORT="${SORT}n"
						[ "X$2" = "Xdesc" ] && 
							SORT="${SORT}r"
						[ "X$3" = "Xdesc" ] && 
							SORT="${SORT}r"
						SORT="$SORT -$OUTFIELDNUM"
					fi
					shift
				done
			elif [ "X$1" = "Xunion" ]
			then	shift
				UNION="Y"
				break
			else	shift
			fi
		done
		[ "$INAGG" ] && WHERE="NR == 1"

		if [ "$DISTINCT" != "" ] 
		then	if [ "$SORT" = "" ]
			then	DIST="sort | uniq | tee /tmp/$$row"
			else	DIST="uniq | tee /tmp/$$row"
			fi
		else	DIST="cat"
		fi					

		TABLEFILE="$TABLE~"
		[ "$SORT" != "" ] && cat $TABLE~ | eval "$SORT" > /tmp/$$ &&
							TABLEFILE="/tmp/$$"

		if [ "$SUBSELECT" ]
		then	$AWK -F"	" "$WHERE {printf \"$PRINTF\", $FIELDS }" \
							$TABLEFILE |eval "$DIST"
		else    if [ ! "$QUIET" -o "$INAGG" = "Y" ]
			then 
			( set X `cut -d\	 -f1 $TABLE@` ; shift 
			  echo $NOCR1 "-$1-$NOCR2" ; shift 
		  	  for HEADING
			  do 
				echo $NOCR1 "$TAB-$HEADING-$NOCR2" 
	 		  done ; echo "" )
			fi |
			$AWK -F"	" \
			"$WHERE { cnt += 1 ; printf \"$PRINTF\\n\", $FIELDS }
			END	{ printf \"( %1d rows)\\n\", (cnt - 1) \
			>\"/tmp/$$row\" }" - $TABLEFILE | eval "$DIST" \
				&& if [ "$DISTINCT" = "" ]
				then	cat /tmp/$$row
				else 	X=`expr \`cat /tmp/$$row|wc -l\` - 1`
					echo '('$X' rows)' 
				fi
		fi
	done
	return 0
}	

#
#**************************************************************************
# main
#**************************************************************************
while :
do
	while :
	do
		echo $NOCR1 "* $NOCR2"
		read LINE || exit 
		SQLPART="`expr "$LINE" : '\(..*\)/.$'`"
		if [ "$SQLPART" != "" ]
		then
			[ "$NEW" = "Y" ] && _CMD=""
			if [ "`expr "$LINE" : '.*/p/g$'`" -ne 0 ]
			then
				_CMD="$_CMD"`expr "$LINE" : '\(.*\)/p/g$'`"$NL"
				LINE="/p/g"
				NEW=""
			else
				_CMD="$_CMD""$SQLPART""$NL"
				LINE="`expr "$LINE" : '.*\(/.\)$'`"
				NEW=""
			fi
		fi
 		case "$LINE" in 
			/p|p)  echo "$_CMD";;
			/g|g)  break;;
			/p/g|pg) echo "$_CMD" ; break ;;
			/r|r)  echo "reset" ; _CMD="";;
			/s|s)  umask $UMASK ; $SHELL ; umask 0000;;
			/e|e)  umask $UMASK ; echo "$_CMD" > /tmp/$$
				$EDITOR /tmp/$$; _CMD="`cat /tmp/$$`"
				umask 0000;;
			/i|i)  echo $NOCR1 "Enter include file: $NOCR2" 
				read LINE  
				[ -f "$LINE" ] && _CMD="$_CMD`cat $LINE`$NL" &&
				echo "$LINE included";;
			/w|w)  echo $NOCR1 "Enter output file: $NOCR2" 
				read LINE  
				[ "$LINE" ] && umask $UMASK && 
				echo "$_CMD" > "$LINE" && umask 0000 && 
				echo "$LINE written";;
			/q|q)  exit 0;; 
			\#*) 	[ "$NEW" = "Y" ] && _CMD="" ;;
			*)  	[ "$NEW" = "Y" ] && _CMD=""
			    	_CMD="$_CMD$LINE$NL";;
		esac
		NEW=""
	done

	CMD=`echo "$_CMD" | sed \
		-e "s/\'/\"/g" \
		-e 's/\"\([^\"]*\)\"/\"\\\"\1\\\"\"/g' \
		-e 's/\([<>!=][<>!=]*\)/ \1 /g' \
		-e 's/</\\\</g' \
		-e 's/>/\\\>/g' \
		-e 's/\*/\\\*/g' \
		-e 's/(/ \\\( /g' \
		-e 's/)/ \\\) /g'`
	[ ! "$CMD" ] && continue
	IFS="$_IFS,"
	eval set X $CMD
	shift
	IFS="$_IFS"
	NEW="Y"
	case $1 in
		select) 	select_ "$@";; 
		create) 	create "$@";;
		delete) 	delete "$@";;
		drop) 		drop "$@";;
		insert) 	insert "$@";;
		update) 	update "$@";;
		edit)		[ "$2" ] && $EDITOR $2@;;
		help)		help "$@";;
		print)		select_ "select" '*' "from" "$2";;
		*) 		echo "Missing or unrecognized command." 1>&2 ;;
	esac
done
