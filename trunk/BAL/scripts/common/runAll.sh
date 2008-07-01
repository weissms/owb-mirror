#!/bin/bash
#read each line of $1, and if not comment or empty, execute $2 with that line as argument
#if $3 is provided, cut file line with this field(s)
#INPUT
#	$1 : file to read
#	$2 : script to execute
#	($3) : fields to pass

#. ../common/defines.sh
. ${0%/*}/defines.sh


trace "+++ Start [$0 $*]"
SOURCE=`checkArg "$1"`
SCRIPT=`checkArg "$2"`

if ! checkFileRead "$SOURCE"
then
	fatal "[$SOURCE] is not good... (doesn't exist or not readable)"
fi

if ! checkFileExec "$SCRIPT"
then
	fatal "[$SCRIPT] is not good... (doesn't exist or not executable)"
fi

CUT="cat"
if ! [ -z "$3" ]
then
	CUT="cut -f $3"
fi

#HEAD="tail -n 20"
HEAD="cat"

OUTPUT="$0.result.$$"

(
	execP cat $SOURCE | egrep -v "^#" | egrep -v "^$" | $HEAD | $CUT | while read line
	do
		$SCRIPT $line 
	done
) | tee $OUTPUT

trace "+++ Ended $0 with [$*]"
