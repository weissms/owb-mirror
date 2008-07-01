#!/bin/bash
#common functions

function getNamespaceFromBALPath()
{
    TBAL_PATH="$1"
    if `echo $TBAL_PATH | grep -q "OWBAL"`
    then
        echo "OWBAL"
    else
        if `echo $TBAL_PATH | grep -q "WKAL"`
        then
            echo "WKAL"
        else
            error "can't determine namespace from path [$TBAL_PATH]. The path must contain 'OWBAL' or 'WKAL'."
            exit 6
        fi
    fi
}

reformat()
{
	awk '
	    BEGIN{last=""}
		/^[[:space:]]*[{:\,]/{
			if (match(last,"^[[:space:]]*#")) {
				print last;print $0;
				last="";
 			}
			else{
				last=gensub("//(.*)$","/*\\1*/","1",last);
				last=last" "$0;
 			}
			next
		}
		{
		if (last!="")
			print last;
			last=$0
		}
		END{print last}' | \
		awk '
			!/^[[:space:]]*public:/&&
			!/^[[:space:]]*protected:/&&
			!/^[[:space:]]*private:/&&
			/[:,][[:space:]]*$/{
	    		ORS=" ";
				print $0 ;
				ORS="\n";
				next
	    	}
			{print}
	    '| \
		sed 's/{/{\n/g' | \
		sed '/}[[:space:]]*$/s/}[[:space:]]*$/\n}\n/g' | \
		sed '/}[[:space:]]*[^;]/s/}\([[:space:]]*\)\([^;].*\)$/\n}\n\1\2/g' | \
		sed '/}[[:space:]]*;/s/}\([[:space:]]*;\)/\n}\1\n/g' | \
		awk '
			/^[[:space:]]*template[[:space:]]*<.*>[[:space:]]*$/{
				ORS=" ";
				print $0;
				ORS="\n";
				next
			}
			{print;}
		' | \
		awk '
		BEGIN{inCommentNext=0;inComment=0;}
		{
			if (inCommentNext==1)
				inComment=1;
		}
		/\/\//{
		    gsub("//.*$","");
		}
		/\/\*.*\*\//{
    		while(match($0,"^(.*)\\/\\*.*\\*\\/(.*)$"))
        		$0=gensub("^(.*)\\/\\*.*\\*\\/(.*)$","\\1 \\2","g");
		}
		/\/\*/{
    		gsub("\\/\\*.*$","");
		    inCommentNext=1;
		}
		/\*\//{
    		gsub("^.*\\*\\/","");
		    inComment=0;
		    inCommentNext=0;
		}
		{
 		   if (inComment>0)
        	next;
			print
		}
		'
}



function getLink()
{
	LINK="$1"
	TGT=`stat -c "%N" "$LINK" | sed "s/.*-> \\\`\\(.*\\)'$/\\1/"`
#	debug "Link [$LINK] => TGT=[$TGT]"
	echo "$TGT"
}


function constructBALName()
{
#INPUT: a line of the $INPUT_LIST file
	LINE="$1"
#	debug "LINE=[$LINE]"
	echo "$LINE" | while read p wk fname
	do
#		debug "    p[$p] wk=[$wk] fname=[$fname]"
		WK_FILE="$wk"
		BC_FNAME="$fname"
        BAL_IMPL=`echo "$p" | sed 's/^.*\///'`
        BAL_MODULE=`echo "$p" | sed 's/.*\/Concretizations\/\(.*\)\/[^\/]*$/\1/'`
        BAL_PATH=`echo "$p" | sed 's/Concretizations\/.*$//'`

#		debug "    file=[$WK_FILE] path=[$BAL_PATH] module=[$BAL_MODULE] impl=[$BAL_IMPL]"

		BC_DIR="$BAL_PATH/Concretizations/$BAL_MODULE/$BAL_IMPL"
		WK_FNAME=`echo $WK_FILE | sed 's/^.*\///g'`
		#   BC_PREFIX=`echo $BC_NAMESPACE | head -c 2`
		#   BC_PREFIX="${BC_PREFIX}BC"
		    BC_PREFIX="BC"
		if [ -z "$BC_FNAME" ];
		then
		    radix=`echo "$WK_FNAME" | sed 's/\.[^\.]*$//' | sed s/$BAL_IMPL$//`
		    tailer=`echo "$WK_FNAME" | sed 's/.*\(\.[^\.]*\)$/\1/'`
		    BC_FILE="$BC_DIR/${BC_PREFIX}${radix}${BAL_IMPL}${tailer}"
#		    BC_LINK_TGT="../${BC_PREFIX}${radix}${BAL_IMPL}${tailer}"
#		    BC_LINK="$BC_DIR/Link/${BC_PREFIX}${radix}${tailer}"
		else
		    BC_FILE="$BC_DIR/${BC_FNAME}"
#		    linkName=`echo "$BC_FNAME" | sed s/$BAL_IMPL$tailer$//`
#		    BC_LINK_TGT="../$BC_FNAME"
#		    BC_LINK="$BC_DIR/Link/$BC_FNAME${tailer}"
		fi
		BC_FILE=`echo "$BC_FILE" | sed 's/\/\//\//g'`
		echo "$BC_FILE"
#		debug "    constructBALName: [$LINE] => [$BC_FILE]"
	done
}


function getFileFromCustomPath()
{
    PATHS="$1"
    FILE="$2"
    if [ -r "$FILE" ];
    then
		trace "use [$FILE]"
        echo "$FILE"
        return 0
    fi

    nbTot=`echo -n "$PATHS" | sed 's/[^:]//g' | wc -c`
    (( nbTot=$nbTot+1 ))
    for (( nb=1; $nb<=$nbTot ; nb=$nb+1 ))
    do
        PF=`echo "$PATHS" | cut -d ':' -f $nb`
        PF="$PF/$FILE"
        if [ -r "$PF" ];
        then
			trace "use [$PF]"
            echo "$PF"
            return 0
        fi
    done
	error "No file [$FILE] found for pathes [$PATHS]"
    echo ""
    return 1
}

function mkLongAwks()
{
#input: $1: filename
#		$2! list of already included files
	fnames="$2"
	AWKF=`getFileFromCustomPath "$AWKPATH" "$1"`
	if ! [ -z "$AWKF" ]
	then
		cat "$AWKF" | while read line
		do
			if echo "$line" | egrep -q "^[[:space:]]*@include"
			then
				fname=`echo "$line" | sed 's/^[[:space:]]*@include[[:space:]]*//'`
				if echo "$fnames" | grep -q ":$fname:"
				then
					trace "[$fname] already included (skiped from $1)"
				else
					fnames=":$fnames:$fname:"
					echo "#FMD<<<<: file [$fname] included from [$1]"
					mkLongAwks "$fname" "$fnames"
					echo "#FMD>>>>>: file [$fname] included from [$1]"
				fi
			else
				echo "$line"
			fi
		done
	fi
}



