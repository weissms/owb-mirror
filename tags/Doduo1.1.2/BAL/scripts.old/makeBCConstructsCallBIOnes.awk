(inDefinedClass<0) {parent=""}

/^[[:space:]]*(template[[:space:]]*<.*>[[:space:]]*)?\y(class|struct)\y[[:space:]]*\y$CLASS\y[[:space:]]*:[^:][^;]*$/ {
    parent=gensub(".*[^:](:[^:{][^}]*){.*","\\1","1",$0);
    parent=gensub("[:,][[:space:]]*(public|protected|private)","","g",parent);
    parent=gensub("^[[:space:]]*","","1",parent);
    parent=gensub("[[:space:]]*$","","1",parent);
#   print "FMD: ["parent"] : "$0
}

#for .h
parent!="" && 
/^[[:space:]]*$CLASS[[:space:]]*\([^;]+$/{
	params=gensub("$CLASS[[:space:]]*\\(([^\\)]*).*","\\1","1");
	names=gensub("[^,]*\\y([a-zA-Z0-9_]+)\\y[[:space:]]*,?","\\1,","g",params);
	names=gensub(",$","","1",names);
    $0=gensub("(^[[:space:]]*$CLASS[[:space:]]*\\([^\\)]*\\))[[:space:]]*:","\\1:BI$CLASS("names"),","1",$0);
    $0=gensub("(^[[:space:]]*$CLASS[[:space:]]*\\([^\\)]*\\))[[:space:]]*{","\\1:BI$CLASS("names"){","1",$0);
}

#for .cpp
#parent!="" && 
/^[[:space:]]*$CLASS::$CLASS[[:space:]]*\([^;]+$/{
	params=gensub("$CLASS::$CLASS[[:space:]]*\\(([^\\)]*).*","\\1","1");
	names=gensub("[^,]*\\y([a-zA-Z0-9_]+)\\y[[:space:]]*,?","\\1,","g",params);
	names=gensub(",$","","1",names);
#print "FMD: names:" names  >"/dev/stderr"
    $0=gensub("(^[[:space:]]*$CLASS::$CLASS[[:space:]]*\\([^\\)]*\\))[[:space:]]*:","\\1:BI$CLASS("names"),","1",$0);
    $0=gensub("(^[[:space:]]*$CLASS::$CLASS[[:space:]]*\\([^\\)]*\\))[[:space:]]*{","\\1:BI$CLASS("names"){","1",$0);
#print "FMD: ici:" $0  >"/dev/stderr"
}


{print}
