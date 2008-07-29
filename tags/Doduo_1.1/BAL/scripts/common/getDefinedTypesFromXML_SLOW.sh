#!/bin/bash
#INPUT:
#	$1: XML file we want to parse
#	$2:	if present filter on this namespace name
#OUTPUT:
#	stdout: list of defined types, prefixed by their namespace
#	stderr: debug/error messages

#. ../common/defines.sh
. ${0%/*}/defines.sh


trace "+++ Start [$0 $*]"
SOURCE=`checkArg "$1"`
NSNAME_FILTER="$2"

function getXMLNameFromID(){
	SOURCE=$1
	ID=$2
	cat $SOURCE | egrep " id=\"$2\" " | sed 's/^.*name="\([^"]*\)".*/\1/'
}

function getXMLIDListFromType(){
	SOURCE=$1
	TYPE=$2
	cat $SOURCE | egrep "^[[:space:]]*<$TYPE " | sed 's/^.*id="\([^"]*\)".*/\1/'
}

function getXMLChildsIDListFromID(){
	SOURCE=$1
	ID=$2
	cat $SOURCE | egrep " id=\"$2\" " | sed 's/^.*members="\([^"]*\)".*/\1/'
}

function getXMLTypeFromID(){
	SOURCE=$1
	ID=$2
	cat $SOURCE | egrep " id=\"$2\" " | sed 's/^[[:space:]]*<\([^[:space:]]*\)[[:space:]].*/\1/'
}

if checkFileRead $SOURCE
then
	trace "    Process file [$SOURCE]"
#	CODE=`execP cat "$SOURCE" | execP reformat`
#	RESULT=`echoAwkFile "$CODE" "$0.awk"`
#	echo "RESULT=[$RESULT]"
	#for each namespace

	for NSID in `getXMLIDListFromType $SOURCE "Namespace"`
	do
		NSNAME=`getXMLNameFromID $SOURCE $NSID`
		if ! [ -z "$NSNAME_FILTER" ] && [ "$NSNAME_FILTER" != "$NSNAME" ]
		then
			continue;
		fi
		debug "Namespace [$NSNAME]"

		#get name of each direct childs:
		for CID in `getXMLChildsIDListFromID $SOURCE $NSID`
		do
			CNAME=`getXMLNameFromID $SOURCE $CID`
			CTYPE=`getXMLTypeFromID $SOURCE $CID`

			debug "    $CTYPE $CNAME"
			echo -e "$NSNAME\t$CTYPE\t$CNAME"

			if [ "$CTYPE" == "Enumeration" ];
			then
				enumVALS=`cat $SOURCE | igawk -v ID=$CID '@include defines.awk
					BEGIN{inRightEnum=0;
						debug("ID="ID);
					}
					(inRightEnum==1){
						debug("inside the enum:" $0);
						#!!! Buggy thing! The following line works just fine in command line, but fail here...
						#	print gensub("^.*name=\"([^\"]+)\".*","\\1","1",$0);
						#I don t know why I must split it here, but that s the only way I found to achieve it!
						name=gensub("^.*name=\"","","1",$0);
						name=gensub("\".*$","","1",name);
						trace(name);
						if (match($0,"^[[:space:]]*</Enumeration>")>0)
							exit
						else
							print name;
					}
					/^[[:space:]]*<Enumeration[[:space:]]+id="/ && (match($0,"[[:space:]]+id=\""ID"\"[[:space:]]+")>0){
						inRightEnum=1
						#debug("Find the right enum:" $0);
					}
				'`
				for VAL in $enumVALS
				do
					debug "        $VAL"
					echo -e "$NSNAME\tEnumValue\t$VAL"
				done
			fi
		done
		

	done

fi
trace "    End $0 for [$*]"
