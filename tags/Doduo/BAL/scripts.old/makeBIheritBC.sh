#!/bin/bash

#parse BAL/ to get all BI*.h 
#then search/remplace mother classes to make them include BC instead of BI (else, pure virtual methods won't be implemented in derivated classes)

. bal_scripts_common.sh

error "STAGE SKIPED : $0"
exit 0

#OWBI=`find $PATH_BAL/OWBAL/Interfaces/ -name 'BI*\.h' -type f`
#WKBI=`find $PATH_BAL/WKAL/Interfaces/ -name 'BI*\.h' -type f`



function getBIIncludeFromClass()
{
	BICLASS="$1"
	WKCLASS=`echo "$BICLASS" | sed 's/^BI//'`
	INCLUDE=`cat $BI_INFOS_FILE | egrep "[[:space:]]$WKCLASS[[:space:]]" | cut -f 2 | sed 's/..*\///'`
	if [ -z "$INCLUDE" ]
	then
		echo "************** ERROR ********* no BI info found for WKclass [$WKCLASS]"
	fi
#	echo "INCLUDE=[$INCLUDE] for [$BICLASS]" >&2
#	cat $BI_INCLUDES_FILE | egrep "[[:space:]]+$INCLUDE$" >&2
	INCLUDEBI=`cat $BI_INCLUDES_FILE | egrep "[[:space:]]+$INCLUDE$" | cut -f 2`
	if [ -z "$INCLUDEBI" ]
	then
		echo ""
		error "INCLUDE=[$INCLUDE] for [$BICLASS] is not yet available" >&2
	else
		INCLUDEBC=`echo "$INCLUDEBI" | sed 's/\([\/[:space:]]*\)BI/\1BC/'`

		FULLPATH_BCH=`find $PATH_BAL/OWBAL/ -name "$INCLUDEBC"`
		if [ -z "$FULLPATH_BCH" ];
		then
			FULLPATH_BCH=`find $PATH_BAL/WKAL/ -name "$INCLUDEBC"`
		fi
		echo "$INCLUDEBI $INCLUDEBC $FULLPATH_BCH"
		debug "INCLUDE=[$INCLUDE] for [$BICLASS] :BI=[$INCLUDEBI]  BC=[$INCLUDEBC] BCH=[$FULLPATH_BCH]" >&2
	fi
}


TMPAWK="/tmp/$0.tmpawk.$$"

TMPAWKINCLUDE="/tmp/$0.tmpawki.$$"
	(
		echo "/^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\y(class|struct)\y[[:space:]]*[a-zA-Z0-9_]+[[:space:]]*:/{"
		echo "	\$0=gensub(\"^.*:\",\"\",\"g\");"
		echo "	\$0=gensub(\"([:,[:space:]]+(public|protected|private)?[[:space:]]*)\",\" \",\"g\");"
		echo "  \$0=gensub(\"[[:space:]]+[^B][[:space:]]+\",\" \",\"g\");"
		echo "  \$0=gensub(\"[[:space:]]+[^B][a-zA-Z0-9_]*([[:space:]]*<[[:space:]]*[a-zA-Z0-9_]+[[:space:]]*>)?\",\" \",\"g\");"
		echo "  \$0=gensub(\"[[:space:]]+B[^I][a-zA-Z0-9_]*([[:space:]]*<[[:space:]]*[a-zA-Z0-9_]+[[:space:]]*>)?\",\" \",\"g\");"
		echo "	print;"
		echo "}"
	) >$TMPAWKINCLUDE

#for BI_FILE in $OWBI $WKBI ;
for BI_FILE in `cat $GENS_FILE | grep "/Interfaces/"`
do
	debug  "work on $BI_FILE : " >&2
	#get mother classes
		MOTHERS=`cat $BI_FILE | awk -f $TMPAWKINCLUDE `
		echo "MOTHERS=[$MOTHERS]"
#		for MOTHER in $MOTHERS
#		do
#			if echo $MOTHER | egrep -q "^BI[a-zA-Z0-9_]*$"
#			then
#				echo "[$MOTHER] is a good mother"
#			else
#				echo "[$MOTHER] is NOT a good mother"
#			fi
#		done

	CINC=`
	for MOTHER in $MOTHERS
	do
		getBIIncludeFromClass $MOTHER | while read BI BC BCH
		do
			echo "$BI $BC $BCH"
		done
	done
	`
	#list of classes that are redefined in BC mother headers
	BC_BICLASSES=""
	BI_FILE2="${BI_FILE}.bal.$$"
	(
        echo "BEGIN{nbLinesMod=0;}"
        echo "/^[[:space:]]*#include[[:space:]]+/{"
        echo "  orig=\$0;"
		echo "$CINC" | while read BI BC BCH
		do
#				echo "BI=[$BI] BC=[$BC]" >&2
            	echo "	\$0=gensub(\"([\\\"<])\\\\y$BI\\\\y([\\\">])\",\"\\\\1$BC\\\\2\",\"g\",\$0);"
        done
        echo "	if (\$0 != orig){nbLinesMod+=1;}"
        echo "}"

		echo "/.*/{"
		echo "$CINC" | while read BI BC BCH
		do
			BC_BIC=`cat $BCH | awk '/^#define/ && \$2 == "BC"\$3 {print \$3;}'`
			debug "BIC=$BC_BIC" >&2
			for CLASS in $BC_BIC
			do
				BI="BI$CLASS"
				BC="BC$CLASS"
   		        #CHAR="[:space:]=+\\\\*\\\\-\\\\/:\\\\&\\\\~\\\\{\\\\}\\\\[\\\\]\\\\(\\\\)"
				CHAR="^A-Za-z0-9_"
	            echo "	\$0=gensub(\"([$CHAR]*)\\\\y$BI\\\\y([$CHAR]*)\",\"\\\\1$BC\\\\2\",\"g\",\$0);"
			done
		done
		echo "}"

		echo "/^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\y(class|struct)\y[[:space:]]*[a-zA-Z0-9_]+[[:space:]]*:/{"
		echo "	\$0=gensub(\":[[:space:]]*(public|protected|private)?[[:space:]]*BI\",\": \\\\1 BC\",\"g\");"
		echo "	\$0=gensub(\",[[:space:]]*(public|protected|private)?[[:space:]]*BI\",\", \\\\1 BC\",\"g\");"
#		echo "	print \"----> \" \$0 >\"/dev/stderr\";"
		echo "}"
		echo "/.*/{print}"
        echo "END{print \"  nb includes modified:\"nbLinesMod >\"/dev/stderr\"}"
	) >$TMPAWK
#replace BI by BC for mother class
		mv $BI_FILE $BI_FILE2
		cat $BI_FILE2 | awk -f $TMPAWK > $BI_FILE
		rm $BI_FILE2
done
rm $TMPAWK
rm $TMPAWKINCLUDE
