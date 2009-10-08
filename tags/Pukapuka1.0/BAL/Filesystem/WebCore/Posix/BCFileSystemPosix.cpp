/*
 * Copyright (C) 2007, 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "FileSystem.h"

#include "CString.h"
#include "PlatformString.h"

#include <sys/stat.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/types.h>
#include <regex.h>
#include <dirent.h>
#include <string>
using namespace std;


namespace WebCore {

char* filenameFromString(const String& string)
{
    return strdup(string.utf8().data());
}

String filenameToString(const char* filename)
{
    if (!filename)
        return String();
    return String::fromUTF8(filename);
}

bool fileExists(const String& path)
{
    if (path.isNull())
        return false;

    CString fsRep = fileSystemRepresentation(path);

    if (!fsRep.data() || fsRep.data()[0] == '\0')
        return false;

    struct stat fileInfo;

    // stat(...) returns 0 on successful stat'ing of the file, and non-zero in any case where the file doesn't exist or cannot be accessed
    return !stat(fsRep.data(), &fileInfo);
}

bool deleteFile(const String& path)
{
    CString fsRep = fileSystemRepresentation(path);

    if (!fsRep.data() || fsRep.data()[0] == '\0')
        return false;

    // unlink(...) returns 0 on successful deletion of the path and non-zero in any other case (including invalid permissions or non-existent file)
    return !unlink(fsRep.data());
}

bool deleteEmptyDirectory(const String& path)
{
    CString fsRep = fileSystemRepresentation(path);

    if (!fsRep.data() || fsRep.data()[0] == '\0')
        return false;

    // rmdir(...) returns 0 on successful deletion of the path and non-zero in any other case (including invalid permissions or non-existent file)
    return !rmdir(fsRep.data());
}

bool getFileSize(const String& path, long long& result)
{
    CString fsRep = fileSystemRepresentation(path);

    if (!fsRep.data() || fsRep.data()[0] == '\0')
        return false;

    struct stat fileInfo;

    if (stat(fsRep.data(), &fileInfo))
        return false;

    result = fileInfo.st_size;
    return true;
}

bool getFileModificationTime(const String& path, time_t& result)
{
    CString fsRep = fileSystemRepresentation(path);

    if (!fsRep.data() || fsRep.data()[0] == '\0')
        return false;

    struct stat fileInfo;

    if (stat(fsRep.data(), &fileInfo))
        return false;

    result = fileInfo.st_mtime;
    return true;
}

String pathByAppendingComponent(const String& path, const String& component)
{
    if (path.endsWith("/"))
        return path + component;
    else
        return path + "/" + component;
}

bool makeAllDirectories(const String& path)
{
    CString fullPath = fileSystemRepresentation(path);
    if (!access(fullPath.data(), F_OK))
        return true;

    char* p = fullPath.mutableData() + 1;
    int length = fullPath.length();

    if(p[length - 1] == '/')
        p[length - 1] = '\0';
    for (; *p; ++p)
        if (*p == '/') {
            *p = '\0';
            if (access(fullPath.data(), F_OK))
                if (mkdir(fullPath.data(), S_IRWXU))
                    return false;
            *p = '/';
        }
    if (access(fullPath.data(), F_OK))
        if (mkdir(fullPath.data(), S_IRWXU))
            return false;

    return true;
}

String pathGetFileName(const String& path)
{
    return path.substring(path.reverseFind('/') + 1);
}

String directoryName(const String& path)
{
    CString fsRep = fileSystemRepresentation(path);

    if (!fsRep.data() || fsRep.data()[0] == '\0')
        return String();

    return dirname(fsRep.mutableData());
}

Vector<String> listDirectory(const String& path, const String& filter)
{
    Vector<String> entries;

    char* filename = filenameFromString(path);
    DIR* dir = opendir(filename);
    if (!dir)
        return entries;
    int err;
    regex_t preg;
    char *str_regex = strdup(filter.utf8().data());
    err = regcomp (&preg, str_regex, REG_NOSUB | REG_EXTENDED | REG_NEWLINE);
    if (err != 0) {
        return entries;
    }
    struct dirent* file;
    int match;
    while ((file = readdir(dir))) {
        match = regexec (&preg, file->d_name, 0, NULL, 0);
        if (match != 0)
            continue;
        string s = filename;
        s += "/";
        s += file->d_name;
        entries.append(filenameToString(s.c_str()));
    }
    regfree (&preg);
    free(str_regex);
    closedir(dir);
    free(filename);

    return entries;
}

//FIXME : implement this and send a patch to WebCore
int writeToFile(PlatformFileHandle, const char* data, int length)
{
    return -1;
}

CString fileSystemRepresentation(const String &file)
{
    return file.utf8();
}

void closeFile(PlatformFileHandle&)
{
}

bool unloadModule(PlatformModule)
{
    return false;
}

String homeDirectoryPath()
{
    return getenv("HOME");
}

CString openTemporaryFile(const char* prefix, PlatformFileHandle&)
{
    return "";
}

} // namespace WebCore
