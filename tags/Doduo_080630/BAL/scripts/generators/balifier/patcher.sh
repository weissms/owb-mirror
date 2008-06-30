#!/bin/bash
#	Patches all files that must be for this stage
#INPUT:
#   $1 : current stage (0 for initial, 1-n for 'in process', '_' at end)
#	$2 : filename we are working on


#. ../common/defines.sh
. ${0%/*}/defines.sh

STAGE=`checkArg "$1"`
BC_FILE=`checkArg "$2"`

if checkFileRead $BC_FILE
then
	grep "$BC_FILE[[:space:]]$STAGE:" $PATCHES_INDEX | grep -v "^$" | grep -v "^#" | while read FNAME LEVPATCH
	do
		PATCH=`echo "$LEVPATCH" | sed 's/^.*://'`
	#	debug "found patch [$PATCH] for stage $STAGE for [$FNAME]"
		( execP "patch -d $OWB_PATH -p0 <$PATCHES_PATH/$PATCH " )
	done
else
	error "can't read [$BC_FILE] for $0 at stage $STAGE"
fi
