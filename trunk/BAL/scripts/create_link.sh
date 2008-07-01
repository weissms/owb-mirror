#!/bin/bash

# $1 : in_File, $2 : dest dir, $3 : keep, $4 : current dir, $5 : noLink

#echo `pwd` > /tmp/foo
#echo "$0: $@" >> /tmp/foo
[ -d generated_link ] || /bin/mkdir -p generated_link/{WKAL,OWBAL/wtf/unicode}
    
prefix=`dirname $1 | sed -e 's/^[a-z,A-Z]*\///g'`

output=`basename $1`
#echo "$prefix" >> /tmp/foo
#echo "$output" >> /tmp/foo
bcfile=`echo $output | sed -e "s/$prefix\.h/\.h/"`

#echo "$bcfile" >> /tmp/foo

if [ $3 -eq 0 ]
then
    symlink=`echo $bcfile | sed -e 's/BC//'`
else
    symlink=`echo $output | sed -e 's/BC//'`
fi

    
if [ `echo $2 | sed -n "/\//p"` ] 
then 
    /bin/ln -f -s $4/$1 generated_link/OWBAL/$bcfile
    /bin/ln -f -s $4/$1 generated_link/OWBAL/$symlink
    /bin/ln -f -s $4/$1 generated_link/$2/$symlink
else
    if [ $5 -eq 0 ]
    then
    	/bin/ln -f -s $4/$1 generated_link/$2/$bcfile
	/bin/ln -f -s $4/$1 generated_link/$2/$symlink
    else
    	/bin/ln -f -s $4/$1 generated_link/$2/$bcfile
    fi
fi
