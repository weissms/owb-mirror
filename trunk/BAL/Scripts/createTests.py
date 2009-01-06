#this module create unit tests
import os
import dataClass

def getSkeletonList(path) :
    f = open(path + "/BAL/Scripts/data/balInputList.txt", "r")
    list = []
    for line in f :
        if line.find("/Skeletons/") != -1 :
	    list.append(line.strip())
    f.close()
    return list

def createDestFile(tab) :
    file = tab[1][tab[1].rfind('/') + 1 : ]
    return tab[0] + "/" + file

def createTest(path) :
    list = getSkeletonList(path)
    for l in list :
    	tab = l.split()
	print tab[1]
        print createDestFile(tab)
	print dataClass.className(tab[1])
        print ""

trunkPath = os.getcwd()
createTest(trunkPath)
