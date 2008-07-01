/^[[:space:]]*#include/{
	#$0=gensub("(#include[[:space:]]*[\"<])(\\y|.*/)","\1\2BC");
	$0=gensub("(#include[[:space:]]*[\"<])","\\1BC","1");
}
/.*/{print}
