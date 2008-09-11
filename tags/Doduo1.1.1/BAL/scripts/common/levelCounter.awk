#count the {} level
#must be pre-formated (only on { or } per line, comments removed, #if/else/endif handled, ...)
BEGIN{
	curlyLevel=0;
}

/{/{
	curlyLevel++;
}
/}/{
	curlyLevel--;
}

END{
	if (curlyLevel!=0)
	{
		print "error in 'levelCounter.awk': curlyLevel at end is "curlyLevel", but it is supposed to be 0!" >"/dev/stderr"
	}
}

