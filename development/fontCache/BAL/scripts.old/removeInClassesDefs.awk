#remove (from BC) types/enum that are already defined inside the BIClass
BEGIN{
	removeLevel=-1;
}
/^[[:space:]]*enum[^{]*$/ {
		next;
	}
/^[[:space:]]*typedef[^{]*$/ {
	if (inDefinedClass>=0)
	{
#		print "FMD:"inDefinedClass"      level="deepLevel"	remove="removeLevel " SKIP   :   " $0 >"/dev/stderr";
		next
	}
}
/^[[:space:]]*enum.*{.*$/ {
		name=gensub(".*enum[[:space:]]+([a-zA-Z0-9_]*)[[:space:]]*{","\\1","1",$0);
		print "#define BC"name" "name;
		removeLevel=deepLevel;
	}
/^[[:space:]]*typedef.*{.*$/ {
	if (inDefinedClass>=0)
	{
		removeLevel=deepLevel;
	}
}
#/.*/{
#	print "FMD:"inDefinedClass"      level="deepLevel"	remove="removeLevel "   :   " $0 >"/dev/stderr";
#	}
/.*/{
	if (removeLevel<0)
		print;
}
/}[[:space:];]*$/{
	if (removeLevel>=0)
	{
		if (removeLevel>deepLevel)
			removeLevel=-1;
	}
}


