@include xmlFuncs.awk

BEGIN{
	#holds CONTEXT when in Enumeration that is direct child of Namespace:
	inValuableEnum="";

}


/^[[:space:]]*<Namespace /{
	CONTEXT=getLineContext();
	FNAME=getFileName();
	if (CONTEXT!="")
		print FNAME"\t"NAME"\tNamespace\t";
}

{
	CONTEXT=getLineContext();
	if (CONTEXT!="")
	{
		if (CONTEXT in NS)
		{
			FNAME=getFileName();
			#only process known files
			if (FNAME=="")
				next;


#			ID=getLineID();
			NAME=getLineName();
			TYPE=getLineType();
#			trace(NS[CONTEXT] "\t"NAME);

			if ((NAME!="")&&(match(NAME,"\\.")==0))
			{
				DEF=getLineIsComplete();
				print FNAME"\t"NS[CONTEXT] "\t"TYPE"\t"NAME"\t"DEF #"\t\t"ID;
			}

			if (TYPE=="Enumeration")
			{
				enumFileName=FNAME;
				inValuableEnum=CONTEXT;
			}


		}
	}
	else
	{
		if (inValuableEnum!="")
		{
			TYPE=getLineType();
			if (TYPE=="EnumValue")
			{
				NAME=getLineName();
				print enumFileName"\t"NS[inValuableEnum] "\t"TYPE"\t"NAME;
			}
			else
			{
				inValuableEnum="";
			}
		}
	}
}
