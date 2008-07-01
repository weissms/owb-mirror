BEGIN {
	inPrivate=0;
	inProtected=0;
	inPublic=0;
	inIfEndif=0;

	classHead="";
	publicCode="";
	protectedCode="";
	privateCode="";

	totalClassCode="";
}
/^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\y(class|struct)\y[[:space:]]+$CLASS\y[^;]*$/ {
#	if (inString==0)
	{
		if (match($0,"^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\\ystruct\\y[[:space:]]+$CLASS\\y")>0)
			inPublic=1;
		else
			inPrivate=1;
		classHead=classHead "\n" $0;
		next;
	}
}

/[[:space:]]*public[[:space:]]*:/{
#	if (inString==0)
	{
		inPrivate=0;
		inProtected=0;
		inPublic=1;
		next;
	}
}
/[[:space:]]*protected[[:space:]]*:/{
#	if (inString==0)
	{
		inPrivate=0;
		inProtected=1;
		inPublic=0;
		next;
	}
}
/[[:space:]]*private[[:space:]]*:/{
#	if (inString==0)
	{
		inPrivate=1;
		inProtected=0;
		inPublic=0;
		next;
	}
}

#skip empty lines
/^[[:space:]]*$/{
	next;
}

#remove chars after class end
/.*/{
	if (exitClassNext>0)
	{
		gsub("};.*$","");
	}
}
#remove pre-compil lines, inside the class
/^[[:space:]]*\#/{
	if (inClass>=0)
		next;
}

#remove lines that defines something that is NOT a method (or reserved word)
/;/ && 
! /\(.*\)/ &&
! /^[[:space:]]*namespace/ &&
! /^[[:space:]]*typedef/ &&
! /^[[:space:]]*class/ &&
! /^[[:space:]]*enum/ {
	next
}

/.*/{
	if (beforeClass)
		classHead=classHead $0;
	if (inClass>=0)
	{
		if (match($0,"^[[:space:]]*friend"))
			classHead=classHead "\n" $0;
		gsub("^[[:space:]]*","\t");
		valuableLine=gensub("[[:space:]]*{.*","","g",$0);
		valuableLine=gensub("[[:space:]]*;.*","","g",valuableLine);
#		print "FMDV=["valuableLine"]"
#		print "TOTAL="totalClassCode
		if (index(totalClassCode,valuableLine)==0)
		{
			totalClassCode=totalClassCode " " $0;
			if (inPrivate>0)
			{
				#constructors go to protected because they must be accessible from BC
				if (match($0,"^[[:space:]]*(explicit[[:space:]]+)?$CLASS\\y\\(")>0)
					protectedCode=protectedCode "\n" $0;
				else
					privateCode=privateCode "\n" $0;
			}
			else if (inProtected>0)
				protectedCode=protectedCode "\n" $0;
			else if (inPublic>0)
				publicCode=publicCode "\n" $0;
		}
		else
		{
#			print "FMD: discard ["$0"]";
		}
	}
}

END{
	print classHead;
	if (publicCode != "")
		print "public:" publicCode ;
	if (protectedCode != "")
		print "protected:" protectedCode ;
	if ($SHOWPRIVATE != 0)
	{
		if (privateCode != "")
			print "private:" privateCode ;
	}
	if (classHead != "")
		print "};";
}
