#this is part of the job, only called in createBI, when covariant is the name class.
#see 2nd pass updateBIIncludes.sh script for all others
/^[[:space:]]*virtual[[:space:]]+$CLASS([[:space:]]*&)?[[:space:]]+/{
	$0="//REMOVED BECAUSE COVARIANT: " $0
}
{print}

