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
#ifndef BIWINDOW_H_
#define BIWINDOW_H_

namespace WebCore {

    class IntRect;

}

namespace BAL {

    class BIPainter;

    class BIWindow {
        public:
            virtual int x() = 0;
            virtual int y() = 0;
            virtual int width() = 0;
            virtual int height() = 0;
            virtual int depth() = 0;
            virtual void clear(const WebCore::IntRect&) = 0;
            virtual void redraw(const WebCore::IntRect&) = 0;
            virtual void expose(const WebCore::IntRect&) = 0;
            virtual void scroll(int deltaX, int deltaY) = 0;
            virtual void setPainter(BIPainter*) = 0;
        // this is mandatory
            virtual ~BIWindow() {};

    };

}

#define IMPLEMENT_BIWINDOW  \
    public: \
            virtual int x(); \
            virtual int y(); \
            virtual int width(); \
            virtual int height(); \
            virtual int depth(); \
            virtual void clear(const WebCore::IntRect&); \
            virtual void redraw(const WebCore::IntRect&); \
            virtual void expose(const WebCore::IntRect&); \
            virtual void scroll(int deltaX, int deltaY); \
            virtual void setPainter(BIPainter*);


#endif // BIWINDOW_H
