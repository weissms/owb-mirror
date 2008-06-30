BEGIN{
	#first #ifndef define
	firstIfNdef_=1;
	firstDef_=1;
}
/^[[:space:]]*#ifndef/{
	if (firstIfNdef_==1)
		gsub("#ifndef[[:space:]]*","#ifndef BI_");
	firstIfNdef_=0;
}
/^[[:space:]]*#define/{
	if (firstDef_==1)
		gsub("#define[[:space:]]*","#define BI_");
	firstDef_=0;
}

/^[[:space:]]*namespace\y/{
	gsub("namespace[[:space:]]*WebCore","namespace $NAMESPACE");
#	gsub("namespace[[:space:]]*WTF","namespace $NAMESPACE");
#	gsub("namespace[[:space:]]*[a-zA-Z0-9_]*","namespace $NAMESPACE");
}

/WebCore::/ || /WTF::/{
	gsub("WebCore::","$NAMESPACE::");
#	gsub("WTF::","$NAMESPACE::");
}

#skip the include of Plaform.h
/#include/ && /Platform.h/{
	next
	}
#/#include/ && /wtf\/Noncopyable.h/{
#	gsub("wtf/Noncopyable.h","BAL/Base/Noncopyable.h");
#}

#removed from here: it's done in a latter pass from now. see BI_INCLUDES_FILE
#/#include/{
#	gsub("wtf/","");
#	gsub("#include[[:space:]]*\"","#include \"BI");
#	gsub("#include[[:space:]]*<","#include <BI");
#}
/.*/{
	print
}
END{
#	print "};"
	print "#endif"
	print "";
}
