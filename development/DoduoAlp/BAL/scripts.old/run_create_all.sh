#!/bin/bash

#takes input files (and path and module) from list_BI.txt file
#generate BI/BC files
#execute: "cat non_generatedBI_files.txt | sort -u" to see the list of input files that can't generate a BI

#takes only the $1 first entries (if empty, takes 20)
nb=$1
if [ -z "$nb" ];
then
	nb=20
fi
HEAD="head -n $nb"

FILTER="ALL"
if ! [ -z "$2" ];
then
	if [ "$2" == "BI" ];
	then
		FILTER="BI"
	elif [ "$2" == "BC" ];
	then
		FILTER="BC"
	fi
fi

cat platform_files_list.txt | egrep -v "^#" | egrep -v "^$" | $HEAD | while read p wk fname
do
#	echo "[$p] [$wk] [$fname]"
	file="$wk"
	if echo "$p" | grep -q "/Interfaces/"
	then
		if [ "$FILTER" != "BC" ]
		then
			module=`echo "$p" | sed 's/^.*Interfaces\/\(.*\)\/[^\/]*$/\1/'`
			path=`echo "$p" | sed 's/Interfaces\/.*$//'`
#			echo "**** file=[$file] path=[$path] module=[$module]"
			CMD="./bal_create_BI.sh $file $path $module $fname"
			echo "+++ execute: $CMD"
			$CMD
		fi
	elif echo "$p" | grep -q "/Concretizations/"
	then
		if [ "$FILTER" != "BI" ]
		then
			impl=`echo "$p" | sed 's/^.*\///'`
			module=`echo "$p" | sed 's/.*\/Concretizations\/\(.*\)\/[^\/]*$/\1/'`
			path=`echo "$p" | sed 's/Concretizations\/.*$//'`
#			echo "BC not implemented for now: **** file=[$file] path=[$path] module=[$module] impl=[$impl]"
			CMD="./bal_create_BC.sh $file $path $module $impl $fname"
			echo "+++ execute: $CMD"
			$CMD
		fi
	else
		echo "WARN: $p skiped: doesn't contain valid keyword (Interfaces/Concretizations)"
	fi
done


#do next stages by hand
#exit 0

echo
echo "################ get defined class names ################"
echo
#once all BI are created, get class/types names that must be changed:
./getDefinedClassesName.sh

echo
echo "################ update defined class names ################"
echo
#and search/replace all of them with BI starting names
./updateDefinedClassesName.sh 2>&1



echo
echo "################ update defined BI includes  ################"
echo
#and search/replace all of them with BI starting names
./updateBIIncludes.sh 2>&1

echo
echo "############## reparent BI to BC ##########################"
echo
./makeBIheritBC.sh 2>&1

echo
echo "############## PostProcess for debugging ##########################"
echo
./postProcessDebug.sh 2>&1


#work directly on ../
#echo
#echo "################ propagate all .h .cpp to right position ################"
#echo
#and finally propagate all
#./propageAll.sh






