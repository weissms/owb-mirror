#!/bin/bash

#INPUT:
#	$1 : WebKit file path name
#	$2 : BAL path (somepath/BAL/OWBAL/ or somepath/BAL/WKAL/)
#	$3 : module name
#	$4 : implementation name
#	[$5]: BC file name (if we don't want to generate one, take this)
#
# generated files will go as this:
#	BAL/OWBAL/Concretizations/Module/Impl/OWBCClass.h	(/ .cpp )

WK_FILE="$1"
BAL_PATH="$2"
BAL_MODULE="$3"
BAL_IMPL="$4"
BC_FNAME="$5"

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

if [ -z "$BAL_IMPL" ]
then
	error "you must provide a BAL implementation name"
	usage
	exit 5
fi



#*************** create or check the destination BI directory **********
BC_DIR="$BAL_PATH/Concretizations/$BAL_MODULE/$BAL_IMPL" 

checkWritableDir "$BC_DIR"
res=$?
if [ $res -eq 2 ]
then
	debug "Create $BC_DIR directory"
	mkdir -p "$BC_DIR"
fi
checkWritableDir "$BC_DIR" || { error "can't get a writing "$BC_DIR" directory." ; exit 4 ; }
mkdir -p "$BC_DIR/Link"

WK_FNAME=`echo $WK_FILE | sed 's/^.*\///g'`
#	BC_PREFIX=`echo $BC_NAMESPACE | head -c 2`
#	BC_PREFIX="${BC_PREFIX}BC"
	BC_PREFIX="BC"
if [ -z "$BC_FNAME" ];
then	
	radix=`echo "$WK_FNAME" | sed 's/\.[^\.]*$//' | sed s/$BAL_IMPL$//`
	tailer=`echo "$WK_FNAME" | sed 's/.*\(\.[^\.]*\)$/\1/'`
	BC_FILE="$BC_DIR/${BC_PREFIX}${radix}${BAL_IMPL}${tailer}"
	BC_LINK_TGT="../${BC_PREFIX}${radix}${BAL_IMPL}${tailer}"
	BC_LINK="$BC_DIR/Link/${BC_PREFIX}${radix}${tailer}"
else
	BC_FILE="$BC_DIR/${BC_FNAME}"
	linkName=`echo "$BC_FNAME" | sed s/$BAL_IMPL$tailer$//`
	BC_LINK_TGT="../$BC_FNAME"
	BC_LINK="$BC_DIR/Link/$BC_FNAME${tailer}"
fi
debug "Concretization: WK=[$WK_FILE] BC=[$BC_FILE]"




CODE=`./preProcessDebug.sh "$WK_FILE"`






#hack for (at less) GIFImageReader.h: it only contain C code, and then must NOT be altered
if echo "$CODE" | egrep -q "^[[:space:]]*namespace"
then
#start of class modification block




#get the code
AWK_CODE=`cat $AWKTOOLS removeComments.awk;echo "/.*/{print}"`
BC_CODE=`echo "$CODE" | awk "$AWK_CODE"  | reformatWK `

#debug "REFORMATED: $BC_CODE"

BC_NAMESPACE=`getNamespace $BAL_PATH`

#get the old namespace
WK_NAMESPACE=`echo "$BC_CODE" | egrep "^[[:space:]]*namespace[[:space:]]+[a-zA-Z0-9_]*[[:space:]]*{" | sed 's/^[[:space:]]*namespace[[:space:]]*\([^[:space:]{]*\).*/\1/' | tail -n 1`
debug "oldNamespace=$WK_NAMESPACE"

#change namespace to OWBAL/WKAL
#BC_CODE=`echo "$BC_CODE" | awk "/^[[:space:]]*namespace\\y/{ print \"//using namespace WebCore;//using namespace WTF;\";gsub(\"namespace[[:space:]]*WebCore\",\"namespace $BC_NAMESPACE\"); gsub(\"namespace[[:space:]]*WTF\",\"namespace $BC_NAMESPACE\"); }/WebCore::/||/WTF::/{gsub(\"WebCore::\",\"$BC_NAMESPACE::\");gsub(\"WTF::\",\"$BC_NAMESPACE::\");}/.*/{print}/^[[:space:]]*namespace\\y/{ print \"using namespace WebCore;using namespace WTF;\";}"`
BC_CODE=`echo "$BC_CODE" | awk "/^[[:space:]]*namespace\\y/{ print \"//using namespace WebCore;//using namespace WTF;\";gsub(\"namespace[[:space:]]*WebCore\",\"namespace $BC_NAMESPACE\"); }/WebCore::/{gsub(\"WebCore::\",\"$BC_NAMESPACE::\");}/.*/{print}"`
#   debug "<namespaced code[ $BC_CODE ]namespaced>"

#get all defined classes names
BC_CLASSES=`getClassNameFromWK "$BC_CODE" | sort -u `
#classes names that are used in constructors

BC_CL_CONSTR=`echo "$BC_CODE" | egrep "^[[:space:]]*\b([a-zA-Z0-9_]*)::\1\b" | sed 's/^.*\([a-zA-Z0-9_]*\)::\1[[:space:]]*(.*/\1/'`
	for CLASS in $BC_CLASSES $BC_CL_CONSTR
	do
		debug "FMD: process class [$CLASS]"
		#force all BC constructors to call their BI
		#get params to pass to BI constructor
		#add ':' if not already present
		#else, add "," to add our BI constructor 
		BC_CODE=`makeBCConstructsCallBIOnes "$CLASS" "$BC_CODE"`
	done


WK_TYPE=`echo $WK_FNAME | sed 's/.*\.\([hcp]*\)$/\1/'`
if [ "$WK_TYPE" == "cpp" ];
then
	debug "Concretization .cpp"
	

	
	#change includes to take BCclass.h instead of class.h
#	BC_CODE=`updateBCcppIncludes "$BC_CODE"`
#   debug "<includesMod code[ $BC_CODE ] IncludesMod>"
	
else
	debug "Concretization .h"



	#get all declared types (outside BC_CLASSES)
	BC_DECLARED_ALL=`getDefinedClassesName "$BC_CODE" | sort -u`
	BC_DECLARED=`
		for declared in $BC_DECLARED_ALL;
		do
			if ! echo " " $BC_CLASSES " " | egrep -q "[[:space:]]+$declared[[:space:]]+"
			then
				echo "$declared"
			fi
		done
		`
	debug "BC_CLASSES=[$BC_CLASSES]"
	debug "BC_DECLARED_ALL=[$BC_DECLARED_ALL]"
	debug "BC_DECLARED=[$BC_DECLARED]"


	#feed config.h (CONFIG_H_FILE)
	if [ "$BC_NAMESPACE" == "OWBAL" ];
	then
		BC2NS="WKAL"
	else
		BC2NS="OWBAL"
	fi

#update the config.h or not	
	if ! true;
	then
		AWK_CMD_WK="/^[[:space:]]*namespace[[:space:]]+$WK_NAMESPACE[[:space:]]*{/ ||
					/^[[:space:]]*namespace[[:space:]]+$BC2NS[[:space:]]*{[[:space:]]*\/\/using/{"
		AWK_CMD_BC="/^[[:space:]]*namespace[[:space:]]+$BC_NAMESPACE[[:space:]]*{[[:space:]]*\/\/declarations/{"
		CONFIG_MOD="0"
		for C in $BC_CLASSES
#		for C in $BC_DECLARED_ALL
		do
			if ! cat $CONFIG_H_FILE | egrep -q "^[[:space:]]*using $BC_NAMESPACE::$C;"
			then
				AWK_CMD_WK="$AWK_CMD_WK print \"    using $BC_NAMESPACE::$C;\";"
				CONFIG_MOD="1"
#				debug "add $C in $BC_NAMESPACE"
#			else
#				debug "$C already in $BC_NAMESPACE"
			fi
			if ! cat $CONFIG_H_FILE | egrep -q "^[[:space:]]*class $C;"
			then
				CONFIG_MOD="1"
				AWK_CMD_BC="$AWK_CMD_BC print \"    class $C;\";"
#				debug "add $C in $WK_NAMESPACE"
#			else
#				debug "$C already in $WK_NAMESPACE"
			fi
		done
		if [ "$CONFIG_MOD" == "1" ]
		then
			AWK_CMD_WK="$AWK_CMD_WK }"
			AWK_CMD_BC="$AWK_CMD_BC }"
			AWK_CMD="/.*/{print} $AWK_CMD_WK $AWK_CMD_BC "
#			echo "AWK_CMD=$AWK_CMD"
			cp $CONFIG_H_FILE $CONFIG_H_FILE.$$
			cat $CONFIG_H_FILE.$$ | awk "$AWK_CMD" > $CONFIG_H_FILE
			rm $CONFIG_H_FILE.$$
		fi
	fi

	#remove in classes definitions (in BIClass { public: enum Toto{};} => remove Toto from BCClass )
	BC_CODE=`removeInClassesDefs "$BC_CODE"`

	#see few lines bellow
	tacAwkCode="BEGIN{first=0;}/^[[:space:]]*#endif/{if (first==0){print ;print \"}\";"

	#for each defined class, 
	for CLASS in $BC_CLASSES $BC_DECLARED
	do
		debug "	process class [$CLASS]"
		#add its public BI$CLASS (be carefull for templates: public BI$CLASS<T>)
		BC_CODE=`processBChClass "$BC_CODE" "$CLASS"`
		#add namespace WebCore/using BAL::$CLASS
		tacAwkCode="$tacAwkCode print \"\\tusing $BC_NAMESPACE::BC$CLASS;\";"
	done

	#add class definition from WK namespace to point to BAL one
	tacAwkCode="$tacAwkCode print \"namespace $WK_NAMESPACE{\";first=1;next}}/.*/{print}"
	BC_CODE=`echo "$BC_CODE" | tac | gawk "$tacAwkCode" | tac`

	
	#add the right includes to get mother (BI) classes
	includeAwkCode="BEGIN{first=0;}/^[[:space:]]*namespace/{if (first==0){first=1;"
	alreadyIncluded=""
	for CLASS in $BC_CLASSES
	do
		IFILE=`cat $BI_INFOS_FILE | egrep "[[:space:]]$CLASS[[:space:]]" | cut -f 2 | sed 's/.*\/OWBAL\/*//' | sed 's/.*\/WKAL\/*//'`
		#FIXME: what if class defined in many files (in different namespaces for exemple?)


		#try to remove directories?
		IFILE=`echo "$IFILE"  | sed 's/.*\///'`

		if [ -z "$IFILE" ];
		then
			error "nothing found for class [$CLASS] in $BI_INFOS_FILE"
			includeAwkCode="$includeAwkCode print \"#error generator fails to find include for class [$CLASS]\";"
		else
			if ! (  echo $alreadyIncluded | grep "^$IFILE$" )
			then
				alreadyIncluded=`echo $alreadyIncluded ; echo $IFILE`
				includeAwkCode="$includeAwkCode print \"#include \\\"$IFILE\\\"\";"
#			else
#				includeAwkCode="$includeAwkCode print \"//already #include \\\"$IFILE\\\"\";"
			fi
		fi

	done
	#!!! TODO FIXME
	#while debugging, we still use objects that are NOT YET ballified. To avoid non-founding them, we tell to use WebCore/WTF namespaces
	includeAwkCode="$includeAwkCode print \"//using namespace WebCore;\";print \"//using namespace WTF;\"; "
	#add includes
	includeAwkCode="$includeAwkCode }} /.*/{print}"
	BC_CODE=`echo "$BC_CODE" | gawk "$includeAwkCode"`

#	debug "FMD: $includeAwkCode"

fi	

#end of class modification block
else
	debug "file $WK_FILE seems to be C (not C++), all modifications are bypassed"
	BC_CODE="$CODE"
fi

( echo "$BC_CODE" ; echo ) > $BC_FILE
#if ! [ "$WK_TYPE" == "cpp" ]
#then
#no more used	ln -s $BC_LINK_TGT $BC_LINK
#fi
echo "##### GENERATEDFILE=  [$BC_FILE]"
echo "$BC_FILE" >> $GENS_FILE
