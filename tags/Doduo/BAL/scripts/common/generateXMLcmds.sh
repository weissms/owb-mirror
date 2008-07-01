#!/bin/bash
#create build
#configure cmake to take owbGcc.sh
#run make to generate all commands. In fact we don't generate XML right now, but only commands to do that.
#XML will be generated only when needed on the fly later.

#. ../common/defines.sh
. ${0%/*}/defines.sh


rm -rf $XML_BUILD_PATH
mkdir $XML_BUILD_PATH
cd $XML_BUILD_PATH


#OwnGCC="$COMMON_PATH/owbGcc.sh"
OwnGCC="$COMMON_PATH/owbGccGetOpts.sh"

#make first a dummy generation to make cmake generate its files (then, he'll test for compiler, and owbGcc.sh will fail the test)
cmake $OWB_PATH
cmake -DCMAKE_C_COMPILER="$OwnGCC" -DCMAKE_CXX_COMPILER="$OwnGCC" $OWB_PATH

#no need to parallelize it (-j) as it is very fast and all OwbGCC write single file, so concurency.
make owb

sort -u $XML_BUILD_CPP_UNSORTED > $XML_BUILD_CPP
sort -u $XML_BUILD_SHF_UNSORTED > $XML_BUILD_SHF

