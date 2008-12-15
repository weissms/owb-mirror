@include levelCounter.awk

BEGIN{
	currentClassDef="";
	inClassDef=0;
}

{
	#tell that this line is a changing state
	lineClassDef=0;
}

/^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\y(class|struct)\y.*{/{
	lineClassDef=1;
	inClassDef=curlyLevel;
    currentClassDef=gensub("^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\\y(class|struct)\\y[[:space:]]+\\y([a-zA-Z0-9_]+)\\y.*","\\3","g",$0);
	trace("Enter class Def ["currentClassDef"]");
	logIndent++;
}
/}/{
	if (inClassDef>0)
	{
		if (curlyLevel<inClassDef)
		{
			logIndent--;
			trace("Exit class Def ["currentClassDef"]");
			inClassDef=0;
			currentClassDef="";
		}
	}
}
