/*
 * Copyright (C) 2007 Pleyo.  All rights reserved.
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
#include "BALConfiguration.h"
#include "ChromeClientWebUI.h"
#include "Frame.h"
#include "FrameTree.h"
#include "FrameLoader.h"

namespace WebCore
{
bool ChromeClientWebUI::runDatabaseSizeLimitPrompt(Frame* frame, const String& msg)
{
    return false;
}

/*this function shows an Alert message*/
void ChromeClientWebUI::runJavaScriptAlert(Frame* frame, const String& msg)
{
	String script = "javascript: try { showAlert('"+msg+"');";
	script += "} catch(e) {};";

	if (m_frame->tree()->parent()) {
		while ((m_frame->tree()->name()) != "_ui") {
			frame = m_frame->tree()->parent();
		}
	}
#ifdef __OWB_JS__
	m_frame->loader()->executeScript(script, 1);
#endif //__OWB_JS__

}

bool ChromeClientWebUI::runJavaScriptConfirm(Frame* frame, const String& msg)
{
    printf("Javascript Confirm: %s (from frame %p), answer is 'false' by default.\n", msg.deprecatedString().ascii(), frame);   
    return true;
}

bool ChromeClientWebUI::runJavaScriptPrompt(Frame* frame, const String& msg, const String& defaultValue, String& result)
{
    printf("Javascript Prompt: %s (from frame %p), answer is 'false' by default.\n", msg.deprecatedString().ascii(), frame);   
    return true;
}

void ChromeClientWebUI::setStatusbarText(const String&)
{
    BALNotImplemented();
}

bool ChromeClientWebUI::shouldInterruptJavaScript()
{
    BALNotImplemented();
    return false;
}

bool ChromeClientWebUI::tabsToLinks() const
{
    return false;
}

}


