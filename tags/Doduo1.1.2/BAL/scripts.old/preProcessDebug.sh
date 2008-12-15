#!/bin/bash
#pre-process files to fix them before entering the workflow
#$1: filename

FNAME="$1"
CODE=`cat $FNAME`

if echo "$1" | grep -q "cairo/GraphicsContextPlatformPrivateCairo.h"
then
	CODE=`echo "#ifndef GRAPHICS_CONTEXT_PLATFORM_PRIVATE";echo "#define GRAPHICS_CONTEXT_PLATFORM_PRIVATE";echo "$CODE" ;echo "#endif"`
fi


echo "$CODE"
