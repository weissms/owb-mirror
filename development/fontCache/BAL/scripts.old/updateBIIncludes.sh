#!/bin/bash

#parse BAL/ to get all BI*.h 
#then search/remplace all includes from $BI_INCLUDES_FILE

. bal_scripts_common.sh

#OWBI=`find $PATH_BAL/OWBAL/Interfaces/ -name 'BI*\.h' -type f`
#WKBI=`find $PATH_BAL/WKAL/Interfaces/ -name 'BI*\.h' -type f`

TMPAWK="/tmp/$0.tmpawk.$$"
	echo -n "" >$TMPAWK
	(
		echo "BEGIN{nbLinesMod=0;}"
#SKIPED
#		echo "/^[[:space:]]*#include[[:space:]]+/{"
#		echo "	orig=\$0;"
#		cat $BI_INCLUDES_FILE | while read NONBI BI
#		do
#			echo "\$0=gensub(\"([\\\"<])\\\\y$NONBI\\\\y([\\\">])\",\"\\\\1$BI\\\\2\",\"g\",\$0);"
#		done
#			echo "if (\$0 != orig){nbLinesMod+=1;}"
#		echo "}"
		for BICLASS in `cat $CLASSNAMES_FILE | cut -f 2` 
		do
			BCCLASS=`echo "$BICLASS" | sed 's/^BI/BC/'`
#			echo "/^[[:space:]]*virtual[[:space:]]+$BICLASS\\y([[:space:]]*&)?[[:space:]]*/{\$0=\"//REMOVED BECAUSE COVARIANT: \" \$0}"
			echo "/^[[:space:]]*virtual[[:space:]]+$BICLASS\\y([[:space:]]*&)?[[:space:]]*/{gsub(\"^[[:space:]]*virtual[[:space:]]+$BICLASS\\\\y\",\"virtual $BCCLAS\");}"

		done
		echo "/.*/{print}"
		echo "END{print \"	nb includes modified:\"nbLinesMod >\"/dev/stderr\"}"
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
