import os
import time
import logging
import sys
from threading import Timer
import subprocess
import difflib
import signal
import progress

def handler(signum, frame):
    print ""

class RunTests :
    def __init__(self, config, testsList) :
        self.config = config
        self.testsList = testsList
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
        self.count = 0
        self.prog = progress.ProgressBar(0, len(testsList), 77, mode='fixed')

    def startDrt(self) :
        if not os.path.exists(self.config['drt'] + "/DumpRenderTree") :
            print "DumpRenderTree are not in " + self.config['drt']
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

        self.__HtmlResult(self.config['output'])


    def __startTest(self, test) :
        self.startTime = time.time()       
        self.out = subprocess.Popen(self.config['drt'] + "/DumpRenderTree " + test + " 2> /tmp/drt.tmp", shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, close_fds=True)
        (child_stdin, child_stdout, child_stderr) = (self.out.stdin, self.out.stdout, self.out.stderr)
        self.pid = self.out.pid
        self.out.wait()
        self.time += time.time() - self.startTime
        outprint = ""
        outTemp = ""
        signal.signal(signal.SIGALRM, handler)
        try:
            signal.alarm(2)
            outTemp = child_stdout.read()
        except Exception, e:
            if not str(e) == 'timeout':  # something else went wrong ..
                if self.timeoutPID == str(self.pid) :
                    os.kill(self.pid + 1, signal.SIGKILL)
                self.pid = 0
                self.timeoutPID = 0

                # update progressbar
                self.__updateProgressBar()
                if self.config['verbose'] :
                    print "\n" + test[test.rfind("/") + 1:] + ": timeout"
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
                if self.config['verbose'] :
                    print "\n" + test[test.rfind("/") + 1:] + ": crashed"
                self.__updateProgressBar() 
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
                #if self.config['verbose'] :
                #    print test[test.rfind("/") + 1:] + ": success"
                #else :
                #    sys.stderr.write(".")
                self.__updateProgressBar() 
                self.resultSuccess[test] = [self.time, diff, file]
            else :
                if not self.timeout :
                    if self.config['verbose'] :
                        print "\n" + test[test.rfind("/") + 1:] + ": failed"
                    self.__updateProgressBar() 
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
            self.__updateProgressBar() 

        if self.config['leak'] :
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
            # Get parent pid on next pid of dumpRenderTree process
            # We must do this because with process 2 process are created, one for /bin/sh and one for Drt
            # And we call kill only on the first
            # here we test the parent to don't kill a other process
            f = open("/proc/" + str(self.pid + 1) + "/stat", "r")
            stat = ""
            for i in f:
                stat += i
            f.close()
            s = stat.split()
            self.timeoutPID = s[3]
            os.kill(self.pid, signal.SIGKILL)
            os.waitpid(-1, os.WNOHANG)
            #os.kill(self.pid, 9)
            #self.pid = 0
        #print test[test.rfind("/") + 1:] + ": timeout"

    def __getExpected(self, test) :
        #get extension
        ext = test[test.rfind("."):]
        expected = test.replace(ext, "-expected.txt")
        if test.find("http://") != -1 :
            if test.find("8000") != -1 :
                expected = expected.replace("http://127.0.0.1:8000", self.config['layout'] + "/http/tests")
            if test.find("8443") != -1 :
                expected = expected.replace("http://127.0.0.1:8443", self.config['layout'] + "/http/tests")
            if test.find("8880") != -1 :
                expected = expected.replace("http://127.0.0.1:8880", self.config['layout'] + "/websocket/tests")
        if not os.path.exists(expected) :
            dir = self.config['layout'] + "/platform/bal/" + self.config['platform'] + "/"
            file = dir + expected[len(self.config['layout']):]
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

    def __updateProgressBar(self) :
        self.count += 1
        oldprog = str(self.prog)
        self.prog.update_amount(self.count)
        if oldprog != str(self.prog):
            print self.prog, "\r",
            sys.stdout.flush()
