from configuration import configurationDRT
import os
import time
import logging
import sys
from threading import Timer
import subprocess
import difflib
import signal

def handler(signum, frame):
    print ""

class RunTests :
    def __init__(self, conf, testsList, drtPath, layoutPath, platform, dstResult="/tmp/DstResult") :
        self.conf = conf
        self.testsList = testsList
        self.drtPath = drtPath
        self.layoutPath = layoutPath
        self.platform = platform
        self.leakList = {}
        self.time = 0
        self.timeout = False
        self.startTime = 0
        self.resultSuccess = {}
        self.resultFailed = {}
        self.resultPlatformFailed = {}
        self.resultTimeout = {}
        self.resultCrashed = {}
        self.resultNew = {}
        self.pid = 0
        self.dstResult = dstResult

    def startDrt(self) :
        if not os.path.exists(self.drtPath + "/DumpRenderTree") :
            print "DumpRenderTree are not in " + self.drtPath
            exit(0)
        for test in self.testsList :
            t = Timer(15.0, self.__timeout, [test])
            t.start()
            self.__startTest(test)
            t.cancel()

        print
        print self.leakList

        total = len(self.resultSuccess) + len(self.resultFailed) + len(self.resultPlatformFailed) + len(self.resultTimeout) + len(self.resultCrashed) + len(self.resultNew)
        print "total tests = " + str(total)
        percentSuccess = (float(len(self.resultSuccess)) / float(total)) * 100.0
        percentFailed = (float(len(self.resultFailed)) / float(total)) * 100.0
        percentPlatformFailed = (float(len(self.resultPlatformFailed)) / float(total)) * 100.0
        percentTimeout = (float(len(self.resultTimeout)) / float(total)) * 100.0
        percentCrashed = (float(len(self.resultCrashed)) / float(total)) * 100.0
        percentNew = (float(len(self.resultNew)) / float(total)) * 100.0
        print "success (" + str(len(self.resultSuccess)) + ") : " + str(percentSuccess)
        print "failed (" + str(len(self.resultFailed)) + ") : " + str(percentFailed)
        print "platform failed (" + str(len(self.resultPlatformFailed)) + ") : " + str(percentPlatformFailed)
        print "timeout (" + str(len(self.resultTimeout)) + ") : " + str(percentTimeout)
        print "crashed (" + str(len(self.resultCrashed)) + ") : " + str(percentCrashed)
        print "new (" + str(len(self.resultNew)) + ") : " + str(percentNew)

        if self.time >= 3600.0 :
            print "total time = " + time.strftime('%H h %M m %S s',time.localtime(self.time))
        elif self.time >= 0.01 :
            print "total time = " + time.strftime('%M m %S s',time.localtime(self.time))
        else :
            print "total time = " + str(self.time)

        self.__HtmlResult(self.dstResult)


    def __startTest(self, test) :
        self.startTime = time.time()       
        out = subprocess.Popen(self.drtPath + "/DumpRenderTree " + test + " 2> /tmp/drt.tmp", shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, close_fds=True)
        (child_stdin, child_stdout, child_stderr) = (out.stdin, out.stdout, out.stderr)
        self.pid = out.pid
        out.wait()
        self.time += time.time() - self.startTime
        outprint = ""
        outTemp = ""
        signal.signal(signal.SIGALRM, handler)
        try:
            signal.alarm(2)
            outTemp = child_stdout.read()
        except Exception, e:
            if not str(e) == 'timeout':  # something else went wrong ..
                print test[test.rfind("/") + 1:] + ": timeout"
        signal.alarm(0)
        if not self.timeout :
            outprint = outTemp
            outprint = outprint[:outprint.find("#EOF")]
        else :
            if outTemp.find("#EOF") != -1 :
                outprint = outTemp
                outprint = outprint[:outprint.find("#EOF")]
                self.timeout = False
        if outprint == "" :
            if not self.timeout :
                print test[test.rfind("/") + 1:] + ": crashed"
                self.resultCrashed[test] =  [self.time, "", ""]
                return
            else :
                self.resultTimeout[test] = [self.time, "", ""]
                self.timeout = False
                return
        result, file = self.__getExpected(test) 
        if result :
            #compare result
            cmp, diff = self.__compareResult(outprint, file)
            res = ""
            if cmp :
                if self.conf.isVerbose() :
                    print test[test.rfind("/") + 1:] + ": success"
                else :
                    sys.stderr.write(".")
                self.resultSuccess[test] = [self.time, diff, file]
            else :
                if not self.timeout :
                    print test[test.rfind("/") + 1:] + ": failed"
                    if (file.find("platform") == -1) :
                        self.resultFailed[test] = [self.time, diff, file]
                    else :
                        self.resultPlatformFailed[test] = [self.time, diff, file]
                else :
                    self.resultTimeout[test] = [self.time, "", ""]
                    self.timeout = False
        else :
            #create expected
            try :
                os.makedirs(file[:file.rfind("/")], 0755)
            except OSError :
               logging.warning("dir exist : " + file[:file.rfind("/")])
            f = open(file, "w")
            f.write(outprint)
            f.close()
            self.resultNew[test] = [self.time, outprint, file]

        if self.conf.supportLeak() :
            #get LEAK
            f = open("/tmp/drt.tmp", "r")
            for leak in f :
                if leak.find("LEAK") != -1 :
                    le = leak.split()
                    try :
                        count = int(self.leakList[le[2]])
                    except KeyError :
                        count = 0
                    count += int(le[1])
                    self.leakList[le[2]] = count

            f.close()

    def __timeout(self, test) :
        self.time += time.time() - self.startTime
        self.timeout = True
        if self.pid != 0 :
            os.kill(self.pid, signal.SIGKILL)
            os.waitpid(-1, os.WNOHANG)
            #os.kill(self.pid, 9)
            self.pid = 0
        #print test[test.rfind("/") + 1:] + ": timeout"

    def __getExpected(self, test) :
        #get extension
        ext = test[test.rfind("."):]
        expected = test.replace(ext, "-expected.txt")
        if test.find("http://") != -1 :
            expected = expected.replace("http://localhost:8080", self.layoutPath + "http/")
        if not os.path.exists(expected) :
            dir = self.layoutPath + "/platform/bal/" + self.platform + "/"
            file = dir + expected[len(self.layoutPath):]
            if not os.path.exists(file) :
                return False, file
            else :
                return True, file
        else : 
            return True, expected
   
    def __compareResult(self, out, expectedFile) :
        f = open(expectedFile)
        expected = f.read()
        f.close()

        if out == expected :
            return True, ""
        else :
            diff = difflib.unified_diff(expected.split("\n"), out.split("\n"))
            outDiff = ""
            for line in diff :
                outDiff += line + "\n"
            return False, outDiff

    def __createImage(self) :
        print "create image"

    def __compareImage(self) :
        print "compare image"

    def __createChecksum(self) :
        print "create checksum"

    def __compareChecksum(self) :
        print "compare checksum"

    def __HtmlResult(self, pathToTestResult) :
        try :
            os.makedirs(pathToTestResult, 0755)
        except OSError :
            logging.warning("dir exist : " + pathToTestResult)
        f = open(pathToTestResult + "/result.html", "w")
        f.write("<html>\n")
        f.write("<head>\n")
        f.write("<title>Layout Test Results</title>\n")
        f.write("</head>\n")
        f.write("<body>\n")

        self.__htmlForResultsSection(f, self.resultFailed, "Tests where results did not match expected results", pathToTestResult);
        self.__htmlForResultsSection(f, self.resultPlatformFailed, "Tests platform specific where results did not match expected results", pathToTestResult);
        self.__htmlForResultsSection(f, self.resultTimeout, "Tests that timed out", pathToTestResult);
        self.__htmlForResultsSection(f, self.resultCrashed, "Tests that caused the DumpRenderTree tool to crash", pathToTestResult);
        self.__htmlForResultsSection(f, self.resultNew, "Tests that had no expected results (probably new)", pathToTestResult);

        f.write("</body>\n")
        f.write("</html>\n")
        f.close()

    def __htmlForResultsSection(self, file, tests, desc, pathToTestResult) :
        file.write("<p>"+desc+":</p>\n")
        file.write("<table>\n")
        keys = tests.keys()
        keys.sort()
        for test in keys :
            res = tests[test]
            file.write("<tr>\n")
            file.write("<td><a href=\"file://" + test + "\">"+test+"</a></td>")
            if res[1] != "" :
                dir = pathToTestResult + "/" + res[2][res[2].find("LayoutTests") + 11:res[2].rfind("/")]
                try :
                    os.makedirs(dir, 0755)
                except OSError :
                   logging.warning("dir exist : " + dir)
                outFile = dir + "/" +res[2][res[2].rfind("/") + 1 :] + "-diff.txt"
                f = open(outFile, "w")
                f.write(res[1])
                f.close()
                file.write("<td><a href=\"" + outFile + "\">diff</a></td>")
                file.write("<td><a href=\"" + res[2] + "\">expected</a></td>")
            file.write("</tr>")
        file.write("</table>")
