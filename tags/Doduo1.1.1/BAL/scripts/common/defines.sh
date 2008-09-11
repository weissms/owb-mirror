#!/bin/bash
#this script only defines data constant for BAL/scripts/
#all files are relative path from BAL/scripts/FOOBAR (FOOBAR=common/ or generators/)

SCRIPT_NAME=`echo ${0} | sed 's/^.*\///'`
SCRIPT_PATH="${0%%${SCRIPT_NAME}}"
#get absolute path from relative one
cd $SCRIPT_PATH ; SCRIPT_PATH=`pwd` ; cd - >/dev/null
#if echo "$SCRIPT_PATH" | egrep -q "^\.";
#then
#	SCRIPT_PATH=`pwd`
#fi

OWB_PATH="${SCRIPT_PATH%/BAL/*}"
BAL_PATH="${OWB_PATH}/BAL/"
#echo "Script name =[$SCRIPT_NAME] BAL_PATH=[$BAL_PATH]"


COMMON_PATH="$BAL_PATH/scripts/common/"
GENERATORS_PATH="$BAL_PATH/scripts/generators/"
DATA_PATH="$GENERATORS_PATH/data/"
GENERATED_PATH="$GENERATORS_PATH/generators/"
SKELETON_PATH="$GENERATORS_PATH/skeleton/"
BALIFIER_PATH="$GENERATORS_PATH/balifier/"
PATCHES_PATH="$BAL_PATH/scripts/patches/"
. $COMMON_PATH/functions.sh

XML_BUILD_NAME="owbGccXmlBuild"
XML_BUILD_PATH="$COMMON_PATH/$XML_BUILD_NAME"
XML_BUILD_CPP="$XML_BUILD_PATH/cpp_xml.txt"
XML_BUILD_CPP_UNSORTED="$XML_BUILD_PATH/cpp_xml.unsorted.txt"
XML_BUILD_SHF="$XML_BUILD_PATH/cpp_shf.txt"
XML_BUILD_SHF_UNSORTED="$XML_BUILD_PATH/cpp_shf.unsorted.txt"


export AWKPATH="$AWKPATH:$COMMON_PATH:$SKELETON_PATH"

#List of files that are to be balified:
INPUT_LIST="$DATA_PATH/balInputList.txt"

#list of headers that are NOT to be balified:
NONBAL_H_LIST="$DATA_PATH/nonbalFiles.txt"

#DEPRECATED
XML_PARSED="$DATA_PATH/xmlParsed"
XML_FILES_DEREF="$DATA_PATH/xmlFilesDeref.txt"

NONBAL_TYPES="$DATA_PATH/nonbalXMLTypes.txt"
BAL_TYPES="$DATA_PATH/balXMLTypes.txt"

NAMESPACER_AWK="$BALIFIER_PATH/generatedNamespacer.awk"

PATCHES_INDEX="$PATCHES_PATH/index.txt"


#define colors for NoRMal, DeBuG, TRaCe, ERRor, PipeeRRor:
COLNRM="[0;0;0m"
COLDBG="[0;0;32m"
COLTRC="[0;0;7m"
COLERR="[0;0;41m"
COLPRR="[0;0;31m"

function debug(){
	(	echo -ne "${COLDBG}DBG:${COLNRM}\t" ; echo "$*"	) >&2
}

function trace(){
	(	echo -ne "${COLDBG}TRC:${COLNRM}\t" ; echo "$*"	) >&2
}

function error(){
	(	echo -ne "${COLERR}ERR:${COLNRM}\t" ; echo "$*"	) >&2
}

function pipeError(){
	while read LINE
	do
		echo -ne "${COLPRR}PRR:${COLNRM}\t" >&2 ; echo "$LINE" >&2
	done
}

#Display an error and kill the script
function fatal(){
	error "FATAL: " $*
	error "Exiting script..."
	kill $$
}

#Exit if $1 is empty, else output it
function checkArg(){
	if [ -z "$1" ];
	then
		fatal "Missing mandatory argument."
	fi
	echo "$1"
}

#check that file exists and can be read
function checkFileRead(){
	if ! [ -r "$1" ]
	then
		if ! [ -e "$1" ]
		then
			error "File [$1] doesn't exist"
		fi
		error "File [$1] can not be read."
		return 1
	fi
	return 0
}

#check that directory exists and can be read
function checkDirRead(){
	if ! [ -d "$1" ]
	then
		error "Directory [$1] doesn't exist or is not a directory"
		return 1
	fi
	if ! [ -r "$1" ]
	then
		error "Directory [$1] can not be read."
		return 2
	fi
	if ! [ -x "$1" ]
	then
		error "Directory [$1] can not be entered."
		return 3
	fi
	return 0
}

#check that file exists and can be executed
function checkFileExec(){
	if ! [ -x "$1" ]
	then
		if ! [ -e "$1" ]
		then
			error "File [$1] doesn't exist"
		fi
		error "File [$1] is not executable."
		return 1
	fi
	return 0
}


AWKPROG="igawk"
AWK="$AWKPROG -f $COMMON_PATH/defines.awk"
if $AWKPROG --version 2>&1 | head -n 2| grep -q "GNU Awk" >/dev/null
then
#	debug "$AWKPROG seem to be good"
	a=$a
else
	fatal "$AWKPROG is not found or has wrong version. Tested with [GNU Awk 3.1.6]"
fi

function execP(){
	debug "will execute [$*]"
	( ( eval $* ) 2>&1 1>&3 | pipeError ) 3>&1
}

function callAwk(){
#not supposed to be called directly
#redirect stderr to pipe, and stdout to &3. At end, inverse.
	 ( ( "$1" "$2" | $AWK "$3" "$4" ) 2>&1 1>&3 | pipeError ) 3>&1  
}
function catAwkFile(){
	callAwk "cat" "$1" "-f" "$2"
}
function echoAwkFile(){
	callAwk "echo" "$1" "-f" "$2"
}
function catAwkProg(){
	callAwk "cat" "$1" "$2" ""
}
function echoAwkProg(){
	callAwk "echo" "$1" "$2" ""
}
