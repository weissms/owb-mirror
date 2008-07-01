#!/bin/bash

. ${0%/*}/defines.sh

nb=$1
if [ -z "$nb" ];
then
    nb=20
fi
HEAD="head -n $nb"

FILTER="BC"

cat $INPUT_LIST | grep "/Concretizations/" | egrep -v "^#" | egrep -v "^$" | 
#grep "Widget.h" |
while read line
do
	$BALIFIER_PATH/balify.sh $line
done



#cat $INPUT_LIST |egrep -v "^#" | egrep -v "^$" | $HEAD | while read p wk fname
#do
##   echo "[$p] [$wk] [$fname]"
#    file="$wk"
#    if echo "$p" | grep -q "/Concretizations/"
#    then
#            impl=`echo "$p" | sed 's/^.*\///'`
#            module=`echo "$p" | sed 's/.*\/Concretizations\/\(.*\)\/[^\/]*$/\1/'`
#            path=`echo "$p" | sed 's/Concretizations\/.*$//'`
##           echo "BC not implemented for now: **** file=[$file] path=[$path] module=[$module] impl=[$impl]"
#            CMD="balifier/balify_h.sh $file $path $module $impl $fname"
#            trace "+++ execute: $CMD"
#            $CMD
#	fi
#done
