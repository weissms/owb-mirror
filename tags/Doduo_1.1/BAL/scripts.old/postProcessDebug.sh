#!/bin/bash
#
#while scripts are in debug state (partially BALified), here is some hack to make things compile!
#
. bal_scripts_common.sh

#PATH_BAL="../"

function modify()
{
	debug "*** modify $1 for particular process ***"
}


#OWBI=`find $PATH_BAL/OWBAL/Interfaces/ -name 'BI*\.h' -type f`
#WKBI=`find $PATH_BAL/WKAL/Interfaces/ -name 'BI*\.h' -type f`
#OWBC=`find $PATH_BAL/OWBAL/Concretizations/ \( -name 'BC*\.h' -or -name 'BC*.cpp' \) -type f`
#WKBC=`find $PATH_BAL/WKAL/Concretizations/ \( -name 'BC*\.h' -or -name 'BC*.cpp' \) -type f`


#for B_FILE in $OWBI $WKBI $OWBC $WKBC;
for B_FILE in `cat $GENS_FILE`
do
	debug "postProcess [ $B_FILE ]"
	CODE=`cat $B_FILE`
	mv $B_FILE $B_FILE.$$

	#force namespace confusion to avoid conflicts
#	CODE=`echo "#define OWBAL WebCore";echo "#define WKAL WebCore";echo "$CODE"`

	#while String is not balified, make sure it stands in WebCore namespace
#	if echo "$CODE" | egrep -q "^[[:space:]]*class[[:space:]]+String[[:space:]]*;[[:space:]]*$"
#	then
#		CODE=`echo "$CODE" | awk 'BEGIN{print "namespace WebCore{class String;}"}/^[[:space:]]*class[[:space:]]+String[[:space:]]*;[[:space:]]*$/{next}{print}'`
#	fi

	#hack to handle mixed BAL/non-yet-balified code
#	CODE=`echo "#define OWBAL WebCore";echo "#define WKAL WebCore";echo "$CODE"`
	CODE=`echo "$CODE" ; echo "#ifdef __cplusplus";echo "namespace WebCore{using namespace WKAL;using namespace OWBAL;}" ;echo "#endif";echo;echo`


	#remove thing that is defined in a non used #if
	#in /home/fred/workspace/owb_doduo/BAL/WKAL/Concretizations/Graphics/Cairo/Link/ImageSource.h
	if echo "$B_FILE" | grep -q "Concretizations/Graphics/Cairo/BCImageSource"
	then
		modify $B_FILE
		CODE=`echo "$CODE" | sed 's/using WKAL::BCNativeBytePtr;$//'`
	fi

	#add a constructor to BI, and make it transitive
#		if echo "$B_FILE" | grep -q "BISVGImage.h"
#		then
#			modify $B_FILE
#			CODE=`echo "$CODE" | awk '
#				/^[[:space:]]*BISVGImage[[:space:]]*\([[:space:]]*ImageObserver\*[[:space:]]*\)[[:space:]]*{[[:space:]]*}/{
#					gsub("\\\\)[[:space:]]*{[[:space:]]*}"," observer):Image((ImageObserver*)observer) {}");
#				}
#				{print}
#				'`
#		fi
#		if echo "$B_FILE" | egrep -q "BCSVGImage[^\.]*.cpp"
#		then
#			#modify constructor init param to match new mother class
#			modify $B_FILE
#			CODE=`echo "$CODE" | awk '
#				/^[[:space:]]*SVGImage::SVGImage\(ImageObserver\* observer\)/{
#					gsub("Image\\\\(observer\\\\)","BISVGImage(observer)");
#				}
#				{print}
#				'`
#		fi



	#fix too particular case in BAL/WKAL/Interfaces/Graphics/BIBitmapImage.h
	if echo "$B_FILE" | grep -q "BIBitmapImage.h" 
	then

#no more needed since I don't erase WTF namespace anymore
#		modify $B_FILE
#		#remove the unused definition (overloaded in the BC)
#		CODE=`echo "$CODE" | awk '
#			BEGIN{cpt=0;}
#			/^[[:space:]]*namespace[[:space:]]+WKAL\y/{ cpt=cpt+1;}
#			{if (cpt!=2) print;}
#			'`
		CODE=`echo "$CODE" | sed 's/BAL_CLASS_VectorTraits//'`

#		#add constructor to link with mother class
#		modify $B_FILE
#		CODE=`echo "$CODE" | awk '
#			/^[[:space:]]*BIBitmapImage[[:space:]]*\([[:space:]]*BIImageObserver\*[[:space:]]*\)[[:space:]]*{[[:space:]]*}/{
#				gsub("\\\\)[[:space:]]*{[[:space:]]*}"," observer):BCImage((ImageObserver*)observer) {}");
#			}
#			{print}
#			'`
	fi
	if echo "$B_FILE" | egrep -q "BCBitmapImage[^\.]*.h"
	then
		#reput hack into WTF namespace
#no more needed since I don't erase WTF anymore
#		modify $B_FILE
#		CODE=`echo "$CODE" | awk '
#			BEGIN{cpt=0;}
#			/^[[:space:]]*namespace[[:space:]]+WKAL\y/{ cpt=cpt+1;}
#			/^[[:space:]]*using WKAL::BCVectorTraits[[:space:]]*;[[:space:]]*$/{next}
#			{if (cpt!=2) print;}
#			/^[[:space:]]*}[[:space:]]*$/{if (cpt==2){print "namespace WTF { template<> class VectorTraits<WKAL::FrameData> : public SimpleClassVectorTraits {}; }/*WARNING: line generated in postProcessdebug.sh*/";}}
#			'`
		CODE=`echo "$CODE" | sed 's/BAL_CLASS_VectorTraits/template<> class VectorTraits<WKAL::FrameData> : public SimpleClassVectorTraits {}; \/*WARNING: line generated in postProcessdebug.sh*\/";/'`
		echo -n ""
	fi
#	if echo "$B_FILE" | egrep -q "BCBitmapImage[^\.]*.cpp"
#	then
#		#modify constructor init param to match new mother class
#		modify $B_FILE
#		CODE=`echo "$CODE" | awk '
#			/^[[:space:]]*BitmapImage::BitmapImage\(ImageObserver\* observer\)/{
#				gsub("Image\\\\(observer\\\\)","BIBitmapImage(observer)");
#			}
#			{print}
#			'`
#	fi



	if echo "$B_FILE" | egrep -q "BCSVGResourceMarkerWK.cpp|BCSVGResourceClipperWK.cpp|BCSVGResourceMaskerWK.cpp"
	then
		#remove calling default SVGResource default constructor as it is no more the direct mother class
		modify $B_FILE
		CODE=`echo "$CODE" | sed '/^[[:space:]]*\([^:]*\)::\1([^)]*)[[:space:]]*:/s/:[[:space:]]*SVGResource()[[:space:]]*,/:/' |
		 sed '/^[[:space:]]*\([^:]*\)::\1([^)]*)[[:space:]]*:/s/:[[:space:]]*SVGResource()[[:space:]]*{/{/' `
	fi
	


	#remove a "};" that may not be here (why is it here?!?)
	if echo "$B_FILE" | egrep -q "BIGraphicsContext.h|BISVGPaintServerGradient.h"
	then
		modify $B_FILE
		CODE=`echo "$CODE" | tac | awk 'BEGIN{first=1;}/^[[:space:]]*};/{if (first==1){first=0;next;}}{print}' | tac`
	fi


	#using only on CG platform
	if echo "$B_FILE" | egrep -q "BCSVGPaintServerGradientWK.h"
	then
		modify $B_FILE
		CODE=`echo "$CODE" | awk '/using WKAL::BCSharedStopCache;/{print "#if PLATFORM(CG)";print;print "#endif";next;}{print}'`
	fi

	#using only if windows platform
	if echo "$B_FILE" | egrep -q "BCGraphicsContextWK.h"
	then
		modify $B_FILE
		CODE=`echo "$CODE" | awk '/using WKAL::BCWindowsBitmap;/{print "#if PLATFORM(WIN)";print;print "#endif";next;}{print}'`
	fi



	#many files may be impacted: Covariant on pointers if class is not defined, only declared
	#=> remove the class declaration, and include the definition (that must of course inherit from the original BI to avoid covariant)
	if echo "$B_FILE" | grep -q "BCImageWK.h"
	then
		modify $B_FILE
		CODE=`echo "$CODE" | awk '/^[[:space:]]*namespace[[:space:]]+WKAL[[:space:]]*{/{print "#include \"ImageObserver.h\"";}{print}'`
	fi
	if echo "$B_FILE" | grep -q "BCImageBufferCairo.h"
	then
		modify $B_FILE
		CODE=`echo "$CODE" | awk '/^[[:space:]]*namespace[[:space:]]+WKAL[[:space:]]*{/{print "#include \"GraphicsContext.h\"";}{print}'`
	fi


	echo "$CODE" > $B_FILE
	rm $B_FILE.$$
done



#OWBC_CPP=`find $PATH_BAL/OWBAL/Concretizations/  -name 'BC*.cpp' -type f`
#WKBC_CPP=`find $PATH_BAL/WKAL/Concretizations/ -name 'BC*.cpp' -type f`
#for BCCPP in $OWBC_CPP $WKBC_CPP
#do
#
#	CODE=`cat $BCCPP`
#	mv $BCCPP $BCCPP.$$
#	
#	#force use of BAL namespaces
##	CODE=`echo "namespace WebCore{};namespace WTF{};using namespace WebCore;using namespace WTF;" ; echo "$CODE"`
#
#
#	echo "$CODE" > $BCCPP
#	rm $BCCPP.$$
#done
