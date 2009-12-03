#!/usr/bin/python

import sys
from configuration import configurationDRT
from testsList import TestsList
from runtests import RunTests
import logging
import os

if len(sys.argv) < 2 :
    print "drt [options] path to build directory"
    print "argument not valid"
    exit(0)

logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s %(levelname)s %(message)s',
                    filename='/tmp/drt.log',
                    filemode='w')

conf = configurationDRT()

def showHelp() : 
    print "drt [options] -dp=dumpRendertreePath -lp=LayoutTestsPath -pl=platformName"
    print "\t--help display the options list"
    print "\t-v verbose"
    print "\t-svg add svg tests"
    print "\t-wml add wml tests"
    print "\t-http add http tests"
    print "\t-media add media tests"
    print "\t-geolocation add geolocation tests"
    print "\t-3d add 3D transforms tests"
    print "\t-plugin add plugin tests"
    print "\t-storage add storage tests"
    print "\t-leak print leak result"
    print "\t-o path to result by default it's in /tmp/DrtResult"
    print "\t--config=path/congig/file give the config file with the to find it"
    print "\t-dp=dumpRendertreePath"
    print "\t-lp=LayoutTestsPath"
    print "\t-pl=platformName for example sdl/gtk/qt..."

layoutPath = ""
drtPath = ""
platform = ""
destResult = ""
tests = []
i = 0

for option in sys.argv[1:] :
    for opt in option.split() :
        if opt.find("--help") != -1 :
           showHelp()
           exit(0)
        elif opt == "-svg" :
            conf.setSupportSVG(True)
        elif opt == "-wml" :
            conf.setSupportWML(True)
        elif opt == "-http" :
            conf.setSupportHTTP(True)
        elif opt == "-media" :
            conf.setSupportMedia(True)
        elif opt == "-geolocation" :
            conf.setSupportGeolocation(True)
        elif opt == "-3d" :
            conf.setSupport3DTransforms(True)
        elif opt == "-plugin" :
            conf.setSupportPlugin(True)
        elif opt == "-storage" :
            conf.setSupportStorage(True)
        elif opt.find("-o") != -1 :
            destResult = opt[opt.find("=") + 1:]
        elif opt == "-leak" :
            conf.setSupportLeak(True)
        elif opt == "-v" :
            conf.setVerbose(True)
        elif opt.find("--config") != -1 :
            file = opt[opt.find("=") + 1:]
            conf.parseConfig(file)
        elif opt.find("-dp") != -1 :
            drtPath = opt[opt.find("=") + 1:]
        elif opt.find("-lp") != -1 :
            layoutPath = opt[opt.find("=") + 1:]
        elif opt.find("-pl") != -1 :
            platform = opt[opt.find("=") + 1:]
        elif opt.find("-path") != -1 :
            path = os.getcwd()
            drtPath =  path + drtPath
            destResult = path + destResult
            layoutPath = path + layoutPath       
        else :
            tests.append(opt)


if layoutPath == "" or drtPath == "" or platform == "":
    print "you forget to fill a argument mandatory (dp, lp or pl )"
    showHelp()
    exit(0)

li = []
if len(tests) == 0 :
    List = TestsList(conf)
    List.constructList(layoutPath)
    li = List.getList()
else :
    List = TestsList(conf)
    for t in tests :
        if os.path.isdir(t) :
            for l in List.createList(t) :
                li.append(l)
        else :
            li.append(t)

#print li
#print len(li) 

if destResult != "" :
    run = RunTests(conf, li, drtPath, layoutPath, platform, destResult)
else :
    run = RunTests(conf, li, drtPath, layoutPath, platform)
run.startDrt()

#print List.getReverseList()
#print
#print List.getShuffleList()



