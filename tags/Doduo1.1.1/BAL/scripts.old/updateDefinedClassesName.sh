#!/bin/bash

#parse BAL/ to get all BI*.h 
#then search/remplace all words from $CLASSNAMES_FILE

. bal_scripts_common.sh

error "STAGE SKIPED: $0"
exit 0

#OWBI=`find $PATH_BAL/OWBAL/Interfaces/ -name 'BI*\.h' -type f`
#WKBI=`find $PATH_BAL/WKAL/Interfaces/ -name 'BI*\.h' -type f`

TMPAWK="/tmp/$0.tmpawk.$$"
	echo -n "" >$TMPAWK
	(
		echo "BEGIN{nbLinesMod=0;}"
		echo "/.*/{"
		echo "	orig=\$0;"
		cat $CLASSNAMES_FILE | while read NONBI BI
		do
#			CHAR="[:space:]=+\\\\*\\\\-\\\\/:\\\\&\\\\~\\\\{\\\\}\\\\[\\\\]\\\\(\\\\)"
			CHAR="^A-Za-z0-9_"
			echo "\$0=gensub(\"([$CHAR]*)\\\\y$NONBI\\\\y([$CHAR]*)\",\"\\\\1$BI\\\\2\",\"g\",\$0);"
		done
			echo "if (\$0 != orig){nbLinesMod+=1;}"
			echo "print "
		echo "}"
		echo "END{print \"	nb lines modified:\"nbLinesMod >\"/dev/stderr\"}"
	) >$TMPAWK

	
#for BI_FILE in $OWBI $WKBI ;
for BI_FILE in `cat $GENS_FILE | grep "/Interfaces/"`
do
	debug -n "work on $BI_FILE : " >&2
	BI_FILE2="${BI_FILE}.bal.$$"
	mv $BI_FILE $BI_FILE2
	cat $BI_FILE2 | awk -f $TMPAWK > $BI_FILE
	rm $BI_FILE2
done
rm $TMPAWK
