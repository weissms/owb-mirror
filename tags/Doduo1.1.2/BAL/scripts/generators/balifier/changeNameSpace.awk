@include detectNamespace.awk

(lineNameSpace!=0) && (currentNameSpace=="WebCore") {
	$0=gensub("WebCore",BCNAMESPACE,"g",$0);
}

{
	print
}
