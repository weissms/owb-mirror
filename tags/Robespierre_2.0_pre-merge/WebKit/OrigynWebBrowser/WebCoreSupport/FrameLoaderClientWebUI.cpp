/*
 * Copyright (C) 2006 Don Gibson <dgibson77@gmail.com>
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2007 Trolltech ASA
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
#include "FrameLoaderClientWebUI.h"
#include "FrameBal.h"
#include "FrameTree.h"
#include "FrameLoader.h"
#include "FrameView.h"
#include "Page.h"
#include "ProgressTracker.h"

using namespace BAL;

namespace WebCore {

/*this function allows to call javaScript function in UI, the parameter in entry is the name of JS function*/
void executeJS(Frame* frame, String code)
{
	String script = "javascript: try { " + code;
	script += "} catch(e) {};";

	if (frame->tree()->parent()) {
		while ((frame->tree()->name()) != "_ui") {
			frame = frame->tree()->parent();
		}
	}
#ifdef __OWB_JS__
	frame->loader()->executeScript(0, script, 1);
#endif //__OWB_JS__
}


FrameLoaderClientWebUI::~FrameLoaderClientWebUI()
{
    for (uint i = 0; i < childFrame.size(); i++)
    {
        //hack to free frame
        Frame *f=childFrame.at(i).get();
        f->deref();
        childFrame.at(i)=0;
    }
    childFrame.clear();
}

/*this function allows to realize the notification, calling JS function "loadBar" with the 	
percentage of loading in parameter */
void FrameLoaderClientWebUI::postProgressEstimateChangedNotification()
{
	char s[4];
	double progress = m_frame->page()->progress()->estimatedProgress();
	int valp = static_cast<int>(progress * 100);
	sprintf(s, "%i", valp);
	if ((m_frame->tree()->name()) != "_ui")
		executeJS(m_frame , "loadBar('" + String(s) + "')");
//  	m_frame->view()->setDirtyRect(IntRect(0, 0, m_frame->view()->width(), m_frame->view()->height()));
}

/*this fonction allows to show the title of iframe in the title bar*/
void FrameLoaderClientWebUI::dispatchDidReceiveTitle(const String& title)
{
	if ((m_frame->tree()->name())!="_ui") {
		String message = title.deprecatedString().ascii();
		message = message.replace("'","\\'");
		executeJS(m_frame, "showTitle('"+message+"')");
	}
}
/*this function shows an error message in a popup*/
void FrameLoaderClientWebUI::dispatchDidFailProvisionalLoad(const ResourceError& error)
{
    if (!(error.isNull())) {
        char errorCode[2];
        sprintf(errorCode, "%i", error.errorCode());
        String description = error.localizedDescription().deprecatedString().ascii();
        description = description.replace("'","\\'");
        executeJS(m_frame, "errorLoad('" + String(error.domain().deprecatedString().ascii()) + "','" + String(errorCode) + "','" + String(error.failingURL().deprecatedString().ascii()) + "','" + description + "')");
    }
 
    m_loadFailed = true;

}
Frame* FrameLoaderClientWebUI::createFrame(const KURL& url, const String& name, HTMLFrameOwnerElement* ownerElement,
                                 const String& referrer, bool allowsScrolling, int marginWidth, int marginHeight)
{
    // new frame must have its frameloaderclient
    FrameLoaderClientBal* frameLoader = new FrameLoaderClientWebUI();
    // we create here our frame
    childFrame.append( new FrameBal(frame()->page(), ownerElement, frameLoader) );
    frameLoader->setFrame(static_cast<FrameBal*>(childFrame.last().get()));
 
    // do not attach new child to parent frame here, or else WindowBal positionning and frame paints will be wrong
    childFrame.last()->view()->setParent(frame()->view());
    // FIXME: All of the below should probably be moved over into WebCore
    childFrame.last()->tree()->setName(name);
    frame()->tree()->appendChild(childFrame.last());

 
    // load url in child
    FrameLoadType loadType = frame()->loader()->loadType();
    FrameLoadType childLoadType = FrameLoadTypeInternal;
    childFrame.last()->loader()->load(url, referrer, childLoadType, name, 0, 0, HashMap<String, String>());
 
    // The frame's onload handler may have removed it from the document.
    if (!childFrame.last()->tree()->parent())
        return 0;
    
    return childFrame.last().get();
}
}
