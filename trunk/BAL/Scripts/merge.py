#!/usr/bin/python

import version
import patch
import modification
import cleanOWB
import check
import balification
import build
import sys
import os
import logging

# TODO : add a help list and change option for the revesion like "-r num"
if len(sys.argv) != 1 and len(sys.argv) != 2 :
    logging.warning("argument not valid")
    exit(0)

trunkPath = os.getcwd()
list = os.listdir(trunkPath)
isValidPath = False
for file in list :
    if file == 'BAL' :
        isValidPath = True
	break

if isValidPath == False :
   logging.warning("you are not in the good directory")
   exit(0)

if not os.path.isdir(trunkPath) :
    logging.warning("path not exist")
    exit(0)

if not os.path.exists(trunkPath + "/CMakeLists.txt") :
    logging.warning("it's not a owb repository")
    exit(0)

if len(sys.argv) == 2 :
    mergeVersion = sys.argv[1]
else :
    mergeVersion = 0

logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s %(levelname)s %(message)s',
                    filename='/tmp/merge.log',
                    filemode='w')


if not check.checkDataIntegrity(trunkPath) :
    print "fix all error in data files, commit and restart the merge"
    exit(0)
cleanOWB.clean(trunkPath)
vl = version.getVersionLastMerge(trunkPath)
if mergeVersion == 0 :
    vr = version.getVersionCurrentMerge()
else :
    vr = mergeVersion

patch.createOWBPatch(trunkPath, vl)
patch.createWebkitPatch(trunkPath, vl, vr)
patch.applyWebkitPatch(trunkPath, vr)
raw_input('Fix the patches rejected and press return to continue...')
modification.getListAdded(trunkPath)
raw_input('Add files in CMakeList.txt appropriate and press return to continue...')
modification.getListDeleted(trunkPath)
raw_input('Delete files in CMakeList.txt appropriate and press return to continue...')
balification.balify(trunkPath)
patch.applyOWBPatch(trunkPath)
raw_input('Fix the patches rejected and press return to continue...')
modification.getWinApiChange(trunkPath)
raw_input("add the change in API if it's possible and press return to continue...")
build.configureSDL()
build.configureGtk()
build.configureSDLMini()
version.registerCurrentVersion(trunkPath, vr)
print "merge finished, please add 'merge with webkit revision " +vr+"' in your commit message"
