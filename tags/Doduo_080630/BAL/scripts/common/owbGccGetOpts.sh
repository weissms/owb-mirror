#!/bin/bash
#This script will get options for gccxml and write the command to a file, without executing it.

OPTIONS_INPUT="$*"
(
if echo "$OPTIONS_INPUT" | egrep -q " -c ";
then
	OPTIONS=`echo "$OPTIONS_INPUT" |\
		sed 's/ -pthread / /' |\
		sed 's/ -o  *[^ ]*/ /' |\
		sed 's/ -c / /'`
	
	CPP=`echo "$OPTIONS_INPUT" | sed 's/^.*[^\\] //g'`
	OBJ=`echo "$OPTIONS_INPUT" | sed 's/^.*-o  *\([^ ]*\).*$/\1/g' `
	XML=`echo "$OBJ" | sed 's/\.o$/.xml/'`
	SHF=`echo "$OBJ" | sed 's/\.o$/.sh/'`
	echo -e "$CPP\t$XML" >> $XML_BUILD_CPP_UNSORTED
	echo -e "$CPP\t$SHF" >> $XMS_BUILD_SHF_UNSORTED
	CMD="gccxml -fxml=${XML} ${OPTIONS}"
	echo "$CMD" > $SHF

	echo "$CPP => $SHF" > $OBJ
fi

) 
