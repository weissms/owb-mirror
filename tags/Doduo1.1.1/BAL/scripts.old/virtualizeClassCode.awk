BEGIN{
    first=0;
	parent="";
}
/^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\y(class|struct)\y[[:space:]]*\y$CLASS\y[[:space:]]*:[^:][^;]*$/ {
    parent=gensub(".*[^:](:[^:{][^}]*){.*","\\1","1",$0);
    parent=gensub("[:,][[:space:]]*(public|protected|private)","","g",parent);
	parent=gensub("^[[:space:]]*","","1",parent);
	parent=gensub("[[:space:]]*$","","1",parent);
#	print "FMD: ["parent"] : "$0
}

/\y(class|struct)\y.*\y$CLASS\y/ && first==0 {
    print "#define BC$CLASS $CLASS"
    print gensub("\\y$CLASS\\y.*$","$CLASS;","1",$0);
    $0=gensub("([^a-zA-Z0-9_]?)$CLASS([^a-zA-Z0-9_]?)","\\1BI$CLASS\\2","g",$0);
    first=1;
}
/.*/{
	constructor=0;
	forcePureVirtual=$FORCEPUREVIRTUAL;
}
/^[[:space:]]*$CLASS[[:space:]]*\([^\)]*\)/ || 
/^[[:space:]]*explicit[[:space:]]*$CLASS[[:space:]]*\([^\)]*\)/ ||
/^[[:space:]]*(virtual)?[[:space:]]*~$CLASS[[:space:]]*\(/{
	constructor=1;
	#remove members initializations, except for parent class
	if (match($0,"[:,][[:space:]]*"parent"[[:space:]]*\\(")>0)
		keep=gensub("([:,][[:space:]]*"parent"[[:space:]]*\\([^\\)]*\\))","\\1","g",$0);
	else
		keep=""
	$0=gensub("([^:]):[^:][^{]*","\\1","g",$0);
}
#/^[[:space:]]*public:[[:space:]]*$/{
#	#first, always create a default constructor, because BC constructor will need it!
#	print $0;
#	print "BI$CLASS(){}"
#	next;
#}
/\(.*\)/{
	if (inClass>=0)
	{
		if (constructor==1)
		{
			if (match($0,"~$CLASS"))
				gsub("{.*}[[:space:]]*$","{}");
			if (match($0,"\\y~?$CLASS[[:space:]]*\\([^\\)]*\\)[[:space:]]*;?[[:space:]]*$")>0)
			if (match($0,"}[[:space:]]*$")==0)
			{
				gsub(";?[[:space:]]*$","{}");
			}
#			if (match($0,"^[[:space:]]*$CLASS[[:space:]]*\\([[:space:]]*\\)[[:space:]]*{[[:space:]]*}[[:space:]]*$")>0)
#			{
#				#default constructor is already automatically added
#				next;
#			}
		    $0=gensub("\\y$CLASS\\y","BI$CLASS","g",$0);

			#reput things we want to keep in initializator
			if (keep != "")
				$0=gensub("{","K<"keep">K""{","1",$0);
		}
		else
		{
			if (match($0,"^[[:space:]]*static")>0)
			{
					gsub("{.*}[[:space:]]*$",";");
			}
			else
			{
				if (match($0,"^[[:space:]]*virtual")==0)
				{
					$0="virtual "$0;
				}
		
				#remove tailing ';' if after a }
				gsub("}[[:space:]]*;[[:space:]]*$","}");
				#remove empty {} implementations
				gsub("{[[:space:]]*}[[:space:]]*$",";");
				#force all methods to be pure virtual
				if (forcePureVirtual!=0)
					gsub("{.*}[[:space:]]*$",";");
				#remove multi ; ; at end
				gsub(";[[:space:]]*;[[:space:]]*$",";");
				#remove already pure virtual
				gsub("[[:space:]*=[[:space:]]*0[[:space:]]*;[[:space:]]*$",";");
#2008-03-21: can't be pure virtual because : class Foobar{virtual Foobar something()=0;} won't compile: can't instanciate a Foobar because its pure virtual...
#				gsub(";[[:space:]]*$",";");	
			}
		}
		#remove default values (implementation dependant)
		if (match($0,"operator")==0)
		{
			$0=gensub("=[^,\\)]*\\([^\\)]*\\)","","g",$0);
			$0=gensub("=[^,\\)]*","","g",$0);
		}

	}
}
/.*/{
	#reformat virtual
	gsub("^[[:space:]]*virtual[[:space:]]*","\tvirtual ");
	#put all pure virtual (if they end by a ';', which is not the case of {})
	if (match($0,"^[[:space:]]*virtual[[:space:]]+")>0)
		gsub(";[[:space:]]*$"," =0;");
	print;
	}
