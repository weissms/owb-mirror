#!/bin/bash
#This script generates a AWK program that will put right namespace if needed before types names

# INPUT:
#    no inputs (takes $NONBAL_TYPES and $BAL_TYPES from sourced defines.sh)
# OUPUT:
#    no outputs (outputs to $NAMESPACER_AWK file)

#. ../common/defines.sh
. ${0%/*}/defines.sh

(

echo "@include detectNamespace.awk "

echo "{print}"

function startNS()
{
    echo " (lineNameSpace == 1) &&  (currentNameSpace == \"$NS\"){"
    NS_CURLY="$NS"
    OLDNS="$NS"
}

function endNS()
{
    if [ "$NS_CURLY" != "$0$$" ]
    then
        echo "}"
        NS_CURLY=""
    fi
}

#read, ordering on NameSpace (and hack for making 'sort' works even with '::' that is bypassed else)
OLDNS="FMDtagadatzointzoinNotYetUsed$0$$"
NS_CURLY="$0$$"
cat $NONBAL_TYPES | egrep -v "[[:space:]]OperatorFunction[[:space:]]" | sed 's/  */\t/g' | sed 's/::/aaaFMD::/' | sort  -k 2 | sed 's/aaaFMD::/::/' | while read FILE NS TYPE NAME DEF
do
    if [ "$NS" != "::" ]
    then
        if [ "$OLDNS" != "$NS" ]
        then
            debug "start Namespace [$NS]"
            endNS
                startNS
        fi
#        echo "    if (match(\$0,\"[^a-zA-Z0-9_]?$NAME[^a-zA-Z0-9_]?\")>0)"
#        echo "        \$0=gensub(\"([^a-zA-Z0-9_])?$NAME([^a-zA-Z0-9_])?\",\"\\\\1$NS::$NAME\\\\2\",\"g\",\$0);"
        echo "    print \"using $NS::$NAME;\";"
    fi

done

    echo "}"

) > $NAMESPACER_AWK
    TMPAWK="/tmp/namespacer.awk.$$"
    cat ../common//defines.awk > $TMPAWK
    mkLongAwks $NAMESPACER_AWK >> $TMPAWK
    mv $TMPAWK $NAMESPACER_AWK
