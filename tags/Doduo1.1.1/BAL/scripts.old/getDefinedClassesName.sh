#!/bin/bash

#parse Interfaces/ to get all BI*.h
#then search for all types/classes defined in the BAL namespace, and if they don't start with 'BI', get them
#put them in a file, that gives oldName => BIName
#This file will be used by an other script to modify all BI files

. bal_scripts_common.sh

#OWBI=`find $PATH_BAL/OWBAL/Interfaces/ -name 'BI*\.h'`
#WKBI=`find $PATH_BAL/WKAL/Interfaces/ -name 'BI*\.h'`

#for BI_FILE in $OWBI $WKBI;
for BI_FILE in `cat $GENS_FILE | grep "/Interfaces/"`
do
	BI_CODE=`cat $BI_FILE | reformatWK `
	OLD_CLASSES=`getDefinedClassesName "$BI_CODE"`
#	OLD_NONBI=`echo "$OLD_CLASSES" | grep -v "^BI"`
	OLD_NONBI=`echo "$OLD_CLASSES" | sed 's/^BI//'`
	echo -e "$BI_FILE\t:" $OLD_CLASSES "("$OLD_NONBI")"
	for NONBI in $OLD_NONBI
	do
		if cat $CLASSNAMES_FILE | egrep -q "^$NONBI[[:space:]]";
		then
			debug -e "\t$NONBI already in"
		else
			echo -e "$NONBI\tBI$NONBI" >> $CLASSNAMES_FILE
			debug -e "\tBI$NONBI added"
		fi
	done
done
