@include detectNamespace.awk
@include detectClassDef.awk

#detect class/struct declaration ONLY (no definition)
function isTypeDeclaration(line)
{
	#class/struct:
	if (match(line,"^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\\y(class|struct)\\y[[:space:]]*[^{}]+;")>0)
		return 1;

	#enum can't be only declared.

	#typedef:
	if (match(line,"^[[:space:]]*typedef[^{}]+;")>0)
		return 1;

	#functions:
#	if (inClassDef<0)
#		if (match(line,"\\)[[:space:]]*(const[[:space:]]*)?;")>0)
#			return 1;

	return 0;
}

function getDeclaredTypeName(line)
{
	return gensub("^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\\y(class|struct)\\y[[:space:]]+\\y([a-zA-Z0-9_]+)\\y.*","\\3","g",$0);
}

{
	if (isTypeDeclaration($0))
	{
		trace("declaration:"$0);
		print getDeclaredTypeName($0);
	}
	else
	{
		debug("non declaration:"$0);
	}
}

END{
	}
