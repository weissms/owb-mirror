/*
 * Copyright (C) 2008 Pleyo.  All rights reserved.
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
 * 3.  Neither the name of Pleyo nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PLEYO AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PLEYO OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "FileSystem.h"

#include "NotImplemented.h"
#include "PlatformString.h"
#include "CString.h"
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

}
