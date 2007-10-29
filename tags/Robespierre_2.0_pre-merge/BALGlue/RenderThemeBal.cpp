/*
 * This file is part of the WebKit project.
 *
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com
 * All rights reserved.
 * Copyright (C) 2007 Pleyo. All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include "config.h"
#include "wtf/Assertions.h"
#include "RenderThemeBal.h"

#include "BALConfiguration.h"
#include "Document.h"
#include "GraphicsContext.h"
#include "FrameBal.h"
#include "Pen.h"

#define THEME_COLOR 204
#define THEME_FONT  210

// Generic state constants
#define TS_NORMAL    1
#define TS_HOVER     2
#define TS_ACTIVE    3
#define TS_DISABLED  4
#define TS_FOCUSED   5

// Button constants
#define BP_BUTTON    1
#define BP_RADIO     2
#define BP_CHECKBOX  3

// Textfield constants
#define TFP_TEXTFIELD 1
#define TFS_READONLY  6

namespace WebCore
{

RenderTheme* theme()
{
    static RenderThemeBal winTheme;
    return &winTheme;
}

RenderThemeBal::RenderThemeBal()
{
}

RenderThemeBal::~RenderThemeBal()
{
}

void RenderThemeBal::close()
{
}

void RenderThemeBal::addIntrinsicMargins(RenderStyle* style) const
{
    // Cut out the intrinsic margins completely if we end up using a small font size
    if (style->fontSize() < 11)
        return;

    // Intrinsic margin value.
    const int m = 2;

    // FIXME: Using width/height alone and not also dealing with min-width/max-width is flawed.
    if (style->width().isIntrinsicOrAuto()) {
        if (style->marginLeft().quirk())
            style->setMarginLeft(Length(m, Fixed));
        if (style->marginRight().quirk())
            style->setMarginRight(Length(m, Fixed));
    }

    if (style->height().isAuto()) {
        if (style->marginTop().quirk())
            style->setMarginTop(Length(m, Fixed));
        if (style->marginBottom().quirk())
            style->setMarginBottom(Length(m, Fixed));
    }
}

bool RenderThemeBal::supportsFocus(EAppearance appearance)
{
    switch (appearance) {
    case PushButtonAppearance:
    case ButtonAppearance:
    case TextFieldAppearance:
        return true;
    default:
        return true;
    }

    return false;
}

unsigned RenderThemeBal::determineState(RenderObject* o)
{
    unsigned result = TS_NORMAL;
    if (!isEnabled(o))
        result = TS_DISABLED;
    else if (isReadOnlyControl(o))
        result = TFS_READONLY; // Readonly is supported on textfields.
    else if (supportsFocus(o->style()->appearance()) && isFocused(o))
        result = TS_FOCUSED;
    else if (isHovered(o))
        result = TS_HOVER;
    else if (isPressed(o))
        result = TS_ACTIVE;
    if (isChecked(o))
        result += 4; // 4 unchecked states, 4 checked states.
    return result;
}

ThemeData RenderThemeBal::getThemeData(RenderObject* o)
{
    ThemeData result;
    switch (o->style()->appearance()) {
    case PushButtonAppearance:
    case ButtonAppearance:
        result.m_part = BP_BUTTON;
        result.m_state = determineState(o);
        break;
    case CheckboxAppearance:
        result.m_part = BP_CHECKBOX;
        result.m_state = determineState(o);
        break;
    case RadioAppearance:
        result.m_part = BP_RADIO;
        result.m_state = determineState(o);
        break;
    case TextFieldAppearance:
        result.m_part = TFP_TEXTFIELD;
        result.m_state = determineState(o);
        break;
    default:
        break;
    }

    return result;
}

bool RenderThemeBal::paintCheckbox(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
    i.context->save();
    i.context->setStrokeStyle(SolidStroke);
    i.context->setStrokeColor(Color::black);
    RenderStyle style;
    addIntrinsicMargins(&style);

    EBorderStyle v = INSET;
    style.setBorderTopStyle(v);
    style.setBorderLeftStyle(v);
    style.setBorderBottomStyle(v);
    style.setBorderRightStyle(v);
    int borderWidth = 1;
    style.setBorderTopWidth(borderWidth);
    style.setBorderLeftWidth(borderWidth);
    style.setBorderBottomWidth(borderWidth);
    style.setBorderRightWidth(borderWidth);
    o->paintBackgroundExtended(i.context,
        o->style()->backgroundColor(), o->style()->backgroundLayers(),
        r.y(), o->height(), r.x(), r.y(), o->width(), o->height(),
        o->borderLeft(), o->borderRight());
    o->paintBorder(i.context,
        r.x(), r.y(), r.width(), r.height(),
        &style, true, true);

    if( o->element()->active() ) {
        IntRect r2(r);
        r2.inflate(-2);
        i.context->setFillColor(WebCore::Color(0xc0,0xc0,0xc0));
        i.context->drawRect(r2);
        v = GROOVE;
        style.setBorderTopStyle(v);
        style.setBorderLeftStyle(v);
        style.setBorderBottomStyle(v);
        style.setBorderRightStyle(v);
        int borderWidth = 1;
        style.setBorderTopWidth(borderWidth);
        style.setBorderLeftWidth(borderWidth);
        style.setBorderBottomWidth(borderWidth);
        style.setBorderRightWidth(borderWidth);
        o->paintBackgroundExtended(i.context,
            o->style()->backgroundColor(), o->style()->backgroundLayers(),
            r.y(), o->height(), r.x(), r.y(), o->width(), o->height(),
            o->borderLeft(), o->borderRight());
        o->paintBorder(i.context,
            r.x(), r.y(), r.width(), r.height(),
            &style, true, true);
    }
    if(o->element()->isChecked()) {
        i.context->setStrokeColor(Color::black);
        IntRect r2(r);
        r2.inflate(-borderWidth-2);
        i.context->drawLine(IntPoint(r2.x(),r2.y()), IntPoint(r2.x()+r2.width(),r2.y()+r2.height()));
        i.context->drawLine(IntPoint(r2.x()+r2.width(),r2.y()), IntPoint(r2.x(),r2.y()+r2.height()));
    }
    i.context->restore();
    return false;
}

bool RenderThemeBal::paintRadio(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
//    return paintButton( o, i, r );
    // NOTE would be nice if we could do :
    // Image* radioboxImage = Image::loadPlatformResource("radiobox.png");
    i.context->save();
    i.context->setStrokeStyle(SolidStroke);
    RenderStyle style;
    EBorderStyle v = RIDGE;
    style.setBorderTopStyle(v);
    style.setBorderLeftStyle(v);
    style.setBorderBottomStyle(v);
    style.setBorderRightStyle(v);
    int borderWidth = 1;
    style.setBorderTopWidth(borderWidth);
    style.setBorderLeftWidth(borderWidth);
    style.setBorderBottomWidth(borderWidth);
    style.setBorderRightWidth(borderWidth);

    o->paintBackgroundExtended(i.context,
        o->style()->backgroundColor(), o->style()->backgroundLayers(),
        r.y(), o->height(), r.x(), r.y(), o->width(), o->height(),
        o->borderLeft(), o->borderRight());
    o->paintBorder(i.context,
        r.x(), r.y(), r.width(), r.height(),
        &style, true, true);
    if(o->element()->isChecked()) {
        IntRect r2(r);
        r2.inflate(-borderWidth - 2);
        i.context->drawRect(r2);
    }
    i.context->restore();
    return false;
}

void RenderThemeBal::adjustButtonStyle(CSSStyleSelector*, RenderStyle* style, WebCore::Element*) const
{
    addIntrinsicMargins(style);
}

bool RenderThemeBal::paintButton(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
    o->paintBackgroundExtended(i.context,
        o->style()->backgroundColor(), o->style()->backgroundLayers(),
        r.y(), o->height(), r.x(), r.y(), o->width(), o->height(),
        o->borderLeft(), o->borderRight());
    o->paintBorder(i.context,
        r.x(), r.y(), r.width(), r.height(),
        o->style(), true, true);
    return false;
}

void RenderThemeBal::setCheckboxSize(RenderStyle* style) const
{
    // If the width and height are both specified, then we have nothing to do.
    if (!style->width().isIntrinsicOrAuto() && !style->height().isAuto())
        return;

    // FIXME:  A hard-coded size of 13 is used.  This is wrong but necessary for now.  It matches Firefox.
    // At different DPI settings on Windows, querying the theme gives you a larger size that accounts for
    // the higher DPI.  Until our entire engine honors a DPI setting other than 96, we can't rely on the theme's
    // metrics.
    if (style->width().isIntrinsicOrAuto())
        style->setWidth(Length(13, Fixed));
    if (style->height().isAuto())
        style->setHeight(Length(13, Fixed));
}

void RenderThemeBal::setRadioSize(RenderStyle* style) const
{
    // This is the same as checkboxes.
    setCheckboxSize(style);
}

void RenderThemeBal::adjustTextFieldStyle(CSSStyleSelector* selector, RenderStyle* style, WebCore::Element* e) const
{
  //RenderTheme::adjustTextFieldStyle( selector, style, e );
}

// Input text
bool RenderThemeBal::paintTextField(RenderObject* aObject, const RenderObject::PaintInfo& i , const IntRect& r)
{
  return true; // true means "let's the background be paint"
}

bool RenderThemeBal::paintTextArea(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
    return paintTextField(o, i, r);
}

void RenderThemeBal::systemFont(int propId, FontDescription& fdesc) const
{
}

bool RenderThemeBal::paintMenuList(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
    RenderStyle style;
    style.setMarginLeft(Length(10, Fixed));
    style.setMarginTop(Length(10, Fixed));
    EBorderStyle v = INSET;
    style.setBorderTopStyle(v);
    style.setBorderLeftStyle(v);
    style.setBorderBottomStyle(v);
    style.setBorderRightStyle(v);
    style.setBorderTopWidth(1);
    style.setBorderLeftWidth(1);
    style.setBorderBottomWidth(1);
    style.setBorderRightWidth(1);
    o->paintBackgroundExtended(i.context,
        o->style()->backgroundColor(), o->style()->backgroundLayers(),
        r.y(), o->height(), r.x(), r.y(), o->width(), o->height(),
        o->borderLeft(), o->borderRight());
    o->paintBorder(i.context,
        r.x(), r.y(), r.width(), r.height(),
        &style, true, true);
    return false;
}

bool RenderThemeBal::paintMenuListButton(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
  BALNotImplemented();
  return true;
}

}
