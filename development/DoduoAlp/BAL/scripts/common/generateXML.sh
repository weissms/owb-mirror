#!/bin/bash
#create build
#configure cmake to take owbGcc.sh
#run compil!

echo "DEPRECATED"
exit 1




if ! echo "$0" | egrep -q "^\./"
then
	echo "ERROR: you must start this script from its directory"
	exit 1
fi

COMMONPATH=`pwd`
OWBPATH="$COMMONPATH/../../../"


BUILD="owbGccBuild"
rm -rf $BUILD
mkdir $BUILD
cd $BUILD


#OwnGCC="$COMMONPATH/owbGcc.sh"
OwnGCC="$COMMONPATH/owbGccGetOpts.sh"

#make first a dummy generation to make cmake generate its files (then, he'll test for compiler, and owbGcc.sh will fail the test)
cmake $OWBPATH
cmake -DCMAKE_C_COMPILER="$OwnGCC" -DCMAKE_CXX_COMPILER="$OwnGCC" $OWBPATH

make -j 3 owb
