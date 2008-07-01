BEGIN{
	inNameSpaceBAL=-1;
}
/^[[:space:]]*namespace[[:space:]]+(OWBAL|WKAL)\y[[:space:]]*/{
	inNameSpaceBAL=deepLevel;
}

/^[[:space:]]*}/{
	if ((inNameSpaceBAL>0) && (inNameSpaceBAL>deepLevel))
	{
		inNameSpaceBAL=-1;
	}
}


#take only defined classes, not declared /^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\y(class|struct)\y/{
/^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\y(class|struct)\y.*{/{
	if (inNameSpaceBAL>=0)
	{
		$0=gensub("([<>])"," \\1 ","g",$0);
		print gensub("^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\\y(class|struct)\\y[[:space:]]+\\y([a-zA-Z0-9_]+)\\y.*","\\3","g",$0);
	}
	next
}



/^[[:space:]]*typedef\y/ && !/[()]/ {
	if (inNameSpaceBAL>=0)
		if (inDefinedClass<0)
		{
			$0=gensub("([<>])"," \\1 ","g",$0);
			print gensub("^[[:space:]]*typedef\\y.*[[:space:]]+\\y([a-zA-Z0-9_]*)\\y[[:space:];]*$","\\1","g",$0);
		}
	next
}


/^[[:space:]]*typedef\y/ && /[()]/ {
	if (inNameSpaceBAL>=0)
		if (inDefinedClass<0)
		{
			$0=gensub("([<>])"," \\1 ","g",$0);
			print gensub("^[[:space:]]*typedef\\y.*\\([[:space:]]*\\*[[:space:]]*\\y([a-zA-Z0-9_]*)\\y.*","\\1","g",$0);
		}
	next
}


/^[[:space:]]*enum\y/ && !/[()]/ {
	if (inNameSpaceBAL>=0)
		if (inDefinedClass<0)
		{
			$0=gensub("([<>])"," \\1 ","g",$0);
			print gensub("^[[:space:]]*enum\\y[[:space:]]+\\y([a-zA-Z0-9_]*)\\y[[:space:];{]*.*$","\\1","g",$0);
		}
	next
}



