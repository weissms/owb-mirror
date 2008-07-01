#FN[id] => file name
#NS[id] => Namespace

function getLineThing(thing,str)
{
	if (str=="")
		str=$0;
	if (match(str,"[[:space:]]"thing"=")==0)
		return "";
	return gensub("^.*[[:space:]]"thing"=\"([^\"]+)\".*$","\\1","1",str);
}
function getLineID(str)
{
	return getLineThing("id",str);
}
function getLineName(str)
{
	name=getLineThing("name",str);
	name=gensub("&lt;","<","g",name);
	name=gensub("&gt;",">","g",name);
	return name;
}
function getLineContext(str)
{
	return getLineThing("context",str);
}
function getLineType(str)
{
	if (str=="")
		str=$0;
	return gensub("^[[:space:]]*<([^[:space:]]*).*","\\1","1",str);
}
function getLineFileID(str)
{
	return getLineThing("file",str);
}
function getLineIsComplete(str)
{
	if (str=="")
		str=$0;
	#declared (incomplete) or defined ?
	if (match(str,"[[:space:]]incomplete=\"1\"")==0)
		return "DF";
	else
		return "DC";
}


function getFileName(str)
{
	fid=getLineFileID(str);
	fn=FN[fid];
	if (fn=="")
		error("File name for ID=["id"] doesn't exist!");
	return fn;
}

/^[[:space:]]*<File id=\"/{
	ID=getLineID();
	NAME=getLineName();
	#we are only interested by headers!
	if (match(NAME,"\\.h$")>0)
	{
		FN[ID]=NAME;
#		trace("File ["NAME"] with ID=["ID"]");
	}
	next;
}


/^[[:space:]]*<Namespace /{
	NAME=getLineName();
	if ((NSFILTER!="")&&(NSFILTER!=NAME))
	{
		trace("Discard Namespace "NAME" because it is filtered");
		next;
	}
	ID=getLineID();

	trace("Enter Namespace " NAME " with ID=" ID); 
	NS[ID]=NAME;
}

