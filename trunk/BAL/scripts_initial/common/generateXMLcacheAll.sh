#!/bin/bash
#
# Generates all XML cache
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

$CAT $XML_BUILD_CPP |  while read CPP XML; do 
	$COMMON_PATH/generateXMLfromCpp.sh "$CPP" >/dev/null
done
