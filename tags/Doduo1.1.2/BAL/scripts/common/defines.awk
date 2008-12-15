BEGIN{
	enableGlobalLogs=1
	enableDebug=enableGlobalLogs+0;
	enableTrace=enableGlobalLogs+0;
	enaleError=	enableGlobalLogs+0;

	logIndent=0;
}

function indent(){
	for (i=0;i<logIndent;i++)
		printf(". ") >"/dev/stderr";
}
function debug(display){
	fflush();
	if (enableDebug!=0)
	{
		indent();
		print "[0;0;33mDBG: "display"[0;0;0m" >"/dev/stderr";
	}
}
function trace(display){
	fflush();
	if (enableTrace!=0)
	{
		indent();
		print "[0;0;32mTRC: "display"[0;0;0m" >"/dev/stderr";
	}
}
function error(display){
	fflush();
	if (enableError!=0)
	{
		indent();
		print "[0;0;31mERR: "display"[0;0;0m" >"/dev/stderr";
	}
}
