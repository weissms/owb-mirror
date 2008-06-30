#!/bin/bash
#
# get XML_PARSED, and extract infos for BAL/nonBAL in 2 separate files
#
#. ../common/defines.sh
. ${0%/*}/defines.sh

if ! checkFileRead $XML_PARSED
then
	fatal "$XML_PARSED must be generated first"
fi
if ! checkFileRead $INPUT_LIST
then
	fatal "$INPUT_LIST is mandatory"
fi
if ! checkFileRead $NONBAL_H_LIST
then
	fatal "$NONBAL_H_LIST is mandatory"
fi

#process all filenames to dereference them if needed, and provide a list of WebKit.filename => XML.filemame
if true;
then
	trace "+++++++++++++++++++++"
	trace "Dereference if needed"
	trace "+++++++++++++++++++++"
	XML_FILES=`cut -f 1 $XML_PARSED`
	GENED_PATH=`grep "owbGccBuild/generated_link/" $XML_PARSED | head -n 1 | sed 's/generated_link\/.*$//'`
	LINK_FILES=`find $GENED_PATH -type l`
	TOT_FILES=`echo "$XML_FILES" "$LINK_FILES" | LC_ALL=C sort -u`
	for file in $TOT_FILES
	do
		if [ -h $file ];
		then
			tgt=`getLink "$file"`
	#		debug "$file => is a link to [$tgt]"
			echo -e "$file\t$tgt"
		else
	#		debug "$file ..."
			echo -e "$file\t$file"
		fi
	done  > $XML_FILES_DEREF
fi


function extractFor(){
	while read fileName; 
	do 
		fname=`echo "$fileName" | sed 's/^\(\.\.\/\)*/\//'`
#		RESULT=`grep "$fname" $XML_PARSED`
#		if [ -z "$RESULT" ];
#		then
			#check with deref
			deref=`grep "$fname$" $XML_FILES_DEREF | cut -f 1`
#			debug "fname=[$fname]"
#			debug "deref=[$deref]"

			if [ -z "$deref" ]
			then
				deref2="$fname"
			else
				deref2=`echo "$deref" | sed 's/^\(\.\.\/\)*/\//g' | sed 's/\/\//\//g' | awk '{if (toto!="") toto=toto"|";toto=toto $0;}END{print toto}' `
			fi
#			debug "deref2=[$deref2]"
			RESULT=`egrep "$deref2" $XML_PARSED`
#		fi
		if [ -z "$RESULT" ];
		then
			fname2=`echo "$deref2" | sed 's/^\///'`
			debug "No types defined in XML for [$fname2] (or link [$deref2])?"
		else
			echo "$RESULT"
		fi
		nb=`echo "$RESULT" | wc -l`	
#		debug "Read Filename: [$fileName]  fname=[$fname] deref=[$deref][$deref2] => $nb lines"
	done
}



trace "++++++++++++++++++++++"
trace "Process NON BAL files:"
trace "++++++++++++++++++++++"
cat $NONBAL_H_LIST | grep -v "^#" | egrep -v "^[[:space:]]*$" | sed 's/  */\t/'|cut -f 1 | extractFor  > $NONBAL_TYPES
trace "+++++++++++++++++++++++++++++++++++"
trace "Process BAL (OWBAL and WKAL) files:"
trace "+++++++++++++++++++++++++++++++++++"
BAL_FNAMES=`cat $INPUT_LIST | grep -v "^#" | egrep -v "^[[:space:]]*$" | grep "/Concretizations/" | grep -v "\.cpp$" |  sed 's/  */\t/' | while read line; do constructBALName "$line" ; done`
echo "$BAL_FNAMES"  | extractFor > $BAL_TYPES

