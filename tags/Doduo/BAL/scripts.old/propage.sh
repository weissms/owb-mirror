#!/bin/bash
#copy from tests/BAL to ../BAL/

for INPUT in $*
do
	ORIG="$INPUT"
	FILE=`echo "$INPUT" | sed 's/^tests/..\/..\//'`
	DIR=`echo "$FILE" | sed 's/\/[^\/]*$//'`
	mkdir -p $DIR
	rm $FILE -f 2>/dev/null
	cp -pdr $ORIG $FILE
	echo "	Copy [$ORIG] to [$FILE]"
done
