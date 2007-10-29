/**
 * @file BCResourceHandleManagerCURL.cpp
 *
 * BAL Transfer Job Manager implementation, with curl
 */

#include "config.h"
#include "BALConfiguration.h"
#include "BCResourceHandleManagerCURL.h"
#include "BCResourceHandleCURL.h"
#include "BIResourceHandle.h"
#include "BTLogHelper.h"

using BC::BCResourceHandleManagerCURL;
using BAL::BIResourceHandleManager;
using BAL::BIResourceHandle;

IMPLEMENT_GET_DELETE(BIResourceHandleManager, BCResourceHandleManagerCURL);

// CURL Implementation
namespace BC
{
const int selectTimeoutMS = 5;
const double pollTimeSeconds = 0.0;

BCResourceHandleManagerCURL::BCResourceHandleManagerCURL()
    : m_downloadTimer(this, &BCResourceHandleManagerCURL::downloadTimerCallback)
    , m_cookieJarFileName("/tmp/cookies-owb.txt")
{
    curl_global_init(CURL_GLOBAL_ALL);
    m_curlMultiHandle = curl_multi_init();
    m_curlShareHandle = curl_share_init();
    curl_share_setopt(m_curlShareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
}
BCResourceHandleManagerCURL::~BCResourceHandleManagerCURL()
{
    curl_share_cleanup(m_curlShareHandle);
    curl_multi_cleanup(m_curlMultiHandle);
}

void BCResourceHandleManagerCURL::setCookieJarFileName(const char* cookieJarFileName)
{
    m_cookieJarFileName = strdup(cookieJarFileName);
}


/**
 * downloadTimerCallback. At each call, check and perform transfers.
 */
void BCResourceHandleManagerCURL::downloadTimerCallback(WebCore::Timer<BCResourceHandleManagerCURL>* timer)
{
    fd_set fdread;
    fd_set fdwrite;
    fd_set fdexcep;
    int maxfd = 0;

    FD_ZERO(&fdread);
    FD_ZERO(&fdwrite);
    FD_ZERO(&fdexcep);
    curl_multi_fdset(m_curlMultiHandle, &fdread, &fdwrite, &fdexcep, &maxfd);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = selectTimeoutMS * 1000;       // select waits microseconds

    WebCore::setDeferringTimers(true);
    int rc = ::select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
    WebCore::setDeferringTimers(false);

    if (-1 == rc) {
        // we may end here if a timer fires on Linux (signals interrupt select)
        // so we must either set timeout to 0, or enclose select()
        // between setDeferringTimers() to disable timers
#ifndef NDEBUG
        printf("bad: select() returned -1\n");
#endif
        return;
    }

    int runningHandles = 0;
    CURLMcode curlCode = CURLM_CALL_MULTI_PERFORM;
    while (CURLM_CALL_MULTI_PERFORM == curlCode) {
        curlCode = curl_multi_perform(m_curlMultiHandle, &runningHandles);
    }

    // check the curl messages indicating completed transfers
    // and free their resources
    int messagesInQueue;
    while (CURLMsg* msg = curl_multi_info_read(m_curlMultiHandle, &messagesInQueue)) {
        CURL* handle = msg->easy_handle;
        assert(handle);
        BCResourceHandleCURL* job;
        curl_easy_getinfo(handle, CURLINFO_PRIVATE, &job);
        job->processMessage(msg);
    }

    if (!m_downloadTimer.isActive() && (runningHandles > 0))
        m_downloadTimer.startOneShot(pollTimeSeconds);
}

/**
 * remove a job. If no more job to work,
 * stop the download timer.
 */
void BCResourceHandleManagerCURL::remove(BIResourceHandle *job)
{
    BCResourceHandleCURL* curlJob = static_cast<BCResourceHandleCURL*> (job);
    ASSERT(curlJob->handle());
    if(contains(job))
        curl_multi_remove_handle(m_curlMultiHandle, curlJob->handle());
    curl_easy_cleanup(curlJob->handle());
    curlJob->setHandle(0);
}

/**
 * add a job to transfer.
 */
void BCResourceHandleManagerCURL::add(BIResourceHandle *resourceHandle)
{
    BCResourceHandleCURL* job = static_cast<BCResourceHandleCURL*> (resourceHandle);
    logml(MODULE_NETWORK, LEVEL_WARNING,
      make_message("ADD TRANSFER JOB: METHOD=%s for %s \n", job->method().deprecatedString().ascii(), job->url().url().ascii()));

    CURLMcode ret = curl_multi_add_handle(m_curlMultiHandle, job->handle());
    // don't call perform, because events must be async
    // timeout will occur and do curl_multi_perform
    if (ret && ret != CURLM_CALL_MULTI_PERFORM) {
#ifndef NDEBUG
        printf("Error %d starting job %s\n", ret, job->url().url().ascii());
#endif
        job->cancel();
        return;
    }

    if (!m_downloadTimer.isActive())
        m_downloadTimer.startOneShot(pollTimeSeconds);

}

CURLcode BCResourceHandleManagerCURL::runImmediately(BIResourceHandle* job)
{
    //cause data to be cached in m_resultData
     BCResourceHandleCURL* curlJob = static_cast<BCResourceHandleCURL*> (job);
    CURL* handle = curlJob->handle();
    CURLcode res = CURLE_OK;

	if (handle) {
        curl_multi_remove_handle(m_curlMultiHandle, handle);
        res = curl_easy_perform(handle);
    }

    return res;
}

bool BCResourceHandleManagerCURL::contains(BIResourceHandle* job)
{
     BCResourceHandleCURL* tmp = 0;
     BCResourceHandleCURL* curlJob = static_cast<BCResourceHandleCURL*> (job);
     curl_easy_getinfo(curlJob->handle(), CURLINFO_PRIVATE, &tmp);
     return (tmp == curlJob);
}

/**
 * cancel a job. i.e. remove it from the job's list,
 * and set it in error.
 */
void BCResourceHandleManagerCURL::cancel(BIResourceHandle *job)
{
    remove(job);
    static_cast<BCResourceHandleCURL*> (job)->deref();
}

} // namespace WebCore
