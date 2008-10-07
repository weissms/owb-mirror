#!/bin/bash

. bal_scripts_common.sh

OWBI=`find $PATH_BAL/OWBAL/Interfaces/ -name 'BI*\.h' -type f`
WKBI=`find $PATH_BAL/WKAL/Interfaces/ -name 'BI*\.h' -type f`
OWBC=`find $PATH_BAL/OWBAL/Concretizations/ \( -name 'BC*\.h' -or -name 'BC*.cpp' \) -type f`
WKBC=`find $PATH_BAL/WKAL/Concretizations/ \( -name 'BC*\.h' -or -name 'BC*.cpp' \) -type f`

for B_FILE in $OWBI $WKBI $OWBC $WKBC;
do
	./propage.sh $B_FILE
done
