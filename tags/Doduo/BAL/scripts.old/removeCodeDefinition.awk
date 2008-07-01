BEGIN{
	removeBlock=-1;
	inEnum=0;
}
/.*/{
#	print "FMD2 removeBlock="removeBlock" level="deepLevel": "$0;
}
/}/{
	if (inEnum>0)
		inEnum=0;
	if (removeBlock>=0)
	{
		if (removeBlock>(deepLevel))
			removeBlock=-1;
		next;
	}
	print;
}
/.*/{
	if (removeBlock>=0)
	{
		next;
	}
	if (inEnum>0)
		print;
}
/^[[:space:]]*enum\y[[:space:]]+/ {
	inEnum=1;
}
/^[[:space:]]*#/ ||
/^[[:space:]]*(template[[:space:]]+<.*>[[:space:]]+)?\y(class|struct)\y[[:space:]]+/
/^[[:space:]]*typedef\y[[:space:]]+/ ||
/^[[:space:]]*enum\y[[:space:]]+/ ||
/^[[:space:]]*namespace\y[[:space:]]+/ ||
/^BAL_CLASS/ {
	print;
	next
	}

/{/{
	removeBlock=deepLevel;
#	print "FMD: removeBlock="removeBlock": "$0;
	next;
}

