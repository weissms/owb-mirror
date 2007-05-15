/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2007 Pleyo.  All rights reserved.
 * 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "BALConfiguration.h"
#include "EditorClientBal.h"

#include "Editor.h"
#include "FocusController.h"
#include "Frame.h"
#include "KeyboardCodes.h"
#include "KeyboardEvent.h"
#include "Page.h"
#include "PlatformKeyboardEvent.h"

namespace WebCore {

bool EditorClientBal::shouldDeleteRange(Range*)
{
    return true;
}

bool EditorClientBal::shouldShowDeleteInterface(HTMLElement*)
{
    return false;
}

bool EditorClientBal::isContinuousSpellCheckingEnabled()
{
    BALNotImplemented();
    return false;
}

bool EditorClientBal::isGrammarCheckingEnabled()
{
    BALNotImplemented();
    return false;
}

int EditorClientBal::spellCheckerDocumentTag()
{
    BALNotImplemented();
    return 0;
}

bool EditorClientBal::shouldBeginEditing(WebCore::Range*)
{
    return true;
}

bool EditorClientBal::shouldEndEditing(WebCore::Range*)
{
    return true;
}

bool EditorClientBal::shouldInsertText(String, Range*, EditorInsertAction)
{
    return true;
}

bool EditorClientBal::shouldChangeSelectedRange(Range* fromRange, Range* toRange, EAffinity, bool stillSelecting)
{
    return true;
}

bool EditorClientBal::shouldApplyStyle(WebCore::CSSStyleDeclaration*,
                                      WebCore::Range*)
{
    return true;
}

void EditorClientBal::didBeginEditing()
{
    m_editing = true;
}

void EditorClientBal::respondToChangedContents()
{
    BALNotImplemented();
}

void EditorClientBal::didEndEditing()
{
    m_editing = false;
}

void EditorClientBal::didWriteSelectionToPasteboard()
{
    BALNotImplemented();
}

void EditorClientBal::didSetSelectionTypesForPasteboard()
{
    BALNotImplemented();
}

bool EditorClientBal::selectWordBeforeMenuEvent()
{
    BALNotImplemented();
    return false;
}

bool EditorClientBal::isEditable()
{
    // FIXME: should be controllable by a setting in QWebPage
    return false;
}

void EditorClientBal::registerCommandForUndo(WTF::PassRefPtr<WebCore::EditCommand> cmd)
{
    BALNotImplemented();
}

void EditorClientBal::registerCommandForRedo(WTF::PassRefPtr<WebCore::EditCommand>)
{
}

void EditorClientBal::clearUndoRedoOperations()
{
    BALNotImplemented();
}

bool EditorClientBal::canUndo() const
{
    BALNotImplemented();
    return false;
}

bool EditorClientBal::canRedo() const
{
    BALNotImplemented();
    return false;
}

void EditorClientBal::undo()
{
    m_inUndoRedo = true;
    BALNotImplemented();
    m_inUndoRedo = false;
}

void EditorClientBal::redo()
{
    m_inUndoRedo = true;
    BALNotImplemented();
    m_inUndoRedo = false;
}

bool EditorClientBal::shouldInsertNode(Node*, Range*, EditorInsertAction)
{
    return true;
}

void EditorClientBal::pageDestroyed()
{
    BALNotImplemented();
}

bool EditorClientBal::smartInsertDeleteEnabled()
{
    BALNotImplemented();
    return false;
}

void EditorClientBal::toggleContinuousSpellChecking()
{
    BALNotImplemented();
}

void EditorClientBal::toggleGrammarChecking()
{
    BALNotImplemented();
}

void EditorClientBal::handleKeyPress(KeyboardEvent* event)
{
    Frame* frame = m_page->focusController()->focusedOrMainFrame();
    if (!frame)
        return;

    const PlatformKeyboardEvent* kevent = event->keyEvent();
    if (!kevent->isKeyUp()) {
        Node* start = frame->selectionController()->start().node();
        if (start && start->isContentEditable()) {
            switch(kevent->WindowsKeyCode()) {
            case VK_BACK:
                frame->editor()->deleteWithDirection(SelectionController::BACKWARD,
                                                     CharacterGranularity, false, true);
                event->setDefaultHandled();
                break;
            case VK_DELETE:
                frame->editor()->deleteWithDirection(SelectionController::FORWARD,
                                                     CharacterGranularity, false, true);
                event->setDefaultHandled();
                break;
            case VK_LEFT:
                if (kevent->shiftKey())
                    frame->editor()->execCommand("MoveLeftAndModifySelection");
                else frame->editor()->execCommand("MoveLeft");
                event->setDefaultHandled();
                break;
            case VK_RIGHT:
                if (kevent->shiftKey())
                    frame->editor()->execCommand("MoveRightAndModifySelection");
                else frame->editor()->execCommand("MoveRight");
                event->setDefaultHandled();
                break;
            case VK_UP:
                if (kevent->shiftKey())
                    frame->editor()->execCommand("MoveUpAndModifySelection");
                else frame->editor()->execCommand("MoveUp");
                event->setDefaultHandled();
                break;
            case VK_DOWN:
                if (kevent->shiftKey())
                    frame->editor()->execCommand("MoveDownAndModifySelection");
                else frame->editor()->execCommand("MoveDown");
                event->setDefaultHandled();
                break;
            case VK_RETURN:
                if (start->isContentRichlyEditable()) {
                    frame->editor()->insertLineBreak();
                    event->setDefaultHandled();
                }
                break;
            case VK_TAB:
                // do not handle tab, leave it to event handler
                break;
            default:
                if (!kevent->ctrlKey() && !kevent->altKey())
                    frame->editor()->insertText(kevent->text(), false, event);
                event->setDefaultHandled();
            }
        }
    }
}

EditorClientBal::EditorClientBal()
    : m_page(0), m_editing(false), m_inUndoRedo(false)
{
}

void EditorClientBal::textFieldDidBeginEditing(Element*)
{
    m_editing = true;
}

void EditorClientBal::textFieldDidEndEditing(Element*)
{
    m_editing = false;
}

void EditorClientBal::textDidChangeInTextField(Element*)
{
}

bool EditorClientBal::doTextFieldCommandFromEvent(Element*, KeyboardEvent*)
{
    return false;
}

void EditorClientBal::textWillBeDeletedInTextField(Element*)
{
}

void EditorClientBal::textDidChangeInTextArea(Element*)
{
}

bool EditorClientBal::isEditing() const
{
    return m_editing;
}

void EditorClientBal::setPage(Page *page)
{
    m_page = page;
}

}

// vim: ts=4 sw=4 et
