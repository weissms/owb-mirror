#ifndef BCResourceHandleManagerCURL_H_
#define BCResourceHandleManagerCURL_H_

#include "BIResourceHandleManager.h"
#include "../JavaScriptCore/wtf/HashSet.h"
#include "Timer.h"
#include <curl/curl.h>
#include <wtf/Vector.h>

using BAL::BIResourceHandleManager;
using BAL::BTResourceHandle;

namespace BC
{

/**
 * BAL Transfer Job Manager Implementation, with CURL
 */
class BCResourceHandleManagerCURL : public BIResourceHandleManager
{
public:
    BCResourceHandleManagerCURL();
    ~BCResourceHandleManagerCURL();

    virtual void add(ResourceHandle* job);
//     virtual bool contains(ResourceHandle* job);
    virtual void cancel(ResourceHandle* job);
//     virtual CURLcode runImmediately(ResourceHandle* job);
//     void remove(BIResourceHandle* job);

    virtual void setupPOST(ResourceHandle*, struct curl_slist**);
    virtual void setupPUT(ResourceHandle*, struct curl_slist**);

    static BIResourceHandleManager* sharedInstance();
    void setCookieJarFileName(const char* cookieJarFileName);
    char* cookieJarFileName() { return m_cookieJarFileName; }


private:
    void downloadTimerCallback(WebCore::Timer<BCResourceHandleManagerCURL>* timer);
    void removeFromCurl(ResourceHandle*);
    bool removeScheduledJob(ResourceHandle*);
    void startJob(ResourceHandle*);
    bool startScheduledJobs();

    Timer<BCResourceHandleManagerCURL> m_downloadTimer;
    CURLM* m_curlMultiHandle;
    CURLSH* m_curlShareHandle;
    char* m_cookieJarFileName;
    char m_curlErrorBuffer[CURL_ERROR_SIZE];
    Vector<ResourceHandle*> m_resourceHandleList;
    int m_runningJobs;
};

}

#endif /*ResourceHandleManagerCURL_H_*/
