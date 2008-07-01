#!/bin/bash
#This script will only call gccxml with right options

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
	echo -e "$CPP\t$XML" >>cpp_xml.txt

#	echo "FMD: [$CPP] => [$XML] ($OPTIONS_INPUT)"
	#echo "gccxml -fxml=$XML $OPTIONS" > /dev/stderr
	gccxml -fxml=${XML} ${OPTIONS}
	RS=$?
	if [ $RS -eq 0 ]
	then
		echo "Generated to $XML" > $OBJ
		echo "gccxml -fxml=$XML $OPTIONS" >> $OBJ
	fi
	exit $RS
fi

) 
