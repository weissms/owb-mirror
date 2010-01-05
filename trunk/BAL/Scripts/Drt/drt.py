#!/usr/bin/python

from parser import ParseDrtOptions
from runtests import RunTests
from testsList import TestsList
import logging
import os
import pysvn
import sys

usage = 'drt -d DIRECTORY -l DIRECTORY -p PLATFORM -s DIRECTORY [extra-options] [directories]'
feature_list = ['3d', 'appcache', 'cehtml', 'dae', 'datagrid', 'domstorage', 'eventsource', 
                'inspector', 'geolocation', 'http', 'mathml', 'media', 'notifications', 
                'plugin', 'sharedworkers', 'storage', 'svg', 'wcss', 'webgl', 'websocket', 
                'workers', 'wml', 'xhtmlmp', 'xpath']

#for each feature, the list of the path from LayoutTests directory available.
ignoredDirectories = {'3d' : ['compositing', 'animations/3d', 'transforms/3d'],
                      'appcache' : ['http/tests/appcache'],
                      'dae' : ['applications'],
                      'datagrid' : ['fast/dom/HTMLDataGridElement'],
                      'domstorage' : ['storage/domstorage'],
                      'eventsource' : ['fast/eventsource', 'http/tests/eventsource'],
                      'inspector' : ['inspector', 'fast/inspector'],
                      'geolocation' : ['fast/dom/Geolocation'],
                      'http' : ['http'],
                      'mathml' : ['mathml'],
                      'media' : ['media', 'http/tests/media'],
                      'notifications' : ['fast/notifications'],
                      'plugin' : ['plugins', 'http/tests/plugins'],
                      'sharedworkers' : ['fast/workers/shared', 'http/tests/xmlhttprequest/workers/shared', 'http/tests/workers/shared'],
                      'storage' : ['storage'],
                      'svg' : ['svg', 'dom/svg'],
                      'wcss' : ['fast/wcss'],
                      'webgl' : ['fast/canvas/webgl'],
                      'websocket' : ['websocket', 'fast/websockets'],
                      'workers' : ['fast/workers', 'http/tests/xmlhttprequest/workers', 'http/tests/workers'],
                      'wml' : ['wml', 'fast/wml', 'http/tests/wml'],
                      'xhtmlmp' : ['fast/xhtmlmp'],
                      'xpath' : ['fast/xpath', 'dom/svg/level3/xpath'],
                      'xslt' : ['fast/xsl']}

supportedFileExtensions = {'svg' : 'svg', 'wml' : 'wml'}

webkit_layout_tests = 'http://svn.webkit.org/repository/webkit/trunk/LayoutTests'

def update_tests():
    svn = pysvn.Client()
    try:
        info = svn.info(config['layout'])
    except pysvn.ClientError, e:
        print str(e)
        print 'to fix: svn checkout ' + webkit_layout_tests + ' ' + config['layout']
        exit(1)
    if info.url == webkit_layout_tests:
        if config['revision']:
            number = config['revision']
        else:
            print 'to update ' + config['layout'] + ' directory, you need to specify revision'
            exit(1)
        revision = pysvn.Revision(pysvn.opt_revision_kind.number, number)
        if info.revision != revision:
            svn.update(config['layout'], revision = revision)
    else:
        svn.update(config['layout'])

drt_options = ParseDrtOptions(usage, feature_list)
drt_options.initialize_options()
(options, args) = drt_options.parse_args()

# convert options to dictionary
config = options.__dict__

logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s %(levelname)s %(message)s',
                    filename='/tmp/drt.log',
                    filemode='w')

if config['update']:
    update_tests()

li = []
List = TestsList(config)
if len(args) == 0 :
    List.constructList(feature_list, ignoredDirectories, supportedFileExtensions)
    li = List.getList()
else :
    for path in args:
        if os.path.isdir(path) :
            for l in List.createList(path) :
                li.append(l)
        else :
            path = List.changePathIfNeeded(path)
            li.append(path)

run = RunTests(config, li)
run.startDrt()
