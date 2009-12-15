import os
import random

class TestsList :
    def __init__(self, config) :
        self.config = config
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

    def constructList(self, list) :
        # remove Layout Tests directory from test
        # if option is not enabled
        for feature in list:
            if not self.config[feature] :
                self.dirOut.append(feature)
        self.list = self.createList(self.config['layout'])
    
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
