@include levelCounter.awk

BEGIN{
	currentNameSpace="";
	inNameSpace=0;
}

{
	lineNameSpace=0;
}

/^[[:space:]]*namespace[[:space:]]+[a-zA-Z0-9_]+[[:space:]]*{[[:space:]]*$/{
	lineNameSpace=1;
	inNameSpace=curlyLevel;
	currentNameSpace=gensub("^[[:space:]]*namespace[[:space:]]+([a-zA-Z0-9_]+)[[:space:]]*{.*$","\\1","1",$0);
	trace("Enter namespace ["currentNameSpace"]");
	logIndent++;
}
/}/{
	if (inNameSpace>0)
	{
		if (curlyLevel<inNameSpace)
		{
			logIndent--;
			trace("Exit namespace ["currentNameSpace"]");
			inNameSpace=0;
			currentNameSpace="";
		}
	}
}
