#!/bin/bash
#Take all generated XML outputs, parse them and aggregate data by source file

#$1	: build dir path
#$2 : if provided, tell the start number (if interrputed, restart at pos...)
#$3 : if provided: tell the number of parallel processes

STARTatPOS=${2:-"0"}
NBPROCESS=${3:-"1"}


#. ../common/defines.sh
. ${0%/*}/defines.sh


trace "+++ Start [$0 $*]"
BUILDDIR=`checkArg "$1"`
FILECPPXML="$BUILDDIR/cpp_xml.txt"

#FAST tmp directory!
TMPD="/dev/shm/"
TMPX="$TMPD/allXmlParsedSorted"
globalAlreadyDone="$TMPD/alreadyParsedXMLs"

LC_ALL=C



if checkDirRead $BUILDDIR && checkFileRead $FILECPPXML
then
	XMLLIST=`sort -u $FILECPPXML | cut -f 2`
	XMLNB=`echo "$XMLLIST" | wc -l`
	REMAIN=$XMLNB
	if [ $STARTatPOS -gt 0 ];
	then
		(( REMAIN=$XMLNB-$STARTatPOS ))
		XMLLIST=`echo "$XMLLIST" | tail -n "$REMAIN"`
	fi


	(( totalTime=0 ))

	debug "remain=$REMAIN	nbProcess=$NBPROCESS"
	for (( tid=0; tid<$NBPROCESS ; tid=$tid+1 ))
	do
		(( nbPerTask=$REMAIN/$NBPROCESS ))

		(
		(( listStart=$nbPerTask*$tid ))
		(( tailNb=$REMAIN-$listStart ))
		headNb=$nbPerTask
		XMLLIST2=`echo "$XMLLIST" | tail -n $tailNb | head -n $headNb`
		debug "tid=$tid listStart=$listStart tailNb=$tailNb headNb=$headNb"

		TMPF="$TMPD/oneXmlParsed.$$.$tid"
		rm $TMPF	2>/dev/null
		TMPX="$TMPD/merged.$$.$tid"
		TOTALperTask="$TMPD/total.$tid"


		(( cpt=0 ))
		cptDone=0
		for XML in $XMLLIST2
		do
			(( cpt=$cpt+1 ))
			#check we don't already process this file:
			if grep -q "$XML" $globalAlreadyDone
			then
				trace "itd=$tid $XML already parsed. skipped."
				continue;
			fi
			#parse XML
			Tbefore=`date +"%s"`
			./getDefinedTypesFromXML.sh "$BUILDDIR/$XML" > $TMPF 2>/dev/null ; 	sort -u $TMPF $TOTALperTask > $TMPX
			mv $TMPX $TOTALperTask
			rm $TMPF
			echo "$XML" >> $globalAlreadyDone
			Tafter=`date +"%s"`
			(( cptDone=$cptDone+1 ))



	#just to know the Estimated Time Left
			(( userTime=$Tafter-$Tbefore ))
			(( totalTime=$totalTime+$userTime ))
			AVERAGE=`echo "scale=2;$totalTime/($cptDone)" | bc`
			ETA=`echo "scale=0;($nbPerTask-$cpt)*$AVERAGE" | bc | sed 's/\..*//'`
	
			if [ $ETA -lt 60 ]
			then
				ETB="$ETA seconds"
			elif [ $ETA -lt 3600 ]
			then
				ETB=`echo "scale=2;($ETA/60)"| bc`
				ETB="$ETB minutes"
			else
				ETB=`echo "scale=2;($ETA/3600)"|bc`
				ETB="$ETB hours"
			fi
			trace "$$ tid=$tid: ${cpt}/$nbPerTask Took $userTime seconds. Total=$totalTime seconds from start. $AVERAGE secs per file. Remains $ETB."
		done
		) &
	done
	trace "Wait for all process to be terminated..."
	wait
fi

#make a backup
#cp $TMPX $BUILDDIR/allXmlParsedSorted.txt
LC_ALL=C sort -u "TMPD/total.*" > $BUILDDIR/allXmlParsedSorted.txt
LC_ALL=C sort -u "TMPD/total.*" > $XML_PARSED


trace "    End $0 for [$*]"
