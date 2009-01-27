#!/bin/bash
#
# outputs the XML file from the CPP name
# first check in cache if XML was already generated. If yes, just display it, else, generate it before displaying
#
# $1 : CPP file name
# 
	
#. ../common/defines.sh
. ${0%/*}/defines.sh


CPP_FNAME="$1"
if [ -z "$CPP_FNAME" ]
then
    fatal "you must give a cpp filename"
fi

matches=`grep "$CPP_FNAME" $XML_BUILD_CPP`
if [ -z "$matches" ];
then
	fatal "[$CPP_FNAME] gives no results... check the name or have a look at [$XML_BUILD_CPP]"
fi
matches_nb=`echo "$matches" | wc -l `
if [ $matches_nb -gt 1 ];
then
	fatal "[$CPP_FNAME] gives more than 1 xml fname. Lines found are: [$matches]"
fi

XMLNAME=`echo "$matches" | cut -f 2`
XMLNAME="$XML_BUILD_PATH/$XMLNAME"

if ! [ -r "$XMLNAME" ]
then
	debug "[$XMLNAME] cache doesn't exist yet. will generate it right now"
	matches=`grep "$CPP_FNAME" $XML_BUILD_SHF`
	matches_nb=`echo "$matches" | wc -l `
	if [ $matches_nb -gt 1 ];
	then
		fatal "[$CPP_FNAME] gives more than 1 xml cmd. Lines found are: [$matches]"
	fi

	SHFNAME=`echo "$matches" | cut -f 2`
	SHFNAME="$XML_BUILD_PATH/$SHFNAME"

	( cd $XML_BUILD_PATH;bash $SHFNAME )
	RT=$?
	if [ $RT -ne 0 ]
	then
		fatal "[$CPP_FNAME] => [$SHFNAME]. Script exited with value $RT"
	fi
else
	debug "[$XMLNAME] already exists, displaying it."
fi

cat "${XMLNAME}"
