#ifndef BCResourceHandleManagerCURL_H_
#define BCResourceHandleManagerCURL_H_

#include "BIResourceHandleManager.h"
#include "../JavaScriptCore/wtf/HashSet.h"
#include "Timer.h"
#include <curl/curl.h>

using BAL::BIResourceHandleManager;
using BAL::BIResourceHandle;

namespace BC
{

/**
 * BAL Transfer Job Manager Implementation, with CURL
 */
class BCResourceHandleManagerCURL : public BIResourceHandleManager
{
public:
    BCResourceHandleManagerCURL();

    virtual void add(BIResourceHandle* job);
    virtual bool contains(BIResourceHandle* job);
    virtual void cancel(BIResourceHandle* job);
    virtual CURLcode runImmediately(BIResourceHandle* job);
    void remove(BIResourceHandle* job);

    void setCookieJarFileName(const char* cookieJarFileName);
    char* cookieJarFileName() { return m_cookieJarFileName; }


private:
    void downloadTimerCallback(WebCore::Timer<BCResourceHandleManagerCURL>* timer);

    WebCore::Timer<BCResourceHandleManagerCURL> m_downloadTimer;

    CURLM* m_curlMultiHandle; // FIXME: never freed
    CURLSH* m_curlShareHandle; // FIXME: never freed
    char* m_cookieJarFileName; // FIXME: never freed

    char m_curlErrorBuffer[CURL_ERROR_SIZE];
};

}

#endif /*ResourceHandleManagerCURL_H_*/
