#for $CLASS, 
#add #define BC$CLASS $CLASS
#add mother class (public BI$CLASS)

/^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\y(class|struct)\y[[:space:]]*\y$CLASS\y[^;]*$/ {
	print "#define BC$CLASS $CLASS"

	#remove previous heritance, because heritance will be done by its BI
	$0=gensub("[[:space:]]+\\y$CLASS\\y[[:space:]]*:[^{]*"," $CLASS : public BI$CLASS","1");
	$0=gensub("[[:space:]]+\\y$CLASS\\y[[:space:]]*({?[[:space:]]*)$"," $CLASS : public BI$CLASS\\1","1");
}

#also add defines to non class objects
/^[[:space:]]*enum[[:space:]]+$CLASS\y/ || 
/^[[:space:]]*typedef\y.*\([[:space:]]*\*[[:space:]]*\y\$CLASS\y/ || 
/^[[:space:]]*(typedef|class)[[:space:]]+.*\y$CLASS\y[[:space:]]*;/{
	if (inDefinedClass<0)
	{
		print "#define BC$CLASS $CLASS"
	}
}
/.*/{
	print
}
