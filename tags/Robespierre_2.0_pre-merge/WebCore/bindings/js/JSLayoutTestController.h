// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2003 Apple Computer, Inc.
 *  Copyright (C) 2005, 2006 Alexey Proskuryakov <ap@nypop.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef JSLAYOUTTESTCONTROLLER_H_
#define JSLAYOUTTESTCONTROLLER_H_

#include "kjs_binding.h"

namespace WebCore {
    class Document;
}

namespace KJS {

  class JSLayoutTestControllerConstructorImp : public DOMObject {
  public:
    JSLayoutTestControllerConstructorImp(ExecState *, WebCore::Document *);
    virtual bool implementsConstruct() const;
    virtual JSObject *construct(ExecState *, const List &args);
  private:
    RefPtr<WebCore::Document> doc;
  };

  class JSLayoutTestController : public DOMObject {
  public:
    JSLayoutTestController(ExecState *, WebCore::Document *d);
    ~JSLayoutTestController();
    virtual const ClassInfo* classInfo() const { return &info; }
    static const ClassInfo info;
    // dumpAsTest is a function *and* an attribute
    enum { DumpAsText_func, DumpAsText_attr, WaitUntilDone, NotifyDone, Unimplemented };

    virtual bool getOwnPropertySlot(ExecState *, const Identifier&, PropertySlot&);
    JSValue *getValueProperty(ExecState *, int token) const;
    virtual void put(ExecState *, const Identifier &propertyName, JSValue *, int attr = None);
    void putValueProperty(ExecState *, int token, JSValue *, int /*attr*/);
    virtual bool toBoolean(ExecState *) const { return true; }
    virtual void mark();

    static bool GetDumpAsText();
  private:
    friend class JSLayoutTestControllerProtoFunc;
    static bool m_dumpAsText;
  };

} // namespace

#endif
