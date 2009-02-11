/*
 * Copyright (C) 2009 Joerg Strohmayer.
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
#include "CString.h"
#include "Logging.h"
#include "SearchPopupMenu.h"

#include <cstdio>

namespace WebCore {

SearchPopupMenu::SearchPopupMenu(PopupMenuClient* client)
    : PopupMenu(client)
{
}

void SearchPopupMenu::saveRecentSearches(const AtomicString& name, const Vector<String>& searchItems)
{
    String fileName = String("PROGDIR:Searches");
    if (!name.isEmpty())
        fileName = fileName + "_" + name;

    FILE *file = fopen(fileName.latin1().data(), "w");
    if (file) {
        Vector<String>::iterator end = (Vector<String>::iterator)searchItems.end();
        for (Vector<String>::iterator it = (Vector<String>::iterator)searchItems.begin(); it < end; it++)
            fprintf(file, "%s\n", (*it).utf8().data());

        fclose(file);
    }
}

void SearchPopupMenu::loadRecentSearches(const AtomicString& name, Vector<String>& searchItems)
{
    String fileName = String("PROGDIR:Searches");
    if (!name.isEmpty())
        fileName = fileName + "_" + name;

    searchItems.clear();

    FILE *file = fopen(fileName.latin1().data(), "r");
    if (file) {
        char item[4096];

        while (fgets(item, 4096, file)) {
            size_t len = strlen(item);
            if (len)
                item[len-1] = 0;
            searchItems.append(String(item));
        }

        fclose(file);
    }
}

bool SearchPopupMenu::enabled()
{
    return true;
}

}
