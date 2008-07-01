BEGIN {
	#must handle depth by hand because some code is inside tests
	cpt=0;
	l1="";
	l2="";
}

/.*/{

}
/[^\\]?{/{
	if (inClass>=0)
	{
		if (inString==0)
			cpt+=1;
	}
}

/.*/{
	if (cpt>1)
		inlined=inlined $0;
	else
	{
		print $0;
	}
}

/[^\\]?}/{
	if (inClass>=0)
	{
		if (inString==0)
			cpt-=1;
		if (cpt==1)
		{
			print inlined;
			inlined="";
		}
	}
}


END{
	
}
