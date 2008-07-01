BEGIN{
	firstIf=0;
}
{

#	print "FMD="preCCLevel" "removeUntilCC" "genericCC" " $0 >"/dev/stderr"
}
{
#	if (genericCC!=1)
#		next;
}
/.*/{
	if (inClass>=0)
	{
#		if (preCCLevel<2)
		if (genericCC==1)
		{
#			print deepLevel $0;
			#make lines ending with "," inline with the next one
			if (match($0,",[[:space:]]*$")>0)
			{
				ORS=" ";print $0;ORS="\n";
			}
			else
				print $0;
		}
	}
	else
	if (1 == 0)
	{
		#keep the first #if
		if ((firstIf==0) && (preCCLevel==1) && (match($0,"#if")>0))
		{
			firstIf=1;
			print $0;
		}
		else
		{
			if (preCCLevel<2)
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
