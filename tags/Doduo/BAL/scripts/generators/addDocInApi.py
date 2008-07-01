#!/usr/bin/python

import sys

print len(sys.argv)
if len(sys.argv) == 2 :
    file = sys.argv[1]
    print file
    
    headOK = 0
    f=open(file, 'r')
    out = ""
    filename = file
    pos = filename.rfind('/')
    if pos != -1 :
        filename = file[pos+1:]
        
    classname = filename[0:filename.find('.h')]
    for line in f :
        if line.find("#include") !=-1 and headOK == 0 :
            out += "\n/**\n *  @file  "+ filename +"\n *  " + classname + " description\n *  Repository informations :\n * - $URL$\n * - $Rev$\n * - $Date$\n */\n#include \"BALBase.h\"\n"
            out += line
            headOK = 1
        if line.find(');') != -1 :
            pos = line.find("(")
            pos2 = line.rfind(" ", 0, pos)
            method = line[pos2:pos]
            out +=  "\n    /**\n     * "+ method +" description\n     * @param[in]: description\n     * @param[out]: description\n     * @code\n     * @endcode\n     */\n"
            out += line
        else :
            out += line
    f.close()
    
    f = open(file, 'w')
    f.write(out)
    f.close()