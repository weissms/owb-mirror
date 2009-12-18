/*
 * Copyright (C) 2009 Pleyo.  All rights reserved.
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

#include "AccessibilityUIElement.h"

#include <JSBase.h>
#include <JSStringRef.h>
#include <string>

using std::string;

AccessibilityUIElement::AccessibilityUIElement(PlatformUIElement element)
    : m_element(element)
{
}

AccessibilityUIElement::AccessibilityUIElement(const AccessibilityUIElement& other)
    : m_element(other.m_element)
{
}

AccessibilityUIElement::~AccessibilityUIElement()
{
}

/*void AccessibilityUIElement::getLinkedUIElements(Vector<AccessibilityUIElement>&)
{
}

void AccessibilityUIElement::getDocumentLinks(Vector<AccessibilityUIElement>&)
{
}

void AccessibilityUIElement::getChildren(Vector<AccessibilityUIElement>& children)
{*/
    /*long childCount;
    if (FAILED(m_element->get_accChildCount(&childCount)))
        return;
    for (long i = 0; i < childCount; ++i)
        children.append(getChildAtIndex(i));*/
/*}

void AccessibilityUIElement::getChildrenWithRange(Vector<AccessibilityUIElement>& elementVector, unsigned location, unsigned length)
{*/
    /*long childCount;
    unsigned appendedCount = 0;
    if (FAILED(m_element->get_accChildCount(&childCount)))
        return;
    for (long i = location; i < childCount && appendedCount < length; ++i, ++appendedCount)
        elementVector.append(getChildAtIndex(i));*/
//}

int AccessibilityUIElement::childrenCount()
{
    /*long childCount;
    m_element->get_accChildCount(&childCount);
    return childCount;*/
    return 0;
}

AccessibilityUIElement AccessibilityUIElement::elementAtPoint(int x, int y)
{
    return 0;
}

AccessibilityUIElement AccessibilityUIElement::getChildAtIndex(unsigned index)
{
    /*COMPtr<IDispatch> child;
    VARIANT vChild;
    ::VariantInit(&vChild);
    V_VT(&vChild) = VT_I4;
    // In MSAA, index 0 is the object itself.
    V_I4(&vChild) = index + 1;
    if (FAILED(m_element->get_accChild(vChild, &child)))
        return 0;
    return COMPtr<IAccessible>(Query, child);*/
    return 0;
}

JSStringRef AccessibilityUIElement::allAttributes()
{
    return JSStringCreateWithCharacters(0, 0);
}

JSStringRef AccessibilityUIElement::attributesOfLinkedUIElements()
{
    return JSStringCreateWithCharacters(0, 0);
}

JSStringRef AccessibilityUIElement::attributesOfDocumentLinks()
{
    return JSStringCreateWithCharacters(0, 0);
}

AccessibilityUIElement AccessibilityUIElement::titleUIElement()
{
    return 0;
}

AccessibilityUIElement AccessibilityUIElement::parentElement()
{
    return 0;
}

JSStringRef AccessibilityUIElement::attributesOfChildren()
{
    return JSStringCreateWithCharacters(0, 0);
}

JSStringRef AccessibilityUIElement::parameterizedAttributeNames()
{
    return JSStringCreateWithCharacters(0, 0);
}

/*static VARIANT& self()
{
    static VARIANT vSelf;
    static bool haveInitialized;

    if (!haveInitialized) {
        ::VariantInit(&vSelf);
        V_VT(&vSelf) = VT_I4;
        V_I4(&vSelf) = CHILDID_SELF;
    }
    return vSelf;
}*/

JSStringRef AccessibilityUIElement::role()
{
    /*VARIANT vRole;
    if (FAILED(m_element->get_accRole(self(), &vRole)))
        return JSStringCreateWithCharacters(0, 0);
    ASSERT(V_VT(&vRole) == VT_I4);
    TCHAR roleText[64] = {0};
    ::GetRoleText(V_I4(&vRole), roleText, ARRAYSIZE(roleText));
    return JSStringCreateWithCharacters(roleText, _tcslen(roleText));*/
    return 0;
}

JSStringRef AccessibilityUIElement::title()
{
    /*BSTR titleBSTR;
    if (FAILED(m_element->get_accName(self(), &titleBSTR)) || !titleBSTR)
        return JSStringCreateWithCharacters(0, 0);
    wstring title(titleBSTR, SysStringLen(titleBSTR));
    ::SysFreeString(titleBSTR);
    return JSStringCreateWithCharacters(title.data(), title.length());*/
    return 0;
}

JSStringRef AccessibilityUIElement::description()
{
    /*BSTR descriptionBSTR;
    if (FAILED(m_element->get_accName(self(), &descriptionBSTR)) || !descriptionBSTR)
        return JSStringCreateWithCharacters(0, 0);
    wstring description(descriptionBSTR, SysStringLen(descriptionBSTR));
    ::SysFreeString(descriptionBSTR);
    return JSStringCreateWithCharacters(description.data(), description.length());*/
    return 0;
}

double AccessibilityUIElement::x()
{
    /*long x, y, width, height;
    if (FAILED(m_element->accLocation(&x, &y, &width, &height, self())))
        return 0;
    return x;*/
    return 0;
}

double AccessibilityUIElement::y()
{
    /*long x, y, width, height;
    if (FAILED(m_element->accLocation(&x, &y, &width, &height, self())))
        return 0;
    return y;*/
    return 0;
}

double AccessibilityUIElement::width()
{
    /*long x, y, width, height;
    if (FAILED(m_element->accLocation(&x, &y, &width, &height, self())))
        return 0;
    return width;*/
    return 0;
}

double AccessibilityUIElement::height()
{
    /*long x, y, width, height;
    if (FAILED(m_element->accLocation(&x, &y, &width, &height, self())))
        return 0;
    return height;*/
    return 0;
}

double AccessibilityUIElement::intValue()
{
    /*BSTR valueBSTR;
    if (FAILED(m_element->get_accValue(self(), &valueBSTR)) || !valueBSTR)
        return 0;
    wstring value(valueBSTR, SysStringLen(valueBSTR));
    ::SysFreeString(valueBSTR);
    TCHAR* ignored;
    return _tcstod(value.data(), &ignored);*/
    return 0;
}

double AccessibilityUIElement::minValue()
{
    return 0;
}

double AccessibilityUIElement::maxValue()
{
    return 0;
}

int AccessibilityUIElement::insertionPointLineNumber()
{
    return 0;
}

JSStringRef AccessibilityUIElement::attributesOfColumnHeaders()
{
    return JSStringCreateWithCharacters(0, 0);
}

JSStringRef AccessibilityUIElement::attributesOfRowHeaders()
{
    return JSStringCreateWithCharacters(0, 0);
}

JSStringRef AccessibilityUIElement::attributesOfColumns()
{
    return JSStringCreateWithCharacters(0, 0);
}

JSStringRef AccessibilityUIElement::attributesOfRows()
{
    return JSStringCreateWithCharacters(0, 0);
}

JSStringRef AccessibilityUIElement::attributesOfVisibleCells()
{
    return JSStringCreateWithCharacters(0, 0);
}

JSStringRef AccessibilityUIElement::attributesOfHeader()
{
    return JSStringCreateWithCharacters(0, 0);
}

int AccessibilityUIElement::indexInTable()
{
    return 0;
}

JSStringRef AccessibilityUIElement::rowIndexRange()
{
    return JSStringCreateWithCharacters(0, 0);
}

JSStringRef AccessibilityUIElement::columnIndexRange()
{
    return JSStringCreateWithCharacters(0, 0);
}

int AccessibilityUIElement::lineForIndex(int)
{
    return 0;
}

JSStringRef AccessibilityUIElement::boundsForRange(unsigned location, unsigned length)
{
    return JSStringCreateWithCharacters(0, 0);
}

AccessibilityUIElement AccessibilityUIElement::cellForColumnAndRow(unsigned column, unsigned row)
{
    return 0;
}

JSStringRef AccessibilityUIElement::selectedTextRange()
{
    return JSStringCreateWithCharacters(0, 0);    
}

void AccessibilityUIElement::setSelectedTextRange(unsigned location, unsigned length)
{
}

JSStringRef AccessibilityUIElement::attributeValue(JSStringRef attribute)
{
    return JSStringCreateWithCharacters(0, 0);
}

bool AccessibilityUIElement::isAttributeSettable(JSStringRef attribute)
{
    return false;
}

bool AccessibilityUIElement::isEnabled()
{
    return false;
}

double AccessibilityUIElement::clickPointX()
{
    return 0;
}

double AccessibilityUIElement::clickPointY()
{
    return 0;
}

JSStringRef AccessibilityUIElement::language()
{
    return JSStringCreateWithCharacters(0, 0);
}

JSStringRef AccessibilityUIElement::valueDescription()
{
    return 0;
}

bool AccessibilityUIElement::isRequired() const
{
    return false;
}

JSStringRef AccessibilityUIElement::subrole()
{
    return 0;
}

JSStringRef AccessibilityUIElement::roleDescription()
{
    // FIXME: implement
    return JSStringCreateWithCharacters(0, 0);
}

void AccessibilityUIElement::increment()
{
}

void AccessibilityUIElement::decrement()
{
}

bool AccessibilityUIElement::isActionSupported(JSStringRef action)
{
    return false;
}

JSStringRef AccessibilityUIElement::stringForRange(unsigned, unsigned)
{
    return JSStringCreateWithCharacters(0, 0);
}

AccessibilityUIElement AccessibilityUIElement::disclosedRowAtIndex(unsigned index)
{
    return 0;
}

AccessibilityUIElement AccessibilityUIElement::ariaOwnsElementAtIndex(unsigned index)
{
    return 0;
}

AccessibilityUIElement AccessibilityUIElement::selectedRowAtIndex(unsigned index)
{
    return 0;
}

AccessibilityUIElement AccessibilityUIElement::disclosedByRow()
{
    return 0;
}

void AccessibilityUIElement::showMenu()
{
    // FIXME: implement
}

JSStringRef AccessibilityUIElement::stringValue()
{
    // FIXME: implement
    return JSStringCreateWithCharacters(0, 0);
}

bool AccessibilityUIElement::isSelected() const
{
    // FIXME: implement
    return false;
}

AccessibilityUIElement AccessibilityUIElement::ariaFlowToElementAtIndex(unsigned index)
{
    return 0;
}

bool AccessibilityUIElement::isExpanded() const
{
    // FIXME: implement
    return false;
}

int AccessibilityUIElement::hierarchicalLevel() const
{
    // FIXME: implement
    return 0;
}

JSStringRef AccessibilityUIElement::accessibilityValue() const
{
    // FIXME: implement
    return JSStringCreateWithCharacters(0, 0);
}

JSStringRef AccessibilityUIElement::orientation() const
{
    // FIXME: implement
    return JSStringCreateWithCharacters(0, 0);
}

JSStringRef AccessibilityUIElement::documentEncoding()
{
    return JSStringCreateWithCharacters(0, 0);
}

JSStringRef AccessibilityUIElement::documentURI()
{
    // FIXME: implement
    return JSStringCreateWithCharacters(0, 0);
}

bool AccessibilityUIElement::ariaIsGrabbed() const
{
    return false;
}

JSStringRef AccessibilityUIElement::ariaDropEffects() const
{
    return 0;
}

bool AccessibilityUIElement::isAttributeSupported(JSStringRef attribute)
{
    return false;
}

JSStringRef AccessibilityUIElement::url()
{
    // FIXME: implement
    return JSStringCreateWithCharacters(0, 0);
}
