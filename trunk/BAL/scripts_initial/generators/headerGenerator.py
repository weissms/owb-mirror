#!/usr/bin/python

import sys

#get pleyo header
h=open('./data/header.txt', 'r')
header = h.read()
h.close()

#create map type
t=open('./data/replaceByBal.txt', 'r')
mapType = []
for type in t :
    mapType.append(type.split('\t'))

f = []
isFile = 0
if len(sys.argv) == 1 :
    f=open('./data/balInputList.txt', 'r')
    isFile = 1
else :
    f.append(sys.argv[1] + "\t" + sys.argv[2])

for line in f:
    if line.find('Skeletons') != -1:
        #print line
        interfaceList = line.split('\t')
        #print interfaceList
        print interfaceList[1].replace('\n', '')
        fileIn=open(interfaceList[1].replace('\n', ''), 'r')
        fileList = interfaceList[1].split('/')
        #print fileList
        print interfaceList[0] + '/' + fileList[len(fileList)-1].replace('.h', '.t').replace('\n', '').replace('Gtk', 'Bal').replace('Cairo', 'Bal').replace('GStreamer', 'Bal')
        
        classname = fileList[len(fileList)-1].replace('.h', '').replace('\n', '')
        StringIn = fileIn.read()
        #print StringIn[StringIn.find('/*'):StringIn.find('*/')+2]
        
        #Replace header
        #StringIn = StringIn.replace(StringIn[StringIn.find('/*'):StringIn.find('*/')+2], header)
        #print StringIn[0:StringIn.find('#ifndef')]
        if StringIn.find('#ifndef') != -1 :
            StringIn = StringIn.replace(StringIn[0:StringIn.find('#ifndef')], header)
        else :
            StringIn = StringIn.replace(StringIn[StringIn.find('/*'):StringIn.find('*/')+2], header + '\n#ifndef '+classname.replace('Gtk', 'Bal').replace('Cairo', 'Bal')+'_H\n#define '+classname.replace('Gtk', 'Bal').replace('Cairo', 'Bal')+'_H\n')

        #Remove platform definition before class name
        fi = StringIn.split("\n")
        res = ""
        platFound = 0
        classFound = 0
        for line in fi :
            if line.find('class') != -1 and line.find('{') != -1 and platFound == 0:
                classFound = 1
                pos = interfaceList[0].find('WKAL')
                classBase = ""
                if pos != -1 :
                    classBase = ' public WKALBase'
                else :
                    classBase = ' public OWBALBase'
                p = line.find(':')
                if p != -1 :
                    p1 = line[0:p+1]
                    p2 = line[p+2:]
                    line = p1 + classBase + ', ' + p2
                else :
                    p = line.find('{')
                    p1 = line[0:p]
                    p2 = line[p-1:]
                    line = p1 + ':' + classBase + p2
            if classFound == 0 :
                if line.find('#if PLATFORM') != -1 :
                    platFound += 1
                else :
                    if platFound > 0 :
                        if line.find('#if') != -1 :
                            platFound += 1
                        else :
                            if line.find('#endif') != -1 :
                                platFound -= 1
                    else :
                        res += line + "\n"
            else :
                res += line + "\n"
        StringIn = res
        #print StringIn
         
         #remove template header implementation
        #fileL = StringIn.split('\n')
        #fileS = ""
        #tempFound = 0
        #parFound = 0
        #for line in fileL :
            #if line.find('template') != -1:
                #tempFound = 1
                #if line.find('{') != -1:
                    #fileS += line[0:line.find('{')] + '\n'
                #else :
                    #if line.find('\n') != -1:
                        #fileS += line
                    #else :
                        #fileS += line + '\n'
                #if line.find('}') != -1:
                    #tempFound = 0
            #if tempFound == 1:
                #if line.find('{') != -1:
                    #parFound += 1
                #if parFound == 0 and line.find('template') == -1:
                    #if line.find(';') != -1 :
                        #fileS += line + '\n'
                        #tempFound = 0
                    #else :
                        #fileS += line + ';\n'
                #if line.find('}') != -1:
                    #parFound -= 1
                    #if parFound == 0 :
                        #tempFound = 0
                        #parFound = 0
            #else :
                #fileS += line + '\n'
        #StringIn = fileS
         
#        print "\n#############################################\n"
        #print StringIn
        #Remove all platform except gtk
        fileL = StringIn.split('\n')
        remove = 0;
        isRemove = 0
        resultString = ""
        ifFound = 0
        elseFound = 0
        notplat = 0
        for l in fileL:
            if l.find('if PLATFORM') != -1 or l.find('if USE') != -1:
                if     l.find('GTK')  != -1 or  l.find('CAIRO')  != -1 or  l.find('CURL') != -1 or l.find('PTHREADS') != -1     or l.find('ICU_UNICODE') != -1 or l.find('GCC') != -1 :
                    #print "ok" + l 
                    remove = 1
                    notplat = 1
                    isRemoved = 0
                elif   l.find('WX')  != -1 or  l.find('QT')  != -1 or  l.find('WIN') != -1 or  l.find('CG') != -1 or  l.find('SOUP') != -1 or  l.find('CFNETWORK') != -1 or  l.find('WININET') != -1 or  l.find('DARWIN') != -1    or  l.find('SYMBIAN') != -1 or  l.find('MAC') != -1 or  l.find('CF') != -1 or l.find('SKIA') != -1 :
                    #print "bad" + l 
                    remove = 2
		    elseFound = 0
            else :
                if l.find('if !PLATFORM') != -1 and l.find('GTK') == -1 :
                    remove = 1
                    isRemoved = 0
                    notplat = 1
                else :
                    if  l.find('#ifdef __OBJC__') != -1 or l.find('USE_PANGO') != -1:
                        remove = 2
                    else :
                        if l.find('USE_FREETYPE') != -1 :
                            remove = 1
                            isRemoved = 0

            if remove == 1 or remove == 2 :
                if l.find("#if") != -1:		    
                    ifFound += 1
                if l.find("#else") != -1:
                    elseFound = 1
                if l.find("endif") != -1:
                    ifFound -= 1
                    elseFound = 0 
                    notplat = 0
                if ifFound == 0 :
                    remove = 0
                if remove == 1:
                    if  isRemoved == 0 :
                        isRemoved = 1
                    else :
                        if elseFound == 1 :
                            if l.find("#else") == -1 and l.find("#error") == -1 :
                                if notplat == 0 :
                                    resultString += l + '\n'
                        else :
                            resultString += l + '\n'
                else :
                    if remove == 2 :
                        if elseFound == 1 :
                            if l.find("#else") == -1 and l.find("#error") == -1 :
                                resultString += l + '\n'
            else :
               resultString += l + '\n'
        
        if resultString.find('#include <cairo.h>') != -1 :
            resultString = resultString.replace('#include <cairo.h>', '')
        if resultString.find('#include <cairo-ft.h>') != -1 :
            resultString = resultString.replace('#include <cairo-ft.h>', '')
        if resultString.find('#include <fontconfig/fcfreetype.h>') != -1 :
            resultString = resultString.replace('#include <fontconfig/fcfreetype.h>', '')
        
        #print "\n#############################################\n"
        #print resultString
        #remove header implementation (or replace by BalNotImplemented() )
        fileL = resultString.split('\n')
        fileS = ""
        isMethod = 0
        parFound = 0
        acFound = 0
        pFound = 0
        acWasFound = 0
        headOK = 0
        methodOpened = 0
        slashFound = 0
        first = 0
        for line in fileL :
            yetWrited = 0
            if line.find("#define") !=-1 and headOK == 0 :
                headOK = 1
            if headOK == 0 :
                fileS += line + '\n'
            else :
                if line.find('(') != -1 and acFound == 0 and methodOpened == 0 and parFound == 0 and ( line.find('//') == -1 or line.find('//') > line.find('(')) and line.find('#if') == -1 and line.find('#elif') == -1 and line.find('#pragma') == -1 and line.find('usually provided') == -1 and line.find('nor should it') == -1 and line.find('U_MASK') == -1 and line.find('WTF_PRIVATE_INLINE') == -1 and line.find(' \\') == -1 and slashFound <= 0 :
                    if line.find(')') != -1  and ( line.find('FloatRect(') == -1 or classname == 'FloatRect') :
                        if line.find('{') != -1 :
                            if line.find(' : ') != -1 and (line.find('{') > line.find(' : ')) :
                                fileS += line[0:line.find(' : ')] + ';\n'
                                yetWrited = 1
                            else :
                                fileS += line[0:line.find('{')] + ';\n'
                                yetWrited = 1
                            if line.find('}') == -1 :
                                acFound = 1
                                first = 1
                        else :
                            if line.find(' : ') != -1:
                                pFound = 1
                                fileS += line[0:line.find(' : ')] + ';\n'
                                yetWrited = 1
                            if line.find(';') == -1:
                                methodOpened = 1
                    else :
                        parFound = 1
                if methodOpened == 1 :
                    if line.find('{') != -1 :
                        pFound = 0
                        acFound += 1
                        acWasFound = 1
                    if line.find('}') != -1 :
                        acFound -= 1
                    if acFound == 0 :
                        if line.find(' : ') != -1 :
                            pFound = 1
                        if acWasFound == 1:
                            methodOpened = 0
                            acWasFound = 0
                        else :
                            if yetWrited == 0 and pFound == 0 :
                                if line.find("#define") != -1 or line.find('#elif') != -1  or line.find('#endif') != -1 or line.find('#else') != -1 or line.find('#ifndef') != -1 or line.find('#if') != -1 or line == '' or  line.find(' \\') != -1 or slashFound > 0 :
                                    fileS += line + '\n'
                                else :
                                    fileS += line + ';\n'
                                yetWrited = 1
                else :
                    if acFound > 0 :
                        if line.find('{') != -1 :
                            if first == 0 :
                                acFound += 1
                            else : 
                                first = 0
                        if line.find('}') != -1 :
                            acFound -= 1
                    else :
                        if parFound == 1 :
                            if line.find(')') != -1 and line.find('FloatRect(') == -1:
                                parFound = 0
                                if line.find('{') != -1 :
                                    if line.find(' : ') != -1 :
                                        fileS += line[0:line.find(' : ')] + ';\n'
                                        yetWrited = 1
                                    else :
                                        fileS += line[0:line.find('{')] + ';\n'
                                        yetWrited = 1
                                    if line.find('}') == -1 :
                                        acFound = 1
                                else :
                                    if line.find(' : ') != -1 :
                                        pFound = 1
                                        fileS += line[0:line.find(' : ')] + ';\n'
                                        yetWrited = 1
                                    if line.find(';') == -1:
                                        if yetWrited == 0 :
                                            fileS += line + ';\n'
                                            yetWrited = 1
                                        methodOpened = 1;
                                    else :
                                        if yetWrited == 0 :
                                            fileS += line + '\n'
                                            yetWrited = 1
                            else :
                                if yetWrited == 0 :
                                    fileS += line + '\n'
                        else :
                            if yetWrited == 0 :
                                if line.find('template') == -1 :
                                    fileS += line + '\n'
                                else :
                                    if line.find(' : ') == -1 and line.find(';') == -1  and line.find('{') == -1 :
                                        fileS += line + ' '
                                    else :
                                        fileS += line + '\n'
                if line.find(' \\') != -1 :
                    slashFound = 2
                else : 
                    slashFound -= 1
        resultString = fileS
        #print "\n#############################################\n"
        #print resultString
        
        
        #replace gtk methods and attributes by bal method and attributes
        for m in mapType :
            resultString = resultString.replace(m[0], m[1].replace('\n',''))
            
        #replace namespace WebCore by OWBAL or WKAL
        pos = interfaceList[0].find('WKAL')
        if pos != -1 :
            resultString = resultString.replace('namespace WebCore', 'namespace WKAL')
        else :
            resultString = resultString.replace('namespace WebCore', 'namespace OWBAL')
        
        #add doxygen comment
        fileL = resultString.split('\n')
        fileString = ""
        headOK = 0
        parIn = 0
        for line in fileL :
            if line.find("#define") !=-1 and headOK == 0 :
                fileString += line + '\n'
                fileString += "/**\n *  @file  "+ fileList[len(fileList)-1].replace('.h', '.t').replace('\n', '').replace('Gtk', 'Bal').replace('Cairo', 'Bal').replace('GStreamer', 'Bal') +"\n *  " + classname + " description\n *  Repository informations :\n * - $URL$\n * - $Rev$\n * - $Date$\n */\n#include \"BALBase.h\"\n"
                headOK = 1
            else :
                if line.find('(') != -1 and line.find('#if') == -1 and line.find('#endif') == -1 and line.find('#define') == -1 and line.find('#elif') == -1 and line.find('class') == -1 and headOK == 1  and line.find('#pragma') == -1 and line.find('usually provided') == -1 and line.find('nor should it') == -1 and ( line.find('//') == -1 or line.find('//') > line.find('(') != -1) and line.find('U_MASK') == -1 and line.find('WTF_PRIVATE_INLINE') == -1 and line.find(' \\') == -1  and slashFound <= 0 :
                    pos = line.find("(")
                    pos2 = line.rfind(" ", 0, pos)
                    method = line[pos2:pos]
                    fileString +=  "    /**\n     * "+ method +" description\n     * @param[in] : description\n     * @param[out] : description\n     * @code\n     * @endcode\n     */\n"
                    fileString += line + '\n'
                else :
                    fileString += line + '\n'

                if line.find(' \\') != -1 :
                    slashFound = 2
                else : 
                    slashFound -= 1
                
        #print "\n#############################################\n"
        #print fileString
        fileIn.close()
        
        merge = 0;
        fileOutread = 0
        try :
            fileOutread = open(interfaceList[0] + '/' + fileList[len(fileList)-1].replace('.h', '.t').replace('\n', '').replace('Gtk', 'Bal').replace('Cairo', 'Bal').replace('GStreamer', 'Bal'), 'r')
            merge = 1
        except :
            merge = 0
        
        
        if merge == 0 :
            #the file not exist, file creation
            fileOut = open(interfaceList[0] + '/' + fileList[len(fileList)-1].replace('.h', '.t').replace('\n', '').replace('Gtk', 'Bal').replace('Cairo', 'Bal').replace('GStreamer', 'Bal') , 'w')
            fileOut.write(fileString)
            fileOut.close()
        else :
            #merge with the last version
            genMethod = []
            #pos = fileString.find('/**')
            p = fileString.find('#ifndef')
            pos = fileString.find(';', p)
            while pos != -1 :
                #pos = fileString.find('\n', pos + 3)
                method = fileString[fileString.rfind('*/', 0, pos )+2:pos+1]
                iPoint = 0
                point = method.find(';')
                while point != -1 :
                    iPoint += 1
                    point = method.find(';', point + 1)
                if iPoint == 1 and method.find('class') == -1 and method.find('#include') == -1 and method.find('GstBus') == -1 :
                    genMethod.append(method)
                if pos != -1 :
                    pos = fileString.find(';', pos + 1)
            
            oldMethod = []
            oldFile = fileOutread.read()
            p = oldFile.find('#ifndef')
            pos = oldFile.find(';', p)
            while pos != -1 :
                #pos = fileString.find('\n', pos + 3)
                method = oldFile[oldFile.rfind('*/', 0, pos )+2:pos+1]
                iPoint = 0
                point = method.find(';')
                while point != -1 :
                    iPoint += 1
                    point = method.find(';', point + 1)
                if iPoint == 1 and method.find('class') == -1 and method.find('#include') == -1 and method.find('GstBus') == -1 :
                    oldMethod.append(method)
                if pos != -1 :
                    pos = oldFile.find(';', pos + 1)

            i = 0
            nextFound = 0
            for m in genMethod :
                if i >= len(oldMethod) :
                    pos = fileString.find(m)
                    p = fileString.rfind('/**', 0, pos)
                    pr = fileString.rfind('\n', 0, p)
                    fin = fileString.find(';', pos)
                    text = fileString[pr:fin+1]
                    #add the new method in oldFile
                    #posp = oldFile.find(oldMethod[i-1])
                    pos = oldFile.find(oldMethod[len(oldMethod) - 1])
                    #if oldFile.find('public:', posp, pos) != -1 or oldFile.find('private:', posp, pos) != -1 or oldFile.find('protected:', posp, pos) != -1 :
                        #pos = posp
                    p = oldFile.rfind(';', 0, pos)
                    test = oldFile[oldFile.rfind('\n', 0, p):p]
                    while test.strip()[0] == '*' :
                        p = oldFile.find(';', p+1)
                        test = oldFile[oldFile.rfind('\n', 0, p):p]
                    #pr = oldFile.rfind('\n', 0, p)
                    out = oldFile[0:p+1]
                    out += text + '\n    '
                    out += oldFile[p+1:-1]
                    oldFile = out
                else :
                    if m.strip() != oldMethod[i].strip() :
                        for n in oldMethod :
                            if m.strip() == n.strip() :
                                nextFound = 1
                                break
                        if nextFound == 0 :
                            #get name
                            u = m.find('(')
                            genName = m[m.rfind(' ', 0, u):u]
                            u = oldMethod[i].find('(')
                            oldName = oldMethod[i][oldMethod[i].rfind(' ', 0, u):u]
                            if genName == oldName :
                                #replace the method
                                pos = fileString.find(m)
                                p = fileString.rfind('*/', 0, pos)
                                pr = fileString.find('\n', p)
                                fin = fileString.find(';', pos)
                                text = fileString[pr:fin+1].strip()
                                oldFile = oldFile.replace(oldMethod[i].strip(), text);
                                i += 1
                            else :
                                #add new method
                                pos = fileString.find(m)
                                p = fileString.rfind('/**', 0, pos)
                                pr = fileString.rfind('\n', 0, p)
                                fin = fileString.find(';', pos)
                                text = fileString[pr:fin+1]
                                #add the new method in oldFile
                                #posp = oldFile.find(oldMethod[i-1])
                                pos = oldFile.find(oldMethod[i])
                                p = oldFile.rfind('/**', 0, pos)
                                p = oldFile.rfind('\n', 0, p)
                                
                                #p = oldFile.rfind(';', 0, pos)
                                #test = oldFile[oldFile.rfind('\n', 0, p):p]
                                #while test.strip()[0] == '*' :
                                #    p = oldFile.find(';', p+1)
                                #    test = oldFile[oldFile.rfind('\n', 0, p):p]
                                
                                out = oldFile[0:p+1]
                                out += text + '\n\n'
                                out += oldFile[p+1:-1]
                                oldFile = out
                        else :
                            #get name
                            u = m.find('(')
                            genName = m[m.rfind(' ', 0, u):u]
                            u = oldMethod[i].find('(')
                            oldName = oldMethod[i][oldMethod[i].rfind(' ', 0, u):u]
                            #method supprime
                            pos = oldFile.find(oldMethod[i])
                            p = oldFile.rfind('/**', 0, pos)
                            pr = oldFile.rfind('\n', 0, p)
                            fin = oldFile.find(';', pos)
                            test = oldFile[oldFile.rfind('\n', 0, fin):fin]
                            while test.strip()[0] == '*' :
                                fin = oldFile.find(';', fin+1)
                                test = oldFile[oldFile.rfind('\n', 0, fin):fin]
                            out = oldFile[0:p-7]
                            out += oldFile[fin+1:-1]
                            #print out
                            oldFile = out
                            nextFound = 0
                            i += 2
                    else :
                        i += 1
            fileOutread.close()
            fileOut = open(interfaceList[0] + '/' + fileList[len(fileList)-1].replace('.h', '.t').replace('\n', '').replace('Gtk', 'Bal').replace('Cairo', 'Bal').replace('GStreamer', 'Bal') , 'w')
            fileOut.write(oldFile)
            fileOut.close()
if isFile == 1 :
    f.close()
