#!/bin/bash
#generate a 'fakedDeepsee.h' that will define to void all DeepSee macros used in sources found from this script directory.
#$1: the generated header

GENERATED_HEADER=$1
if [ -z "${GENERATED_HEADER}" ];
then
	echo "ERROR: You must provide an output filename for generated header file." >&2
	exit 1
fi

if [ -f ${GENERATED_HEADER} ];
then
	echo "output file already exists, backing it up into ${GENERATED_HEADER}.bak ."
	mv ${GENERATED_HEADER} ${GENERATED_HEADER}.bak
fi


#stuff for portability on MAC system ('find' command is not portable as it from Linux to Darwin systems)
OPT_FIND=""
OPT_FILTER="\\"
UNAME=`uname`
if [ "$UNAME" = "Darwin" ] ;
then
	OPT_FIND="-E"
	OPT_FILTER=""
fi


SRC_DIR=`echo $0 | sed 's/[^\/]*$/.\//'`
SRC_FILTER=".*\\.cpp\$${OPT_FILTER}|.*\\.h\$${OPT_FILTER}|.*\\.cxx\$${OPT_FILTER}|.*\\.hxx$"

(
	echo "//Faked deepsee.h:"
       	echo -n "//	generated on: " ; date 
	echo "//	from sources (${SRC_FILTER}) found under: $SRC_DIR"
	echo "//	by script: $0"
	echo "//To regenerate this header, just re-run $0, or delete the file and re-configure cmake if it uses the DeepSee block."
	echo "//"
	echo "//You can get a fresh DeepSee from svn at http://www.sand-labs.org/deepsee/svn/trunk/"
	echo "//And visit DeepSee project homepage at http://www.sand-labs.org/deepsee/"
	echo "//"
	echo "//This file may be include by #include DEEPSEE_INCLUDE if your build system defined DEEPSEE_INCLUDE=\"${GENERATED_HEADER}\". It may be the case if you don't compile with DeepSee support. Else (if you enabled DeepSee), your build system may define the regular DEEPSEE_INCLUDE=\"deepsee/deepsee.h\" and this generated file will not be used."
	echo
	echo

	find ${OPT_FIND} ${SRC_DIR} -iregex "${SRC_FILTER}" -exec egrep "(^|[^[:alnum:]_])DS_" {} \;	|\
		egrep "(^|[^[:alnum:]_])DS_" $file    |\
		sed "s/.*[[:blank:]]*\(DS_[[:alnum:]_]*\).*/#define \1(args...) /g"	|\
		sort -u
	echo
	echo

	)>${GENERATED_HEADER}
