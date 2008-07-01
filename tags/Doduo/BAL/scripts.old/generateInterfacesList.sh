#!/bin/bash

LISTBI="list_BI.txt"


for inputPath in ../../WebCore/platform/ ../../WebCore/svg/graphics/ ../../JavaScriptCore/wtf/ 
do
	headers=`find $inputPath -name '*.h' | egrep -v "/qt/|/qt4/|/wx/|/win/|/mac/|/cg/|/filters/" `

	for header in $headers
	do
		#FIXME: choose between WKAL and OWBAL
		path="../WKAL"

		module=""
		impl=""

		#try to get module/path from list_BI.txt
		found=`cat $LISTBI | egrep "^[#]?$header[[:space:]]"`
		if ! [ -z "$found" ]
		then
			path=`echo "$found" | cut -f 2`
			module=`echo "$found" | cut -f 3`
		fi
		if [ -z "$module" ]
		then
			if echo "$header" | grep -q "/graphics/";
			then
				module="Graphics";
			elif echo "$header" | grep -q "/network/";
			then
				path="../OWBAL"
				module="Network"
			elif echo "$header" | egrep -q "/text/|/unicode/"
			then
				path="../OWBAL"
				module="Internationization"
			elif echo "$header" | grep -q "/image-decoders/"
			then
				path="../OWBAL"
				module="ImageDecoder"
			elif echo "$header" | grep -q "/sql/"
			then
				path="../OWBAL"
				module="Database"
			fi

			if [ -z "$module" ];
			then
				module="Common"
			fi
		fi

		if echo "$header" | grep -q "/gtk/"
		then
			impl="gtk"
		elif echo "$header" | grep -q "/cairo/"
		then
			impl="cairo"
		fi

		if ! [ -z "$found" ];
		then
			echo -n "MARIO: "
		fi
		if [ -z "$impl" ]
		then	
			echo -e "[$header]\t=> $path/Interfaces/$module/"
		else
			echo -e "[$header]\t=> $path/Concretization/$module/WK/$impl/"
		fi
	
	done
done 


