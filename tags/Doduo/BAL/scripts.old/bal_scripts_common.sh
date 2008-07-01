AWK="gawk"
AWKTOOLS="awk_tools.awk"

#GENERATED file that tells class convertion (Class => BIClass)
CLASSNAMES_FILE="./class_name_convertion.txt"
#GENERATED file that tells, for a WK file, the BI destination and classes names that are defined in it
BI_INFOS_FILE="./bi_infos.txt"
#GENERATED file that gives convertion for includes (#include "class.h" => #include "BIClass.h")
BI_INCLUDES_FILE="./bi_includes.txt"

#list of all generated files
GENS_FILE="./gens_files.txt"

#file that allows cross namespaces classes
CONFIG_H_FILE="../Base/config.h"

#PATH_BAL="tests/BAL"
PATH_BAL="../"

function debug()
{
	echo -n "DBG:"
	echo "$1" "$2" "$3" "$4" 
}
function error()
{
	echo -n "ERR:"
	echo "$1" "$2" "$3" "$4"  >&2
}

function checkReadableFile()
{
	    if [ -z "$1" ] || ! [ -r "$1" ] ;
	    then
	        error "You must provide $2 file. [$1] is invalid or not found."
	        usage
	        exit 1
	    fi
}

function checkWritableDir()
{
	if [ -z "$1" ];
	then
		error "checkWritableDir: no arg here"
		exit 1
	fi
	if ! [ -d "$1" ]
	then
		error "$1 is NOT a directory"
		return 2
	fi
	if ! [ -w "$1" ]
	then
		error "$1 is NOT writable"
		exit 3
	fi

	return 0

}

function getDirectoryFromFile()
{
	echo "$1" | sed 's/\/[^\/]*$/\//'
}

function getNamespace()
{
	TBAL_PATH="$1"
	if `echo $TBAL_PATH | grep -q "OWBAL"`
	then
		echo "OWBAL"
	else
		if `echo $TBAL_PATH | grep -q "WKAL"`
		then
   			echo "WKAL"
		else
		    error "can't determine namespace from path [$TBAL_PATH]. The path must contain 'OWBAL' or 'WKAL'."
		    exit 6
		fi
	fi
}



#******************************************************************************
#** * * * * 	WK extractors
#******************************************************************************


function reformatWK()
{
	#awk: force the "class foobar" line to don't break block
#	awk '/^[[:space:]]*class[^;{]*$/{printf($0);next}/.*/{print}'| sed 's/{/{\n/g' | sed 's/}[[:space:]]*\(;?\)/\n}\1\n/g'

#	awk '/.*/{gsub("//.*$","");print}' | indent -l1000 -brs | astyle -a | sed 's/{/{\n/g' | sed '/}[[:space:]]*[^;]/s/}\([[:space:]]*\)\([^;].*\)$/\n}\1\n\2/g' | sed '/}[[:space:]]*;/s/}\([[:space:]]*;\)/\n}\1\n/g' | awk '/^[[:space:]]*template[[:space:]]*<.*>[[:space:]]*$/{printf($0);next}/.*/{print;}'


#lines starting with {:, are joined to previous one, and remove //.* on previous before doing that
#lines that end with :, are joined to following one
awk '
	BEGIN{last=""}
	/^[[:space:]]*[{:\,]/{
		if (match(last,"^[[:space:]]*#")) {
				print last;print $0;
				last="";
			}
		else{
			last=gensub("//(.*)$","/*\\1*/","1",last);
			last=last" "$0;
		}
		next
	}
	/.*/{
		if (last!="") 
			print last;
		last=$0
	}
	END{print last}' | awk '
	!/^[[:space:]]*public:/&&!/^[[:space:]]*protected:/&&!/^[[:space:]]*private:/&&/[:,][[:space:]]*$/{
	ORS=" ";print $0 ;ORS="\n";next
	}{print}
	'|  sed 's/{/{\n/g' | sed '/}[[:space:]]*[^;]/s/}\([[:space:]]*\)\([^;].*\)$/\n}\1\n\2/g' | sed '/}[[:space:]]*;/s/}\([[:space:]]*;\)/\n}\1\n/g' | awk '/^[[:space:]]*template[[:space:]]*<.*>[[:space:]]*$/{ORS=" ";print $0;ORS="\n";next}/.*/{print;}'
}


function getClassNameFromWK()
{
	CODE="$1"
#	echo "$CODE" | reformatWK | egrep "class[[:space:]]+[[:alnum:]]+[[:space:]]+[^;]?$" | sed 's/.*class[[:space:]]*\([[:alpha:]][a-zA-Z0-9_]*\).*/\1/g'
	echo "$CODE" | reformatWK | egrep "class[[:space:]]+[a-zA-Z_][a-zA-Z0-9_]*[[:space:]]*[:{]" | sed 's/.*class[[:space:]]*\([a-zA-Z_][a-zA-Z0-9_]*\).*/\1/g'
	echo "$CODE" | reformatWK | egrep "struct[[:space:]]+[a-zA-Z_][a-zA-Z0-9_]*[[:space:]]*[:{]" | sed 's/.*struct[[:space:]]*\([a-zA-Z_][a-zA-Z0-9_]*\).*/\1/g'
}

function getCodeWithoutClassesFromWK()
{
    CODE="$1"
    CLASSES="$2"
    AWKF="getCodeWithoutClassFromWK.awk"
    TMPF="/tmp/$AWKF.$$"
    cat $AWKTOOLS removeComments.awk $AWKF > $TMPF
    echo "$CODE" | reformatWK | $AWK -f $TMPF
    rm $TMPF
}

#get the code of CLASS
function getClassCodeFromWK()
{
	CODE="$1"
	CLASS="$2"
	AWKF="getClassCodeFromWK.awk"
	TMPF="/tmp/$AWKF.$$"
	cat $AWKTOOLS removeComments.awk $AWKF | sed s/\$CLASS/$CLASS/g > $TMPF
	echo "$CODE" | reformatWK | $AWK -f $TMPF
	rm $TMPF
}

#tell all classes declared before the CLASS, ignoring #ifdef
function getExternalClassesFromWK()
{
	CODE="$1"
#	CLASS="$2"
	AWKF="getExternalClassesFromWK.awk"
	TMPF="/tmp/$AWKF.$$"
#	cat $AWKTOOLS $AWKF | sed s/\$CLASS/$CLASS/g > $TMPF
	cat $AWKTOOLS removeComments.awk $AWKF >  $TMPF
	echo "$CODE" | reformatWK | $AWK -f $TMPF
	rm $TMPF

}


#inline fonctions that are defined in header on multi lines
function inlineClassCode()
{
	CLASS="$1"
	CLASS_CODE="$2"
    AWKF="inlineClassCode.awk"
    TMPF="/tmp/$AWKF.$$"
    cat $AWKTOOLS $AWKF | sed s/\$CLASS/$CLASS/g > $TMPF
    echo "$CLASS_CODE" | $AWK -f $TMPF
    rm $TMPF
}

#- group class code by access type (public, protected, private)
#- sort methods
#- remove duplicates
#- remove non generic ones
function regroupClassCode()
{
	CLASS="$1"
	CLASS_CODE="$2"
	SHOW_PRIVATE="$3"
	if [ -z "$SHOW_PRIVATE" ];
	then
		SHOW_PRIVATE="0"
	fi
    AWKF="regroupClassCode.awk"
    TMPF="/tmp/$AWKF.$$"
    cat $AWKTOOLS $AWKF | sed s/\$CLASS/$CLASS/g | sed s/\$SHOWPRIVATE/$SHOW_PRIVATE/g > $TMPF
    echo "$CLASS_CODE" | $AWK -f $TMPF
    rm $TMPF
}

function namespaceClassCode()
{
	CLASS_CODE="$1"
	NAMESPACE="$2"
    AWKF="namespaceClassCode.awk"
    TMPF="/tmp/$AWKF.$$"
    cat $AWKTOOLS $AWKF | sed s/\$CLASS/$CLASS/g | sed s/\$NAMESPACE/$NAMESPACE/g > $TMPF
    echo "$CLASS_CODE" | $AWK -f $TMPF
    rm $TMPF
}


function virtualizeClassCode()
{
	CLASS="$1"
	CLASS_CODE="$2"
	FORCE_PURE_VIRTUAL="$3"
	if [ -z "$FORCE_PURE_VIRTUAL" ];
	then
		FORCE_PURE_VIRTUAL="0"
	fi
    AWKF="virtualizeClassCode.awk"
    TMPF="/tmp/$AWKF.$$"
    cat $AWKTOOLS $AWKF | sed s/\$CLASS/$CLASS/g | sed s/\$FORCEPUREVIRTUAL/$FORCE_PURE_VIRTUAL/g > $TMPF
    echo "$CLASS_CODE" | $AWK -f $TMPF
    rm $TMPF
}


removeCovariantMethods()
{
	CLASS="$1"
	CLASS_CODE="$2"
    AWKF="removeCovariantMethods.awk"
    TMPF="/tmp/$AWKF.$$"
    cat $AWKTOOLS $AWKF | sed s/\$CLASS/$CLASS/g > $TMPF
    echo "$CLASS_CODE" | $AWK -f $TMPF
    rm $TMPF
}

prefixClassName()
{
	CLASS="$1"
	CLASS_CODE="$2"
	PREFIX="$3"
    AWKF="prefixClassName.awk"
    TMPF="/tmp/$AWKF.$$"
    cat $AWKTOOLS $AWKF | sed s/\$CLASS/$CLASS/g | sed s/\$PREFIX/$PREFIX/g > $TMPF
    echo "$CLASS_CODE" | $AWK -f $TMPF
    rm $TMPF
}


function documenteClassCode()
{
	CLASS="$1"
	CLASS_CODE="$2"
	#FIXME TODO
	echo "$CLASS_CODE"
}


function getDefinedClassesName()
{
	BI_CODE="$1"
    AWKF="getDefinedClassesName.awk"
    TMPF="/tmp/$AWKF.$$"
    cat $AWKTOOLS $AWKF > $TMPF
    echo "$BI_CODE" | $AWK -f $TMPF
    rm $TMPF
}

function removeCodeDefinition()
{
	BI_CODE="$1"
    AWKF="removeCodeDefinition.awk"
    TMPF="/tmp/$AWKF.$$"
    cat $AWKTOOLS $AWKF > $TMPF
    echo "$BI_CODE" | $AWK -f $TMPF
    rm $TMPF
}

function updateBCcppIncludes()
{
	BC_CODE="$1"
    AWKF="updateBCcppIncludes.awk"
    TMPF="/tmp/$AWKF.$$"
    cat $AWKTOOLS $AWKF > $TMPF
    echo "$BC_CODE" | $AWK -f $TMPF
    rm $TMPF
}


function processBChClass()
{
	BC_CODE="$1"
	CLASS="$2"
    AWKF="processBChClass.awk"
    TMPF="/tmp/$AWKF.$$"
    cat $AWKTOOLS $AWKF | sed s/\$CLASS/$CLASS/g > $TMPF
    echo "$BC_CODE" | $AWK -f $TMPF
    rm $TMPF
}

function removeInClassesDefs()
{
	BC_CODE="$1"
    AWKF="removeInClassesDefs.awk"
    TMPF="/tmp/$AWKF.$$"
    cat $AWKTOOLS $AWKF > $TMPF
    echo "$BC_CODE" | $AWK -f $TMPF
    rm $TMPF
}

function makeBCConstructsCallBIOnes()
{
	CLASS="$1"
	CODE="$2"
    AWKF="makeBCConstructsCallBIOnes.awk"
    TMPF="/tmp/$AWKF.$$"
    cat $AWKTOOLS $AWKF | sed s/\$CLASS/$CLASS/g > $TMPF
    echo "$CODE" | $AWK -f $TMPF
    rm $TMPF

}


