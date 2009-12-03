from configuration import configurationDRT
import os
import random

class TestsList :
    def __init__(self, conf) :
        self.conf = conf
        self.dirOut = [".svn", "resources", "platform", "conf", "js"]

    def createList(self, path) :
        dirList = os.listdir(path)
        dirList.sort()
        testsList = []
        for fname in dirList:
            if os.path.isdir(path + "/" + fname) == True :
                if fname not in self.dirOut :
                    pathName = ""
                    if path[len(path) - 1] == "/" :
                        pathName = path + fname
                    else :
                        pathName = path + "/" + fname
                    for li in self.createList(pathName) :
                        if li.find("/http") != -1 and li.find("/local") == -1:
                            rpl = li[:li.find("/http") + 5]
                            li = li.replace(rpl, "http://localhost:8080")
                        testsList.append(li)
            else :
                #find all the .html, .shtml, .xml, .xhtml, .pl, .php (and svg) files in the test directory. 
                if (fname.find(".html") != -1 
                or fname.find(".shtml") != -1 
                or fname.find(".xml") != -1
                or fname.find(".xhtml") != -1
                or fname.find(".pl") != -1
                or fname.find(".php") != -1
                or fname.find(".svg") != -1
                or fname.find(".wml") != -1) and fname.find("-disabled") == -1 and fname.find(".xsd") == -1 :
                    pathName = ""
                    if path[len(path) - 1] == "/" :
                        pathName = path + fname
                    else :
                        pathName = path + "/" + fname
                    testsList.append(pathName)
        return testsList

    def constructList(self, path) :
        if not self.conf.supportSVG() :
            self.dirOut.append("svg")
        if not self.conf.supportWML() :
            self.dirOut.append("wml")
        if not self.conf.supportHTTP() :
            self.dirOut.append("http")
        if not self.conf.supportMedia() :
            self.dirOut.append("media")
        if not self.conf.supportGeolocation() :
            self.dirOut.append("geolocation")
        if not self.conf.support3DTransforms() :
            self.dirOut.append("3d")
        if not self.conf.supportPlugin() :
            self.dirOut.append("plugin")
        if not self.conf.supportStorage() :
            self.dirOut.append("storage")

        self.list = self.createList(path)
    
    def getList(self) :
        return self.list
    def getReverseList(self) :
        l = self.list
        l.reverse()
        return l
    def getShuffleList(self) :
        l = self.list
        random.shuffle(l)
        return l
