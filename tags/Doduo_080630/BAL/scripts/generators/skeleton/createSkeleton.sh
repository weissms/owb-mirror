#!/bin/bash
#
#INPUT:
#   $1: XML file
#	$2: NameSpace
#	$3: Class name we want to generate 
#

. ${0%/*}/defines.sh
#. ../common/defines.sh

XML_FILE=`checkArg "$1"`
NAMESPACE=`checkArg "$2"`
CLASS_NAME=`checkArg "$3"`


    trace "    Process XML file [$XML_FILE] for generating $NAMESPACE::$CLASS_NAME"

	execP cat "$XML_FILE" | execP $AWK -v CLASS=$CLASS_NAME -v NAMESPACE=$NAMESPACE -f createSkeleton.awk 

	trace "    Processed XML file [$XML_FILE]. generated: $NAMESPACE::$CLASS_NAME"

