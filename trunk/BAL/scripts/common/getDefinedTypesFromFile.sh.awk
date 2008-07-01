@include detectNamespace.awk
@include detectClassDef.awk

BEGIN{
	currentEnumDef="";
}


function found(type,name)
{
	ns=currentNameSpace"::"
	if ((currentClassDef!="")&&(currentClassDef!=name))
		ns=ns currentClassDef"::";
	if ((currentEnumDef!="")&&(currentEnumDef!=name))
		ns=ns currentEnumDef"::";
	trace("definition: ns=["ns"] type=["type"] name=["name"]");
	print type"\t"ns name;
}



#take only defined classes, not declared /^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\y(class|struct)\y/{
/^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\y(class|struct)\y.*{/{
    $0=gensub("([<>])"," \\1 ","g",$0);
	found(gensub("^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\\y(class|struct)\\y[[:space:]]+\\y([a-zA-Z0-9_]+)\\y.*","\\2","g",$0),gensub("^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\\y(class|struct)\\y[[:space:]]+\\y([a-zA-Z0-9_]+)\\y.*","\\3","g",$0));
}

/^[[:space:]]*typedef\y.*{/ && !/[()]/ {
    $0=gensub("([<>])"," \\1 ","g",$0);
	found("typedef",gensub("^[[:space:]]*typedef\\y.*[[:space:]]+\\y([a-zA-Z0-9_]*)\\y[[:space:]{]*$","\\1","g",$0));
}

/^[[:space:]]*typedef\y.*{/ && /[()]/ {
    $0=gensub("([<>])"," \\1 ","g",$0);
	found("typedef",gensub("^[[:space:]]*typedef\\y.*\\([[:space:]]*\\*[[:space:]]*\\y([a-zA-Z0-9_]*)\\y.*","\\1","g",$0));
}

/^[[:space:]]*enum\y/ && !/[()]/ {
    $0=gensub("([<>])"," \\1 ","g",$0);
	name=gensub("^[[:space:]]*enum\\y[[:space:]]+\\y([a-zA-Z0-9_]*)\\y[[:space:];{]*.*$","\\1","g",$0);
	currentEnumDef=name;
	found("enum",name);
}
(currentEnumDef!="") && 
! /[{}]/ {
	split($0,results,",[[:space:]]*")
	for (i in results)
	{
		enumVal=gensub("[[:space:]]*\\y([A-Za-z0-9_]*)\\y[[:space:]]*(=[^,]+)?,?","\\1","1",results[i]);
		debug("enumVal=["enumVal"]");
		if ((enumVal!="")&&(match(enumVal,"[^[:space:]]")>0))
			found("enumValue",enumVal)
	}
}
(currentEnumDef!="") &&
/}/{
	currentEnumDef="";
}
{
	debug($0);
}

END{
	}
