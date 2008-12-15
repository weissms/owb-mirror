#!/bin/bash

. ${0%/*}/defines.sh

SRCTAG="\/Skeletons\/"
grep "$SRCTAG" $DATA_PATH/balInputList.txt | grep ".h$" | while read SRCP SRCH TAIL
do 
	TESTP=`echo "../$SRCP/" | sed "s/$SRCTAG/\/Tests\//"`
	if [ -z "$TAIL" ]
	then
		TESTH=`echo "${SRCH##*/}" | sed 's/Gtk.h$/Bal.h/'`
	else
		TESTH=`echo "$TAIL" | sed 's/^BI//'`
	fi
	TESTN="$TESTH"
	TESTH=`echo "$TESTH" | sed 's/\.h$/Test.h/'`
	TESTF="${TESTP}${TESTH}"
	TESTCPP=`echo "$TESTH" | sed 's/\.h$/.cpp/'`
	TESTFCPP="${TESTP}${TESTCPP}"
	#echo -n "[$SRCP $SRCH] -> [$TESTF] : "
	echo -n "[$TESTF] : "
	if ! [ -f $TESTF ]
#	if true #force overwrite
	then
		TESTHM=`echo "${TESTH}_CPPUNIT" | sed 's/\./_/g'`
		TESTCLASS=`echo "${TESTH}" | sed 's/\.h$//'`
		TESTCLASS="${TESTCLASS}Test"
		mkdir -p $TESTP
		( 
			echo "#warning This file is NOT YET customized"
			echo "#if 0 //remember to remove last #endif at end of file!"
			echo "//#ifndef $TESTHM"
			echo "#define $TESTHM"
			echo 
			echo "#include <cppunit/extensions/HelperMacros.h>"
			echo "#include \"BC$TESTN\""
			echo "class $TESTCLASS : public CppUnit::TestFixture"
			echo "{"
			echo "    CPPUNIT_TEST_SUITE( $TESTCLASS );"
			echo "//register each method:"
			echo 
			echo "    CPPUNIT_TEST_SUITE_END();"
			echo
			echo
			echo 
			echo
			echo "#error: remove few following lines, including the #ifndef/#define header gard..."
			cat ../$SRCH
			echo "#endif //remember to remove this line once this file is customized"
			echo
		)>$TESTF
		(
			echo "#include \"$TESTH\""
			echo "#ifdef $TESTHM"
			echo "CPPUNIT_TEST_SUITE_REGISTRATION( $TESTCLASS );"
			echo "#endif"
			echo "//to be implemented here... See ./$TESTH for methods to implement"
		)>$TESTFCPP
		echo "Generated"
	else
		echo "ALREADY EXISTS."
	fi
done 
