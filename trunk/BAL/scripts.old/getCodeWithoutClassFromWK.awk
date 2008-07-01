BEGIN{
}
/^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\y(class|struct)\y[[:space:]]+.*{/{
	className=gensub("^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\\y(class|struct)\\y[[:space:]]+([a-zA-Z0-9_]*)\\y.*{.*","\\3","g");
	print "\nBAL_CLASS_"className;
}
/.*/{
	if ((inDefinedClass<0)&&(exitInDefinedClassNext!=1))
    {
        #keep the first #if
        if ((firstIf==0) && (preCCLevel==1) && (match($0,"#if")>0))
        {
            firstIf=1;
            print $0;
        }
        else
        {
#            if (preCCLevel<2)
			if (genericCC==1)
            {
                if (match($0,"^[[:space:]]*#")>0)
                {
                    if (match($0,"^[[:space:]]*#define")>0)
                        print $0;
                    if (match($0,"^[[:space:]]*#include")>0)
                        print $0;
                }
                else
                    print $0;
            }
        }
    }
}
