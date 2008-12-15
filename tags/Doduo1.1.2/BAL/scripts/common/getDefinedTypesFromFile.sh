#!/bin/bash
#INPUT:
#	$1: file we want to parse
#OUTPUT:
#	stdout: list of defined types, prefixed by their namespace
#	stderr: debug/error messages

#. ../common/defines.sh
. ${0%/*}/defines.sh


trace "+++ Start [$0 $*]"
SOURCE=`checkArg "$1"`

if checkFileRead $SOURCE
then
	trace "    Process file [$SOURCE]"
	CODE=`execP cat "$SOURCE" | execP reformat`
	RESULT=`echoAwkFile "$CODE" "$0.awk"`
#	echo "RESULT=[$RESULT]"
	echo "$RESULT"

fi
trace "    End $0 for [$*]"
