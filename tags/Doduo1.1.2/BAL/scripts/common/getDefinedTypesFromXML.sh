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

if checkFileRead $SOURCE
then
	trace "    Process file [$SOURCE]"

#	execP cat $SOURCE | execP $AWK -v NSFILTER="$NSNAME_FILTER" -f extractInfosFromXML.awk | LC_ALL=C sort -u


	#get all files id
	XMLFilesID=`egrep "^[[:space:]]*<File id=\"" $SOURCE`
	( echo "$XMLFilesID" ; cat $SOURCE ) | 
	#grep -v "incomplete=\"1\"" | 
	execP $AWK -v NSFILTER="$NSNAME_FILTER" -f extractInfosFromXML.awk #| LC_ALL=C sort -u

fi
trace "    End $0 for [$*]"
