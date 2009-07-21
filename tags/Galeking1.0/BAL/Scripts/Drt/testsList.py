from configuration import configurationDRT
import os
import random

class TestsList :
    def __init__(self, conf) :
        self.conf = conf

    def __createList(self, path) :
        dirList = os.listdir(path)
        dirList.sort()
        testsList = []
        for fname in dirList:
            if os.path.isdir(path + "/" + fname) == True :
                if fname not in self.dirOut :
                    for li in self.__createList(path + "/" + fname) :
                        testsList.append(li)
            else :
                if fname.find(".html") != -1 and fname.find("-disabled") == -1 :
                    testsList.append(path + "/" + fname)
        return testsList

    def constructList(self, path) :
        self.dirOut = [".svn", "resources", "platform"]
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

        self.list = self.__createList(path)
    
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
