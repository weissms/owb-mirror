/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2004 Apple Computer, Inc.
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

#include "config.h"
#include "JSLayoutTestController.h"

#include "BALConfiguration.h"
#include "BIObserverService.h"
#include "Frame.h"
#include "HTMLDocument.h"
#include "kjs_events.h"
#include "kjs_window.h"
#include "xmlhttprequest.h"

#include "JSLayoutTestController.lut.h"


using namespace WebCore;

namespace KJS {

bool JSLayoutTestController::m_dumpAsText = false;

////////////////////// JSLayoutTestController Object ////////////////////////

/* Source for JSLayoutTestControllerProtoTable.
@begin JSLayoutTestControllerProtoTable 1
  dumpAsText                    JSLayoutTestController::DumpAsText_func              DontDelete|Function 0
    waitUntilDone               JSLayoutTestController::WaitUntilDone              DontDelete|Function 0
    notifyDone                  JSLayoutTestController::NotifyDone                 DontDelete|Function 0
    dumpSelectionRect           JSLayoutTestController::Unimplemented              DontDelete|Function 0
    setMainFrameIsFirstResponder JSLayoutTestController::Unimplemented              DontDelete|Function 0
    setWindowIsKey              JSLayoutTestController::Unimplemented              DontDelete|Function 0
    clearBackForwardList        JSLayoutTestController::Unimplemented              DontDelete|Function 0
    repaintSweepHorizontally    JSLayoutTestController::Unimplemented              DontDelete|Function 0
    testRepaint                 JSLayoutTestController::Unimplemented              DontDelete|Function 0
    display                     JSLayoutTestController::Unimplemented              DontDelete|Function 0
    keepWebHistory              JSLayoutTestController::Unimplemented              DontDelete|Function 0
    doesNotExist                JSLayoutTestController::Unimplemented              DontDelete|Function 0
@end
*/

KJS_DEFINE_PROTOTYPE(JSLayoutTestControllerProto)
KJS_IMPLEMENT_PROTOTYPE_FUNCTION(JSLayoutTestControllerProtoFunc)
KJS_IMPLEMENT_PROTOTYPE("LayoutTestController", JSLayoutTestControllerProto, JSLayoutTestControllerProtoFunc)

JSLayoutTestControllerConstructorImp::JSLayoutTestControllerConstructorImp(ExecState *exec, Document *d)
    : doc(d)
{
    // set up prototype
    setPrototype(exec->lexicalInterpreter()->builtinObjectPrototype());
    putDirect(prototypePropertyName, JSLayoutTestControllerProto::self(exec), None);
    // Now there's a prototype named LayoutTestController
}

bool JSLayoutTestControllerConstructorImp::implementsConstruct() const
{
    return true;
}

/**
 * Construct an instance of prototype
 */
JSObject *JSLayoutTestControllerConstructorImp::construct(ExecState *exec, const List &)
{
    return new JSLayoutTestController(exec, doc.get());
}

const ClassInfo JSLayoutTestController::info = { "LayoutTestController", 0, &JSLayoutTestControllerTable, 0 };

/* Source for JSLayoutTestControllerTable.
@begin JSLayoutTestControllerTable 1
  isDumpedAsText    JSLayoutTestController::DumpAsText_attr     DontDelete
@end
*/

bool JSLayoutTestController::GetDumpAsText() {
    return m_dumpAsText;
}

bool JSLayoutTestController::getOwnPropertySlot(ExecState *exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSLayoutTestController, DOMObject>(exec, &JSLayoutTestControllerTable, this, propertyName, slot);
}

JSValue* JSLayoutTestController::getValueProperty(ExecState *exec, int token) const
{
    switch (token) {
        case DumpAsText_attr:
            return jsBoolean(m_dumpAsText);
        default:
            return 0;
    }
}

void JSLayoutTestController::put(ExecState *exec, const Identifier &propertyName, JSValue* value, int attr)
{
    lookupPut<JSLayoutTestController,DOMObject>(exec, propertyName, value, attr, &JSLayoutTestControllerTable, this );
}

void JSLayoutTestController::putValueProperty(ExecState *exec, int token, JSValue* value, int attr)
{
    switch(token) {
      case DumpAsText_attr:
          m_dumpAsText = value->getBoolean();
      break;
  }
}

void JSLayoutTestController::mark()
{
    DOMObject::mark();
}


JSLayoutTestController::JSLayoutTestController(ExecState *exec, Document *d)
{
    m_dumpAsText = false;
    setPrototype(JSLayoutTestControllerProto::self(exec));
}

JSLayoutTestController::~JSLayoutTestController()
{
}

JSValue* JSLayoutTestControllerProtoFunc::callAsFunction(ExecState *exec, JSObject* thisObj, const List& args)
{
    if (!thisObj->inherits(&JSLayoutTestController::info))
    return throwError(exec, TypeError);

    JSLayoutTestController *controller = static_cast<JSLayoutTestController *>(thisObj);

    switch (id) {
    case JSLayoutTestController::DumpAsText_func:
        controller->m_dumpAsText = true;
        return jsUndefined();
    case JSLayoutTestController::WaitUntilDone:
        BAL::getBIObserverService()->notifyObserver("layoutTestController", "waitUntilDone");
        return jsUndefined();
    case JSLayoutTestController::NotifyDone:
        BAL::getBIObserverService()->notifyObserver("layoutTestController", "notifyDone");
        return jsUndefined();
    case JSLayoutTestController::Unimplemented:
        return jsUndefined();

    }

    return jsUndefined();
}

} // end namespace
