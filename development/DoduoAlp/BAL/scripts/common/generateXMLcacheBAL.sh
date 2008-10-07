#!/bin/bash
#
# Generates caches only for BAL .cpp files (from inputfile)
#
# $1: if present use it instead of "cat" to get things from input file
	
#. ../common/defines.sh
. ${0%/*}/defines.sh

if [ -z "$1" ]
then
	CAT="cat"
else
	CAT="$1"
fi

$CAT $XML_BUILD_CPP | egrep "/BAL/OWBAL/|/BAL/WKAL/" | while read CPP XML; do 
	$COMMON_PATH/generateXMLfromCpp.sh "$CPP" >/dev/null
done
