BEGIN{
	inCommentNext=0;	#tell that this line had a "/*" (which is removed as well as following chars) and that next line will start a comment
	inComment=0;
}


/.*/{
	if (inCommentNext==1)
		inComment=1;
}


#************************ comments removing
#remove // .*
/\/\//{
	gsub("//.*$","");
}
#remove /* ... .*
/\/\*.*\*\//{
#	print "FMD1: remove " $0;
#	$0=gensub("\\/\\*.*\\*\\/","","g");
	while(match($0,"^(.*)\\/\\*.*\\*\\/(.*)$"))
		$0=gensub("^(.*)\\/\\*.*\\*\\/(.*)$","\\1 \\2","g");
#	print "FMD1: removed " $0;
}
#remove /* ...
/\/\*/{
#	print "FMD2: remove " $0;
	gsub("\\/\\*.*$","");
	inCommentNext=1;
}
##remove ... */
/\*\//{
#	print "FMD3: remove " $0;
	gsub("^.*\\*\\/","");
	inComment=0;
	inCommentNext=0;
}
#skip lines in comment (between multilined /* */)
/.*/{
	if (inComment>0)
		next;
}

