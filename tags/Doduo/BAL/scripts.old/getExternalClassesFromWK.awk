BEGIN {
	classes="";
	keep[0]="";
	keepNb=0;
}

#{
#		print "FMD: CC="preCCLevel " genericCC="genericCC " " $0 >"/dev/stderr";
#}

/^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\y(class|struct)\y[[:space:]]*[a-zA-Z0-9_]*[[:space:]]*;/{
#	print "FMD match inDefinedClass=" inDefinedClass >"/dev/stderr";
	if (inDefinedClass<0)
#		if (preCCLevel>1)
		class=gensub("^.*(template[[:space:]]*<.*>[[:space:]]*)?\\y(class|struct)\\y[[:space:]]*([a-zA-Z0-9_]*).*","\\3 ",1,$0);
		if (genericCC!=1)
		{
#			print "FMD match3 "class>"/dev/stderr";
			classes=classes class;
		}
		else
		{
#			print "FMD keep3 "class>"/dev/stderr";
			keep[keepNb]=class;
			keepNb+=1;
		}
	}
/^[[:space:]]*typedef/{
	if (inDefinedClass<0)
#		if (preCCLevel>1)
		class1=gensub("[[:space:]]*([^[:space:]]*)[[:space:]]*;","²\\1",1,$0);
		class=gensub(".*²(.*)","\\1 ",1,class1);
		if (genericCC!=1)
		{
#			print "FMD match4 "class>"/dev/stderr";
			classes=classes class;
		}
		else
		{
#			print "FMD keep4 "class>"/dev/stderr";
			keep[keepNb]=class;
			keepNb+=1;
		}
	}
/.*/{
	if (inDefinedClass>=0)
	{
		#stop when class found
#		exit;
	}
}
END{
#	print "FMD4: classes="classes >"/dev/stderr";
	for (indice in keep)
	{
		kept=keep[indice];
#	print "FMD4: keep="kept >"/dev/stderr";
		classes=gensub("\\y"kept"\\y","","g",classes);
	}
	print classes;
}
