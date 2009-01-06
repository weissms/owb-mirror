# this module extract some informations in BAL class to create unit test skeletons and header skeleton
import os

def isClass(line) :
    return (line.find("class ") != -1 or line.find("struct ") != -1)  \
        and (line.find(":") != -1 or line.find("{") != 1 or line.find("\n") != 1) \
        and line.find("//") == -1 \
        and line.find(";") == -1 \
        and line.find("typedef") == -1

#todo finish find template ( add a status variable and check if this variable is to on when you check the class or struct.
def isTemplate(line) :
    return line.find("template") != -1 or line.find("<") != -1

#get class name
def className(path) :
    f = open(path, "r")
    list = []
    for line in f :
        if isClass(line) and not isTemplate(line):
	    if line.find("class ") != -1 :
                pos = line.find("class ") + 6
	    elif line.find("struct ") != -1 :
                pos = line.find("struct ") + 7
	    fin = line.find(" ", pos)
	    list.append(line[pos:fin])
    f.close()
    return list 
