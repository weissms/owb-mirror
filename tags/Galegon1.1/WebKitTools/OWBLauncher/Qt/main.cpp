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

#include <WebKit.h>
#include <QtGui/qwidget.h>
#include <QObject>
#include <QEvent>
#include <QMainWindow>
#include <QApplication>
#include <QRect>
#include <QFile>
#include <QTextStream>

using namespace WebCore;

class QWebView : public QWidget
{
    Q_OBJECT
public:
    explicit QWebView(QWidget *parent = 0) : QWidget(parent) {
        setAcceptDrops(true);

        setMouseTracking(true);
        setFocusPolicy(Qt::WheelFocus);
//        setAttribute(Qt::WA_OpaquePaintEvent, this);
        m_webView = WebView::createInstance();

        QRect clientRect(0, 0, 800, 600);
    
        m_webView->initWithFrame(clientRect, "", "");
        m_webView->setViewWindow(this);
    };

    virtual ~QWebView() {
        delete m_webView;
    };

    void setWebView(WebView *webView) {
        m_webView = webView;
    }

    virtual bool event(QEvent *ev){
      switch (ev->type()) {
        case QEvent::Paint:
            m_webView->onExpose(static_cast<QPaintEvent*>(ev));
            break;
        case QEvent::Timer:
            QObject::timerEvent(static_cast<QTimerEvent*>(ev));
            break;
        case QEvent::MouseMove:
            m_webView->onMouseMotion(static_cast<QMouseEvent*>(ev));
            break;
        case QEvent::MouseButtonPress:
            m_webView->onMouseButtonDown(static_cast<QMouseEvent*>(ev));
            break;
        case QEvent::MouseButtonRelease:
            m_webView->onMouseButtonUp(static_cast<QMouseEvent*>(ev));
            break;
        case QEvent::Wheel:
            m_webView->onScroll(static_cast<QWheelEvent*>(ev));
            break;
        case QEvent::KeyPress:
            m_webView->onKeyDown(static_cast<QKeyEvent*>(ev));
            break;
        case QEvent::KeyRelease:
            m_webView->onKeyUp(static_cast<QKeyEvent*>(ev));
            break;
        case QEvent::Resize:
            m_webView->onResize(static_cast<QResizeEvent*>(ev));
            break;
        case QEvent::FocusIn:
            //m_webView->onExpose(static_cast<QFocusEvent*>(ev));
            break;
        case QEvent::FocusOut:
            //d->focusOutEvent(static_cast<QFocusEvent*>(ev));
            break;
        case QEvent::InputMethod:
            //d->inputMethodEvent(static_cast<QInputMethodEvent*>(ev));
            break;
        case QEvent::Close:
            m_webView->onQuit(ev);
            emit quitWindow();
            break;
        default:
             return QObject::event(ev);
        }

        return true;
  
    };

    QSize sizeHint() const {
        QRect rect = m_webView->frameRect();
        return QSize(rect.width(), rect.height());
    }

    void loadUrl(const char* url) {
        printf("load url=%s\n", url);
        m_webView->mainFrame()->loadURL(url);
    }
private:
    WebView *m_webView;
Q_SIGNALS:
    void quitWindow();

};
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(const char* url = NULL) {
        view = new QWebView(this);
        setCentralWidget(view);
        printf("url %s\n", url);
        if (url)
            view->loadUrl(url);
        connect(view, SIGNAL(quitWindow()), this, SLOT(close()));
    }
    ~MainWindow() {
        delete view;
    }
    QWebView* qwebview() { return view;}
private:
    QWebView *view;
};


int main (int argc, char* argv[])
{

    QApplication app(argc, argv);
    app.setApplicationName("OWBLauncher");

    char* uri = (char*) (argc > 1 ? argv[1] : "http://www.google.com/");
    MainWindow window(uri);

//    window.qwebview()->update();

    window.show();
    return app.exec();

    /*QRect clientRect= {0, 0, 800, 600};
   
    webView->initWithFrame(clientRect, "", "");

    QWidget *view = webView->viewWindow();*/


    /*if(webView->canZoomPageIn()) {
        printf("canZoomPageIn\n");
        webView->zoomPageIn();
    }*/
    /*if(webView->canMakeTextLarger()) {
        printf("canMakeTextLarger\n");
        webView->makeTextLarger();
    }*/


    return 0;
}

#include "main.moc"
