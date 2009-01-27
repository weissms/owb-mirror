#!/bin/bash

#This script generates a Bal Interface (BI file) header from a WebKit header + FIXME specialisation

#example of use:
# ./bal_create_BI.sh ../../WebCore/platform/Widget.h ../WKAL/ Graphics
# ./bal_create_BI.sh tests/test.h ../OWBAL/ Graphics

#INPUT:
#	$1 : WebKit header file path name
#	$2 : BAL path (somepath/BAL/OWBAL/ or somepath/BAL/WKAL/)
#	$3 : module name
#	[$4]: BI file name (if we don't want to generate one, take this)
#
# generated files will go as this:
#	BAL/OWBAL/Interfaces/Module/OWBIClass.h

WK_FILE="$1"
BAL_PATH="$2"
BAL_MODULE="$3"
BI_FNAME="$4"

. bal_scripts_common.sh


#debug: create a BAL in tests/
BAL_PATH=`echo "$BAL_PATH" | sed 's/\.\.\///g'`
BAL_PATH="$PATH_BAL/$BAL_PATH"
#echo "WARNING: BAL_PATH changed to [$BAL_PATH] for tests"


function usage()
{
echo "usage:"
echo -e "\t$0 WebKitHeaderPath BAL_path BAL_module"
}

#***************** Make checks for input args ***************************

checkReadableFile "$WK_FILE" "a WebKit"
if [ -z "$BAL_PATH" ];
then
error "you must provide a destination BAL PATH"
usage
exit 1
fi

if [ -z "$BAL_MODULE" ]
then
error "you must provide a BAL module name"
usage
exit 5
fi


#*************** create or check the destination BI directory **********
BI_DIR="$BAL_PATH/Interfaces/$BAL_MODULE" 

checkWritableDir "$BI_DIR"
res=$?
if [ $res -eq 2 ]
then
debug "Create $BI_DIR directory"
mkdir -p "$BI_DIR"
fi
checkWritableDir "$BI_DIR" || { error "can't get a writing "$BI_DIR" directory." ; exit 4 ; }


WK_FNAME=`echo $WK_FILE | sed 's/^.*\///g'`
#	BI_PREFIX=`echo $BI_NAMESPACE | head -c 2`
#	BI_PREFIX="${BI_PREFIX}BI"
BI_PREFIX="BI"
if [ -z "$BI_FNAME" ];
then	
BI_FILE="$BI_DIR/${BI_PREFIX}${WK_FNAME}"
else
BI_FILE="$BI_DIR/${BI_FNAME}"
fi
#echo "WK=$WK_FILE BI=$BI_FILE"

#************** extract needed infos from the WK file *****************

CODE=`./preProcessDebug.sh "$WK_FILE"`



BI_CLASS_NAME=`getClassNameFromWK "$CODE"`
BI_NAMESPACE=`getNamespace $BAL_PATH `
#debug "BI_CLASS_NAME=[$BI_CLASS_NAME]"

if [ -z "$BI_CLASS_NAME" ];
then
echo "WARNCLASS: no class found for $WK_FILE" | tee -a non_generatedBI_files.txt
else
#debug "Process file $WK_FILE"

CODE_WITHOUT_CLASSES=`getCodeWithoutClassesFromWK "$CODE" "$BI_CLASS_NAME"`
#	debug "<code without classes[ $CODE_WITHOUT_CLASSES ] without class>"


BI_CODE="$CODE_WITHOUT_CLASSES"

#remove code definition that from BI
BI_CODE=`removeCodeDefinition "$BI_CODE"`

#	debug "<BI code without[ $BI_CODE ] without >"

EXTERNAL_CLASSES=`getExternalClassesFromWK "$CODE" `
#	debug "external classes: " "$EXTERNAL_CLASSES"

if [ 0 -eq 1 ];
then
	debug "DEBUG: input reformated:<"
	echo "$CODE" | reformatWK
	debug ">reformated"
fi

for CLASS in $BI_CLASS_NAME
do
	debug "	Extract infos for class [$CLASS]"

	CLASS_CODE=`getClassCodeFromWK "$CODE" "$CLASS"`
#		debug "<class code[ $CLASS_CODE ]class code>"



	CLASS_CODE_INLINED=`inlineClassCode "$CLASS" "$CLASS_CODE"`
#		debug "<inlined class code[ $CLASS_CODE_INLINED ]inlined>"


#debug "External classes = [$EXTERNAL_CLASSES]"

	#remove lines that use 'specific extern classes':
	CLASS_CODE_COMMON="$CLASS_CODE_INLINED"
	for eclass in $EXTERNAL_CLASSES
	do
		CLASS_CODE_COMMON=`echo "$CLASS_CODE_COMMON" | egrep -v "[^a-zA-Z0-9_]$eclass[^a-zA-Z0-9_]"`
	done
#		debug "<common code[ $CLASS_CODE_COMMON ]common>"

	#remove doublons, regroup by public/protected/private
	#don't show private (0)
	CLASS_CODE_REGROUPED=`regroupClassCode "$CLASS" "$CLASS_CODE_COMMON" "0"`
#		debug "<regrouped code[ $CLASS_CODE_REGROUPED ]regrouped>"


	#add virtual (pure if needed)
	#force VIRTUAL (1) in BI
	CLASS_CODE_VIRTUALIZED=`virtualizeClassCode "$CLASS" "$CLASS_CODE_REGROUPED" "1"`
#		debug "<virtualized code[ $CLASS_CODE_VIRTUALIZED ]virtualized>"

	CLASS_CODE_DOCUMENTED=`documenteClassCode "$CLASS" "$CLASS_CODE_VIRTUALIZED"`
#		debug "<documented code[ $CLASS_CODE_DOCUMENTED ] documented>"

	CLASS_CODE="$CLASS_CODE_DOCUMENTED"
#		debug "<class code [ $CLASS_CODE ] clascode>"


#	CLASS_CODE=`removeCovariantMethods "$CLASS" "$CLASS_CODE"`
#	debug "<covariant [ $CLASS_CODE] covariant>"

	#duplicate declaration, and add prefix for following name use
#	CLASS_CODE=`prefixClassName "$CLASS" "$CLASS_CODE" "BI"`
#	echo "$CLASS: [$CLASS_CODE]"

#		debug "<BI BEFORE remplacing $CLASS [$BI_CODE] BI before $CLASS>"
	#reconstruct the BI code:
	BI_CODE_BEFORE=`echo "$BI_CODE" | awk "/^BAL_CLASS_$CLASS\\y/{exit;}/.*/{print}"`
	BI_CODE_AFTER=`echo "$BI_CODE" | awk "BEGIN{after=0;}/.*/{if(after==1)print;}/^BAL_CLASS_$CLASS\\y/{after=1;}"`
	BI_CODE=`echo -e "${BI_CODE_BEFORE}\n${CLASS_CODE}\n${BI_CODE_AFTER}"`


done

#	debug "<BI after classes[ $BI_CODE ] BIafterClasses>"


#fix namespaces and #includes
BI_CODE=`namespaceClassCode "$BI_CODE" "$BI_NAMESPACE"`
#BI_CODE=`echo "$BI_CODE" | awk "/.*/{print}/^[[:space:]]*namespace[[:space:]]+$BI_NAMESPACE/{print \"using namespace WebCore;\";print \"using namespace WTF;\";}"`
#	debug "<namespaced code[ $BI_CODE ]namespaced>"





	#write the BI file
	echo -n "" > $BI_FILE
	cat templates/license.txt >> $BI_FILE
	echo "$BI_CODE" >> $BI_FILE
	echo "" >> $BI_FILE
	astyle -s -b -y -C -K -N -f -P -p -V -n $BI_FILE >/dev/null

#	debug "		=> generated file [$BI_FILE]"
fi

#write infos that may be used for BC generators:
#$WK_FILE
#$BI_FILE
#$BI_CLASSES_NAME
mv $BI_INFOS_FILE $BI_INFOS_FILE.$$
( echo -e $WK_FILE"\t"$BI_FILE"\t"$BI_CLASS_NAME" " ; cat $BI_INFOS_FILE.$$ ) |  sort -k 1,1 -u > $BI_INFOS_FILE
rm $BI_INFOS_FILE.$$

mv $BI_INCLUDES_FILE $BI_INCLUDES_FILE.$$
WK_SHORT=`echo $WK_FILE | sed 's/.*\///'`
BI_SHORT=`echo $BI_FILE | sed 's/.*\///'`
( echo -e $WK_SHORT"\t"$BI_SHORT ; cat $BI_INCLUDES_FILE.$$ ) | sort -k 1,1 -u > $BI_INCLUDES_FILE
rm $BI_INCLUDES_FILE.$$


echo "##### GENERATEDFILE=  [$BI_FILE]"
echo "$BI_FILE" >> $GENS_FILE
