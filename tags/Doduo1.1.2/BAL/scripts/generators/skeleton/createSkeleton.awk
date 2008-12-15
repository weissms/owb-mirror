@include xmlFuncs.awk

BEGIN{
	CurrentID="";
	CurrentIDLine="";
	CurrentIDType="";
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
		CurrentID="";
		CurrentIDLine="";
	}


/[[:space:]]*Class[[:space:]]/ && (match($0,"name=\""CLASS"\"")>0){
	trace("Class <"CLASS"> is defined by:" $0);	
	CLASSID=getLineID();
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

END{
#	for(idx in LinesByID) { debug("ID="idx" =>  "LinesByID[idx]); }
#	debug("Class ID="CLASSID);
	classType=getXMLTypeFromID(CLASSID);
#	debug("Class Type="classType);

	if (classType=="Class")
		printf("class "CLASS);
	else if (classType=="Struct")
		printf("struct "CLASS);

	heritance=getHeritanceForID(CLASSID);
	if (heritance!="")
		printf(" : "heritance);

	print ""
	print "{"
		getAllMembersFromID(CLASSID,members);
		for (idx in members)
		{
			member="";
			mID=members[idx];
			mMinLine=getMinLineFromID(mID);
#			mDemangled=getDemangledFromID(mID);
#			if (mDemangled!="")
#				print "    "mReturnType"\t"mDemangled";";
#			else
			{
				mName=getNameFromID(mID);
				mType=getXMLTypeFromID(mID);
				mAccess=getAccessFromID(mID);
				if (getLineThing("artificial",mMinLine)=="1")
				{
					member="/*artificial*/ "member;
				}
				if (getLineThing("extern",mMinLine)=="1")
				{
					member="/*extern*/ "member;
				}
				if (getLineThing("inline",mMinLine)=="1")
				{
					member="inline "member;
				}
				if (getLineThing("explicit",mMinLine)=="1")
				{
					member="explicit "member;
				}
				if (getLineThing("virtual",mMinLine)=="1")
				{
					member="virtual "member;
				}
				if (getLineThing("static",mMinLine)=="1")
				{
					member="static "member;
				}
				if (mType=="Field")
				{
					mType=getTypeFromID(mID,"type");
					member=member mType" ";
				}
				if (mType=="Method")
				{
					mReturnType=getTypeFromID(mID,"returns");
					member=member mReturnType" ";
				}
				if (mType=="Destructor")
					mName="~"mName;
				member=member mName;
				if (mType=="Method" || mType=="Constructor" || mType=="Destructor")
				{
					args=getArgumentsFromID(mID);
					member=member"("args")"
				}
				if (getLineThing("const",mMinLine)=="1")
				{
					member=member"const";
				}
				member=member";"
				members[mAccess,++mbs[mAccess]]=member;
			}
		}

		accesses[1]="public";
		accesses[2]="protected";
		accesses[3]="private";
		for (ida=1;ida<=3;ida++)
		{
			print "    "accesses[ida]":";
			for (idx=1;idx<=mbs[accesses[ida]];idx++)
			{
				member=members[accesses[ida],idx];
				#print "idx=["idx"] member=   "member;
				print "        "member;
			}
		}
	print "};"#end of class declaration
}
