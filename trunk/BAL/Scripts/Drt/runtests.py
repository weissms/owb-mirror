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
    raise Exception('timeout')

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
        self.webSocketServerRunning = False
        self.httpServerRunning = False
        self.webSocketServerPID = 0
        self.httpServerPID = 0
        os.environ["WEBKIT_TESTFONTS"]=self.config['source'] + "/WebKitTools/DumpRenderTree/bal/fonts/"

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
        self.__stopWebSocketServer()
        self.__stopHttpServer()


    def __startTest(self, test) :
        if test.find("8880") != -1 :
            self.__startWebSocketServer()
        if test.find("8000") != -1 or test.find("8443") != -1 :
            self.__startHttpServer()
        if test.find("inspector") != -1 :
            os.environ["INSPECTOR_URL"]=self.config['source'] + "/WebCore/inspector/front-end/inspector.html"

        self.startTime = time.time()       
        self.out = subprocess.Popen(self.config['drt'] + "/DumpRenderTree " + test, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, close_fds=True)
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
            self.err = child_stderr.read()
        except Exception, e:
            if not str(e) == 'timeout':  # something else went wrong ..
                if self.timeoutPID == str(self.pid) :
                    os.kill(self.pid + 1, signal.SIGKILL)
                self.pid = 0
                self.timeoutPID = 0

                # update progressbar
                self.__updateProgressBar()
                if self.config['verbose'] :
                    print "\n" + test + ": timeout"
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
                    print "\n" + test + ": crashed"
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
                        print "\n" + test + ": failed"
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
            f = self.err.split('\n')
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
            statFile = "/proc/" + str(self.pid + 1) + "/stat"
            if os.path.exists(statFile) :
                f = open(statFile, "r")
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
                expected = expected.replace("http://127.0.0.1:8880", self.config['layout'])
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

    def __startWebSocketServer(self) :
        if not self.webSocketServerRunning and self.config['source'] is not None :
            self.webSocketServerRunning = True
            command = "PYTHONPATH=" + self.config['source'] + "/WebKitTools/pywebsocket/"
            command += " python "
            command += self.config['source'] +"/WebKitTools/pywebsocket/mod_pywebsocket/standalone.py"
            command += " -p 8880"
            command += " -d " + self.config['layout']
            command += " -s " + self.config['layout'] + "/websocket/tests"
            command += " -l /tmp/pywebsocket_log.txt"
            command += " --strict"

            out = subprocess.Popen(command, shell=True)
            self.webSocketServerPID = out.pid


    def __stopWebSocketServer(self) :
        if self.webSocketServerRunning :
            os.kill(self.webSocketServerPID, signal.SIGKILL)
            os.waitpid(-1, os.WNOHANG)
            self.webSocketServerRunning = True

    def __startHttpServer(self) :
        if not self.httpServerRunning and self.config['source'] is not None :
            conf = ""
            server = ""
            if os.path.exists("/etc/gentoo-release") :
                conf = self.config['source'] + "/BAL/Scripts/data/apache2-gentoo-httpd.conf"
                server = "/usr/sbin/apache2"
            elif os.path.exists("/etc/fedora-release") or os.path.exists("/etc/arch-release") :
                conf = self.config['layout'] + "/http/conf/fedora-httpd.conf"
                server = "/usr/sbin/httpd"
            elif os.path.exists("/etc/debian_version") :
                conf = self.config['layout'] + "/http/conf/apache2-debian-httpd.conf"
                server = "/usr/sbin/apache2"
            else :
                server = "/usr/sbin/httpd"
                conf = self.config['layout'] + "/http/conf/apache2-httpd.conf"
                

            if conf == "" :
                return
            else :
                conf = os.path.abspath(conf)

            if not os.path.exists("/tmp/WebKit") :
                os.mkdir("/tmp/WebKit/")

            self.httpServerRunning = True
            command = server
            #command += " -f " + self.config['layout'] + "/http/conf/apache2-debian-httpd.conf"
            command += " -f " + conf
            command += " -C \"DocumentRoot " + self.config['layout'] + "/http/tests\""
            command += " -c \"Alias /js-test-resources " + self.config['layout'] + "/fast/js/resources\""
            command += " -C \"Listen 127.0.0.1:8000\""
            command += " -c \"TypesConfig " + self.config['layout'] + "/http/conf/mime.types\""
            command += " -c \"CustomLog /tmp/access_log.txt common\""
            command += " -c \"ErrorLog /tmp/error_log.txt\""
            command += " -c \"SSLCertificateFile " + self.config['layout'] + "/http/conf/webkit-httpd.pem\""

            out = subprocess.Popen(command, shell=True)
            self.httpServerPID = out.pid

    def __stopHttpServer(self) :
        if self.httpServerRunning :
            f = open("/tmp/WebKit/httpd.pid")
            pid = f.read()
            f.close()

            p = int(pid)

            os.kill(p, 15)
            self.httpServerRunning = True
