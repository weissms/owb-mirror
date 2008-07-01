#!/bin/bash
#balify a HEADER
#
#INPUT:
#   $1 : WebKit file path name
#   $2 : BAL path (somepath/BAL/OWBAL/ or somepath/BAL/WKAL/)
#   $3 : module name
#   $4 : implementation name
#   [$5]: BC file name (if we don't want to generate one, take this)
#
# generated files will go as this:
#   BAL/OWBAL/Concretizations/Module/Impl/OWBCClass.h   (/ .cpp )


. ../common/defines.sh

WK_FILE=`checkArg "$1"`
BAL_PATHTO=`checkArg "$2"`
BAL_MODULE=`checkArg "$3"`
BAL_IMPL=`checkArg "$4"`
BC_FNAME="$5"

if ! checkFileRead $WK_FILE
then
	fatal "$WK_FILE"
fi

BC_DIR="$BAL_PATHTO/Concretizations/$BAL_MODULE/$BAL_IMPL"
mkdir -p "$BC_DIR"
WK_FNAME=`echo $WK_FILE | sed 's/^.*\///g'`
BC_FILE=`constructBALName "$WK_FILE" "$BAL_PATHTO" "$BAL_MODULE" "$BAL_IMPL" "$BC_FNAME"`
debug "Concretization: WK=[$WK_FILE] BC=[$BC_FILE]"


BC_NAMESPACE=`getNamespaceFromBALPath $BAL_PATHTO`
debug "BC_NAMESPACE=[$BC_NAMESPACE]"


    trace "    Process file [$WK_FILE]"
    CODE=`execP cat "$WK_FILE" | execP reformat`
	CODE=`echo "$CODE" | execP $AWK -v BCNAMESPACE="$BC_NAMESPACE" -f $BALIFIER_PATH/changeNameSpace.awk `
	debug "namespaced<$CODE>namespaced"


