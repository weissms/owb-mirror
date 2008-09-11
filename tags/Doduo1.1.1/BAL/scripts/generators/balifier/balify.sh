#!/bin/bash
#
#INPUT:
#   $* : a line of $INPUT_LIST
#
# generated files will go as this:
#   BAL/OWBAL/Concretizations/Module/Impl/OWBCClass.h   (/ .cpp )


. ${0%/*}/defines.sh

BC_DIR=`checkArg "$1"`
WK_FILE=`checkArg "$2"`
BC_NAME="$3"

BC_FILE=`constructBALName "$*"`
mkdir -p "$BC_DIR"
debug "Concretization: WK=[$WK_FILE] BC=[$BC_FILE]"


BC_NAMESPACE=`getNamespaceFromBALPath $BC_DIR`
debug "BC_NAMESPACE=[$BC_NAMESPACE]"


    trace "    Process file [$WK_FILE]"
    execP "cp $WK_FILE $BC_FILE"
    #!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! STAGE 0
    execP "$BALIFIER_PATH/patcher.sh 0 $BC_FILE"

#    CODE=`execP cat "$BC_FILE" | execP reformat`
    #lighter reformat:only join lines if no '{' after namespace
    CODE=`execP cat "$BC_FILE" | gawk '/^[[:space:]]*namespace[[:space:]]+/{ if (match($0,"{")>0) print; else printf($0);next; } {print} '`


#    CODE=`echo "$CODE" | execP gawk -f $NAMESPACER_AWK`
#    debug "namespacered<$CODE>namespacered"

    CODE=`echo "$CODE" | execP $AWK -v BCNAMESPACE="$BC_NAMESPACE" -f $BALIFIER_PATH/changeNameSpace.awk `
#    debug "namespaced<$CODE>namespaced"

    echo "$CODE" > $BC_FILE
    #!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! STAGE 1
    execP "$BALIFIER_PATH/patcher.sh 1 $BC_FILE"

    #!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! STAGE _
    execP "$BALIFIER_PATH/patcher.sh _ $BC_FILE"


    trace "    Generated file [$BC_FILE]"

