#!/usr/bin/env python

import os
import sys

#print sys.argv 

cmakeModulePath = ''

unixpath=os.getcwd()
pathlist=unixpath.split('/')
#print pathlist

counter=0
winPath=''
cyg=0

for i in pathlist:
    counter=counter+1
    if 'cygdrive' in i :
        #print 'THIS IS CYGWIN DRIVER'
        cyg=1
        #print counter
        #print pathlist[2]
        winPath=pathlist[2]+':/'
        counter=counter+2
        #print winPath
        break
        
counter2=0

if cyg==1 :
    for i in pathlist :
        counter2=counter2+1
        if counter2>=counter :
            winPath+=pathlist[counter2-1]+'/'
    #print winPath
else :
    counter=0
    winPath=sys.argv[1]
    for i in pathlist :
        counter=counter+1
        winPath+=pathlist[counter-1]+'/'
    #print winPath

for root, directories, files in os.walk(winPath):
    for directory in directories:
        if 'cmake' in directory:
            cmakeModulePath += os.path.join(root, directory)
            cmakeModulePath += ';'
            
print cmakeModulePath