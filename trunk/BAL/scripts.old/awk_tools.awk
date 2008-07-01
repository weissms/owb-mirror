BEGIN{
	deepLevel=0;	#level into {}
	inString=0;		#between two " or two '


	preCCLevel=0;	#level of #if
	removeUntilCC=-1;
	genericCC=1;	#=1 if code is generic (for BI)


	#if we work with $CLASS
	beforeClass=1;	# true if before class
	inClass=-1;	#>=0 if in class (=the deelLevel)
	exitClassNext=0;
	afterClass=0;	#true if after class

	#tell if we are inside a defined class
	inDefinedClass=-1;	#>=0 if in a defined class
    exitInDefinedClassNext=0;
}


/.*/{
	if (exitClassNext==1)
	{
		inClass=-1;
		exitClassNext=0;
		afterClass=1;
	}
	if (exitInDefinedClassNext==1)
	{
		exitInDefinedClassNext=0;
		inDefinedClass=-1;
	}
}


#********************* string detection
#FIXME: broken, since awk is line based
#function isInString(
#/[^\\]?'/{
#	if (inComment==0)
#	{
#		if (inString==0)
#			inString=1;
#		else
#			inString=0;
#	}
##	print "FMD': inString=" inString
#}
#
#/[^\\]?"/{
#	if (inString==0)
#		inString=1;
#	else
#		inString=0;
##	print "FMD\": inString=" inString
#}



#************************ {} depth counter
#WARNING: works only if code is 'well' formated, that's to say, 1 [{}] per line, nothing else
/[^\\]?{/{
#	print "FMD+" deepLevel;
	if (inString==0)
		deepLevel+=1;
}

/[^\\]?}/{
#	print "FMD-" deepLevel;
	if (inString==0)
		deepLevel-=1;

	if (deepLevel<inClass)	#detect when we exit class
	{
		if (inClass>=0)
		{
			#tell to exit class next line
			#FIXME: it can we a problem when class{things};someCode, as someCode will be taken...
			exitClassNext=1;
	#		print "FMD: exit class: $0"
		}
	}
	if (inDefinedClass>=0)
		if (deepLevel<inDefinedClass)
   		{
   		    exitInDefinedClassNext=1;
   		}
}


#************************** #if depth counter
/^[[:space:]]*#[iI][fF]/{
	if (inString==0)
	{
		preCCLevel+=1;
		if (preCCLevel>1)
	        if (match($0,"ENABLE(.*)")==0)
	        {
				if (removeUntilCC<0)
		            removeUntilCC=preCCLevel;
				if (preCCLevel>1)
					genericCC=0;
   		     }
	}
}
/^[[:space:]]*#[eE][lL][sS][eE]/{
	genericCC=1;
}
/^[[:space:]]*#[eE][lL][iI][fF]/{
		if (preCCLevel>1)
	        if (match($0,"ENABLE(.*)")==0)
	        {
				genericCC=0;
   		     }

}
/^[[:space:]]*#[eE][nN][dD][iI][fF]/{
	if (inString==0)
	{
		preCCLevel-=1;
    	if (preCCLevel<removeUntilCC)
    	{
        	removeUntilCC=-1;
			genericCC=1;
   	 	}
	}
}
/^[[:space:]]*#/{
	if (match($0,"__OWB__")>0)
	{
#		print "FMD:OWB "$0 >"/dev/stderr"
		#FIXME: add checks here (! or 'not' is not present, not &&, ...)
		genericCC=1;
	}
}

#************************* detect when we enter into class
/^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\y(class|struct)\y[[:space:]]*\y$CLASS\y[^;]*$/ {
    inClass=deepLevel;
	beforeClass=0;
}

/^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\y(class|struct)\y[[:space:]:{]+[^;]*$/ {
    inDefinedClass=deepLevel;
}

