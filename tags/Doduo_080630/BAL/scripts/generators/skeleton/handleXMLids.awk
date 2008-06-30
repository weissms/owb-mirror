@include xmlFuncs.awk

BEGIN{
	CurrentID="";
	CurrentIDLine="";
	CurrentIDType="";
	#wantedFile ID
	wfID="";
	#wanted things ID
	#array wtIDs
	wtIDsnb=0;
}

/ id=\"[^\"]*\" /{
	ID=getLineID();
	if (CurrentID!="")
		error("new ID="ID" byt CurrentID="CurrentID);
	CurrentID=ID;
	CurrentIDLine="";
	CurrentType=getLineType();
}
(CurrentID!=""){
	if (CurrentIDLine!="")
		CurrentIDLine=CurrentIDLine "\n" $0;
	else
		CurrentIDLine=$0;
}
(/\/>/ && (CurrentID!="") && (getLineType()==CurrentType))||
(/^[[:space:]]*<\// && (match($0,"^[[:space:]]*</"CurrentType">[[:space:]]*$")>0)){
		CurrentIDLine=gensub("&amp;","\\&","g",CurrentIDLine);
		LinesByID[CurrentID]=CurrentIDLine;
		if((wfID!="")&&(match(CurrentIDLine," file=\""wfID"\" ")>0)){
			wtID=CurrentID;
#			debug("wtID="wtID);
			wtIDs[++wtIDsnb]=wtID;
		#	debug("wt    ["$0"]");
		}
		CurrentID="";
		CurrentIDLine="";
	}



/<WantedFile /{
	wfID=getLineID();	
}



function getMinLineFromID(id){
	info=LinesByID[id];
	if (info=="") error("id ["id"] is not found!");
	else
	{
		return gensub("^[[:space:]]*(<[^>]*>).*","\\1","1",info);
	}

}
function getXMLTypeFromID(id){
	info=getMinLineFromID(id);
	if (info=="") error("id ["id"] is not found!");
	else
	{
		return gensub("^[[:space:]]*<([^[:space:]]*) .*","\\1","1",info);
	}
}
function getNameFromID(id){
	return getLineName(getMinLineFromID(id));
}
function getAllMembersFromID(id,members)
{
	info=getMinLineFromID(id);
	mids=gensub("^.* members=\"([^\"]*)\".*$","\\1","1",info);
	split(mids,members," ");
}
function getAccessFromID(id){
	return getLineThing("access",getMinLineFromID(id));
}
function getDemangledFromID(id){
	return getLineThing("demangled",getMinLineFromID(id));
}

function getTypeFromID(id,tag){
	return getTypeFromLine(getMinLineFromID(id),tag);
}

function getTypeRecurs(line,typeName){
	xmlType=getLineType(line);
	nextType=getLineThing("type",line);
	if (getLineThing("const",line)=="1")
		typeName="const "typeName;
	if (xmlType=="PointerType")
		return getTypeRecurs(getMinLineFromID(nextType),typeName)"*";
	else
		if (xmlType=="ReferenceType")
			return getTypeRecurs(getMinLineFromID(nextType),typeName)"&";
		else
			if (nextType=="")
				return typeName getLineName(line);
			else 
				return getTypeRecurs(getMinLineFromID(nextType),typeName);
}

function getTypeFromLine(line,tag){
	if (tag=="")
		tag="type"
	typeID=getLineThing(tag,line);
	lineType=getMinLineFromID(typeID);
	return getTypeRecurs(lineType,"");
}

function getArgumentsFromID(id){
	#returns string between () for a method
	line=LinesByID[id];
	nbArgs=split(line,argLines,"<Argument ");
#	match(line,"(<Argument [^/]*/>)",argLines);
#	debug("args for ["line"]");
	args=""
	for (idx=2;idx<=nbArgs;idx++) # skip the first part of line
	{
		argL="<Argument "argLines[idx];
#		debug("idx=["idx"] arg: ["argL"]");
		argType=getTypeFromLine(argL);
		argName=getLineName(argL);
		#printf(argType" "argName);
		args=args argType" "argName;
		if (idx<nbArgs)
		{
			args=args", ";
		#	printf(", ");
		}
	}
	return args;

}

function getHeritanceForID(id){
    line=LinesByID[id];
    nbBases=split(line,baseLines,"<Base ");
    bases=""
    for (idx=2;idx<=nbBases;idx++) # skip the first part of line
    {
        baseL="<Base "baseLines[idx];
		baseAccess=getLineThing("access",baseL);
        baseType=getTypeFromLine(baseL);
        bases=bases baseAccess" "baseType;
        if (idx<nbBases)
        {
            bases=bases", ";
        }
    }
    return bases;

}

