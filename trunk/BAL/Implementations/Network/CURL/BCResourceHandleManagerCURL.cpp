/**
 * @file BCResourceHandleManagerCURL.cpp
 *
 * BAL Transfer Job Manager implementation, with curl
 */

#include "config.h"
#include "BALConfiguration.h"
#include "BCResourceHandleManagerCURL.h"
#include "BTResourceHandle.h"
#include "CString.h"
#include <sys/stat.h>
#include "MIMETypeRegistry.h"
#include "HTTPParsers.h"
#include "Base64.h"

#include <errno.h>
#include "ResourceHandleInternal.h"
#include "BTLogHelper.h"

using BC::BCResourceHandleManagerCURL;
using BAL::BIResourceHandleManager;
using BAL::BTResourceHandle;

IMPLEMENT_GET_DELETE(BIResourceHandleManager, BCResourceHandleManagerCURL);

// CURL Implementation
namespace BC
{
const int selectTimeoutMS = 5;
const double pollTimeSeconds = 0.05;
const int maxRunningJobs = 5;

BCResourceHandleManagerCURL::BCResourceHandleManagerCURL()
    : m_downloadTimer(this, &BCResourceHandleManagerCURL::downloadTimerCallback)
    , m_cookieJarFileName(0)
    , m_runningJobs(0)
{
    curl_global_init(CURL_GLOBAL_ALL);
    m_curlMultiHandle = curl_multi_init();
    m_curlShareHandle = curl_share_init();
    curl_share_setopt(m_curlShareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
    curl_share_setopt(m_curlShareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
}

BCResourceHandleManagerCURL::~BCResourceHandleManagerCURL()
{
    curl_share_cleanup(m_curlShareHandle);
    curl_multi_cleanup(m_curlMultiHandle);
    if (m_cookieJarFileName)
        free(m_cookieJarFileName);
}

void BCResourceHandleManagerCURL::setCookieJarFileName(const char* cookieJarFileName)
{
    m_cookieJarFileName = strdup(cookieJarFileName);
}

// called with data after all headers have been processed via headerCallback
static size_t writeCallback(void* ptr, size_t size, size_t nmemb, void* data)
{
    ResourceHandle* job = static_cast<ResourceHandle*>(data);
    ResourceHandleInternal* d = job->getInternal();
    if (d->m_cancelled)
        return 0;
    size_t totalSize = size * nmemb;

    // this shouldn't be necessary but apparently is. CURL writes the data
    // of html page even if it is a redirect that was handled internally
    // can be observed e.g. on gmail.com
    CURL* h = d->m_handle;
    long httpCode = 0;
    CURLcode err = curl_easy_getinfo(h, CURLINFO_RESPONSE_CODE, &httpCode);
    if (CURLE_OK == err && httpCode >= 300 && httpCode < 400)
        return totalSize;

    // since the code in headerCallback will not have run for local files
    // the code to set the URL and fire didReceiveResponse is never run,
    // which means the ResourceLoader's response does not contain the URL.
    // Run the code here for local files to resolve the issue.
    // TODO: See if there is a better approach for handling this.
    if (!d->m_response.responseFired()) {
        const char* hdr;
        err = curl_easy_getinfo(h, CURLINFO_EFFECTIVE_URL, &hdr);
        d->m_response.setUrl(KURL(hdr));
        if (d->client())
            d->client()->didReceiveResponse(job, d->m_response);
        d->m_response.setResponseFired(true);
    }

    if (d->client())
        d->client()->didReceiveData(job, static_cast<char*>(ptr), totalSize, 0);
    return totalSize;
}

/*
 * This is being called for each HTTP header in the response. This includes '\r\n'
 * for the last line of the header.
 *
 * We will add each HTTP Header to the ResourceResponse and on the termination
 * of the header (\r\n) we will parse Content-Type and Content-Disposition and
 * update the ResourceResponse and then send it away.
 *
 */
static size_t headerCallback(char* ptr, size_t size, size_t nmemb, void* data)
{
    ResourceHandle* job = static_cast<ResourceHandle*>(data);
    ResourceHandleInternal* d = job->getInternal();

    if (d->m_cancelled)
        return 0;
    size_t totalSize = size * nmemb;
    ResourceHandleClient* client = d->client();

    WebCore::String header(static_cast<const char*>(ptr), totalSize);

    /*
    * a) We can finish and send the ResourceResponse
    * b) We will add the current header to the HTTPHeaderMap of the ResourceResponse
    */
    if (header == WebCore::String("\r\n")) {
        CURL* h = d->m_handle;
        CURLcode err;

        double contentLength = 0;
        err = curl_easy_getinfo(h, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &contentLength);
        d->m_response.setExpectedContentLength(static_cast<long long int>(contentLength));

        const char* hdr;
        err = curl_easy_getinfo(h, CURLINFO_EFFECTIVE_URL, &hdr);
        d->m_response.setUrl(KURL(hdr));

        long httpCode = 0;
        err = curl_easy_getinfo(h, CURLINFO_RESPONSE_CODE, &httpCode);
        d->m_response.setHTTPStatusCode(httpCode);

        d->m_response.setMimeType(extractMIMETypeFromMediaType(d->m_response.httpHeaderField("Content-Type")));
        d->m_response.setTextEncodingName(extractCharsetFromMediaType(d->m_response.httpHeaderField("Content-Type")));
        d->m_response.setSuggestedFilename(filenameFromHTTPContentDisposition(d->m_response.httpHeaderField("Content-Disposition")));

        // HTTP redirection
        if (httpCode >= 300 && httpCode < 400) {
            WebCore::String location = d->m_response.httpHeaderField("location");
            if (!location.isEmpty()) {
                KURL newURL = KURL(job->request().url(), location.deprecatedString());

                ResourceRequest redirectedRequest = job->request();
                redirectedRequest.setURL(newURL);
                if (client)
                    client->willSendRequest(job, redirectedRequest, d->m_response);

                d->m_request.setURL(newURL);

                return totalSize;
            }
        }

        if (client)
            client->didReceiveResponse(job, d->m_response);
        d->m_response.setResponseFired(true);
    } else {
        int splitPos = header.find(":");
        if (splitPos != -1)
            d->m_response.setHTTPHeaderField(header.left(splitPos), header.substring(splitPos+1).stripWhiteSpace());
    }

    return totalSize;
}

/* This is called to obtain HTTP POST or PUT data.
   Iterate through FormData elements and upload files.
   Carefully respect the given buffer size and fill the rest of the data at the next calls.
*/
size_t readCallback(void* ptr, size_t size, size_t nmemb, void* data)
{
    ResourceHandle* job = static_cast<ResourceHandle*>(data);
    ResourceHandleInternal* d = job->getInternal();
    if (d->m_cancelled)
        return 0;

    size_t sent = 0;
    size_t toSend = size * nmemb;
    if (!toSend)
        return 0;

    Vector<WebCore::FormDataElement> elements = job->request().httpBody()->elements();
    if (d->m_formDataElementIndex >= elements.size())
        return 0;

    WebCore::FormDataElement element = elements[d->m_formDataElementIndex];

    if (element.m_type == WebCore::FormDataElement::encodedFile) {
        if (!d->m_file)
            d->m_file = fopen(element.m_filename.utf8().data(), "rb");

        if (!d->m_file) {
            // FIXME: show a user error?
#ifndef NDEBUG
            DBG("Failed while trying to open %s for upload\n", element.m_filename.utf8().data());
#endif
            job->cancel();
            return 0;
        }

        sent = fread(ptr, size, nmemb, d->m_file);
        if (!size && ferror(d->m_file)) {
            // FIXME: show a user error?
#ifndef NDEBUG
            DBG("Failed while trying to read %s for upload\n", element.m_filename.utf8().data());
#endif
            job->cancel();
            return 0;
        }
        if (feof(d->m_file)) {
            fclose(d->m_file);
            d->m_file = 0;
            d->m_formDataElementIndex++;
        }
    } else {
        size_t elementSize = element.m_data.size() - d->m_formDataElementDataOffset;
        sent = elementSize > toSend ? toSend : elementSize;
        memcpy(ptr, element.m_data.data() + d->m_formDataElementDataOffset, sent);
        if (elementSize > sent)
            d->m_formDataElementDataOffset += sent;
        else {
            d->m_formDataElementDataOffset = 0;
            d->m_formDataElementIndex++;
        }
    }

    return sent;
}

/**
 * downloadTimerCallback. At each call, check and perform transfers.
 */
void BCResourceHandleManagerCURL::downloadTimerCallback(WebCore::Timer<BCResourceHandleManagerCURL>* timer)
{
    startScheduledJobs();

    fd_set fdread;
    fd_set fdwrite;
    fd_set fdexcep;
    int maxfd = 0;

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = selectTimeoutMS * 1000;       // select waits microseconds

    // Temporarily disable timers since signals may interrupt select(), raising EINTR errors on some platforms
    WebCore::setDeferringTimers(true);
    int rc;
    do {
        FD_ZERO(&fdread);
        FD_ZERO(&fdwrite);
        FD_ZERO(&fdexcep);
        curl_multi_fdset(m_curlMultiHandle, &fdread, &fdwrite, &fdexcep, &maxfd);
        rc = ::select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
    } while (rc == -1 && errno == EINTR);
    WebCore::setDeferringTimers(false);

    if (maxfd > -1 && -1 == rc) {
        // we may end here if a timer fires on Linux (signals interrupt select)
        // so we must either set timeout to 0, or enclose select()
        // between setDeferringTimers() to disable timers
#ifndef NDEBUG
        perror("bad: select() returned -1\n");
#endif
        return;
    }

    int runningHandles = 0;
    while (curl_multi_perform(m_curlMultiHandle, &runningHandles) == CURLM_CALL_MULTI_PERFORM) { }

    // check the curl messages indicating completed transfers
    // and free their resources
    while (true) {
        int messagesInQueue;
        CURLMsg* msg = curl_multi_info_read(m_curlMultiHandle, &messagesInQueue);
        if (!msg)
            break;

        // find the node which has same d->m_handle as completed transfer
        CURL* handle = msg->easy_handle;
        ASSERT(handle);
        ResourceHandle* job = 0;
        CURLcode err = curl_easy_getinfo(handle, CURLINFO_PRIVATE, &job);
        ASSERT(CURLE_OK == err);
        ASSERT(job);
        if (!job)
            continue;
        ResourceHandleInternal* d = job->getInternal();
        ASSERT(d->m_handle == handle);

        if (d->m_cancelled) {
            removeFromCurl(job);
            continue;
        }

        if (CURLMSG_DONE != msg->msg)
            continue;

        if (CURLE_OK == msg->data.result) {
            if (d->client())
                d->client()->didFinishLoading(job);
        } else {
#ifndef NDEBUG
            char* url = 0;
            curl_easy_getinfo(d->m_handle, CURLINFO_EFFECTIVE_URL, &url);
            DBGML(MODULE_NETWORK, LEVEL_WARNING, "Curl ERROR for url='%s', error: '%s'\n", url, curl_easy_strerror(msg->data.result));
#endif
            if (d->client())
                d->client()->didFail(job, ResourceError());
        }

        removeFromCurl(job);
    }

    bool started = startScheduledJobs(); // new jobs might have been added in the meantime

    if (!m_downloadTimer.isActive() && (started || runningHandles > 0))
        m_downloadTimer.startOneShot(pollTimeSeconds);
}

/**
 * remove a job. If no more job to work,
 * stop the download timer.
 */
void BCResourceHandleManagerCURL::removeFromCurl(ResourceHandle* job)
{
    ResourceHandleInternal* d = job->getInternal();
    ASSERT(d->m_handle);
    if (!d->m_handle)
        return;
    m_runningJobs--;
    curl_multi_remove_handle(m_curlMultiHandle, d->m_handle);
    curl_easy_cleanup(d->m_handle);
    d->m_handle = 0;
}

void BCResourceHandleManagerCURL::setupPUT(ResourceHandle*, struct curl_slist**)
{
}

/* Calculate the length of the POST.
   Force chunked data transfer if size of files can't be obtained.
*/
void BCResourceHandleManagerCURL::setupPOST(ResourceHandle* job, struct curl_slist** headers)
{
    ResourceHandleInternal* d = job->getInternal();
    Vector<WebCore::FormDataElement> elements = job->request().httpBody()->elements();
    size_t numElements = elements.size();

    if (!numElements)
        return;

    // Do not stream for simple POST data
    if (numElements == 1) {
        job->request().httpBody()->flatten(d->m_postBytes);
        if (d->m_postBytes.size() != 0) {
            curl_easy_setopt(d->m_handle, CURLOPT_POST, true);
            curl_easy_setopt(d->m_handle, CURLOPT_POSTFIELDSIZE, d->m_postBytes.size());
            curl_easy_setopt(d->m_handle, CURLOPT_POSTFIELDS, d->m_postBytes.data());
        }
        return;
    }

    // Obtain the total size of the POST
    static const long long maxCurlOffT = (1LL << (sizeof(curl_off_t) * 8 - 1)) - 1;
    curl_off_t size = 0;
    bool chunkedTransfer = false;
    for (size_t i = 0; i < numElements; i++) {
        WebCore::FormDataElement element = elements[i];
        if (element.m_type == WebCore::FormDataElement::encodedFile) {
            struct stat fileStat;
            stat(element.m_filename.utf8().data(), &fileStat);
            long long fileSizeResult = (long long) fileStat.st_size;
            if (fileSizeResult) {
                if (fileSizeResult > maxCurlOffT) {
                    // File size is too big for specifying it to cURL
                    chunkedTransfer = true;
                    break;
                }
                size += fileSizeResult;
            } else {
                chunkedTransfer = true;
                break;
            }
        } else
            size += elements[i].m_data.size();
    }
    curl_easy_setopt(d->m_handle, CURLOPT_POST, true);

    // cURL guesses that we want chunked encoding as long as we specify the header
    if (chunkedTransfer)
        *headers = curl_slist_append(*headers, "Transfer-Encoding: chunked");
    else
        curl_easy_setopt(d->m_handle, CURLOPT_POSTFIELDSIZE_LARGE, size);

    curl_easy_setopt(d->m_handle, CURLOPT_READFUNCTION, readCallback);
    curl_easy_setopt(d->m_handle, CURLOPT_READDATA, job);
}

/**
 * add a job to transfer.
 */
void BCResourceHandleManagerCURL::add(ResourceHandle *job)
{
    // we can be called from within curl, so to avoid re-entrancy issues
    // schedule this job to be added the next time we enter curl download loop
    m_resourceHandleList.append(job);
    if (!m_downloadTimer.isActive())
        m_downloadTimer.startOneShot(pollTimeSeconds);

}

bool BCResourceHandleManagerCURL::removeScheduledJob(ResourceHandle* job)
{
    int size = m_resourceHandleList.size();
    for (int i = 0; i < size; i++) {
        if (job == m_resourceHandleList[i]) {
            m_resourceHandleList.remove(i);
            return true;
        }
    }
    return false;
}

bool BCResourceHandleManagerCURL::startScheduledJobs()
{
    // TODO: Create a separate stack of jobs for each domain.

    bool started = false;
    while (!m_resourceHandleList.isEmpty() && m_runningJobs < maxRunningJobs) {
        ResourceHandle* job = m_resourceHandleList[0];
        startJob(job);
        m_resourceHandleList.remove(0);
        started = true;
    }
    return started;
}

static void parseDataUrl(ResourceHandle* handle)
{
    DeprecatedString data = handle->request().url().deprecatedString();

    ASSERT(data.startsWith("data:", false));

    DeprecatedString header;
    bool base64 = false;

    int index = data.find(',');
    if (index != -1) {
        header = data.mid(5, index - 5).lower();
        data = data.mid(index + 1);

        if (header.endsWith(";base64")) {
            base64 = true;
            header = header.left(header.length() - 7);
        }
    } else
        data = DeprecatedString();

    data = KURL::decode_string(data);

    if (base64) {
        Vector<char> out;
        if (WebCore::base64Decode(data.ascii(), data.length(), out))
            data = DeprecatedString(out.data(), out.size());
        else
            data = DeprecatedString();
    }

    if (header.isEmpty())
        header = "text/plain;charset=US-ASCII";

    ResourceHandleClient* client = handle->getInternal()->client();

    ResourceResponse response;

    response.setMimeType(extractMIMETypeFromMediaType(header));
    response.setTextEncodingName(extractCharsetFromMediaType(header));
    response.setExpectedContentLength(data.length());
    response.setHTTPStatusCode(200);

    client->didReceiveResponse(handle, response);

    if (!data.isEmpty())
        client->didReceiveData(handle, data.ascii(), data.length(), 0);

        client->didFinishLoading(handle);
}

void BCResourceHandleManagerCURL::startJob(ResourceHandle* job)
{
    KURL kurl = job->request().url();
    WebCore::String protocol = kurl.protocol();

    if (equalIgnoringCase(protocol, "data")) {
        parseDataUrl(job);
        return;
    }

    ResourceHandleInternal* d = job->getInternal();
    DeprecatedString url = kurl.deprecatedString();

    if (kurl.isLocalFile()) {
        DeprecatedString query = kurl.query();
        // Remove any query part sent to a local file.
            if (!query.isEmpty())
                url = url.left(url.find(query));
            // Determine the MIME type based on the path.
            d->m_response.setMimeType(WebCore::MIMETypeRegistry::getMIMETypeForPath(WebCore::String(url)));
    }

    d->m_handle = curl_easy_init();
#ifndef NDEBUG
    if (getenv("DEBUG_CURL"))
        curl_easy_setopt(d->m_handle, CURLOPT_VERBOSE, 1);
#endif
    curl_easy_setopt(d->m_handle, CURLOPT_PRIVATE, job);
    curl_easy_setopt(d->m_handle, CURLOPT_ERRORBUFFER, m_curlErrorBuffer);
    curl_easy_setopt(d->m_handle, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(d->m_handle, CURLOPT_WRITEDATA, job);
    curl_easy_setopt(d->m_handle, CURLOPT_HEADERFUNCTION, headerCallback);
    curl_easy_setopt(d->m_handle, CURLOPT_WRITEHEADER, job);
    curl_easy_setopt(d->m_handle, CURLOPT_AUTOREFERER, 1);
    curl_easy_setopt(d->m_handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(d->m_handle, CURLOPT_MAXREDIRS, 10);
    curl_easy_setopt(d->m_handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    curl_easy_setopt(d->m_handle, CURLOPT_SHARE, m_curlShareHandle);
    curl_easy_setopt(d->m_handle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5); // 5 minutes
    // enable gzip and deflate through Accept-Encoding:
    curl_easy_setopt(d->m_handle, CURLOPT_ENCODING, "");

    // url must remain valid through the request
    ASSERT(!d->m_url);
    d->m_url = strdup(url.ascii());
    curl_easy_setopt(d->m_handle, CURLOPT_URL, d->m_url);

    if (m_cookieJarFileName) {
        curl_easy_setopt(d->m_handle, CURLOPT_COOKIEFILE, m_cookieJarFileName);
        curl_easy_setopt(d->m_handle, CURLOPT_COOKIEJAR, m_cookieJarFileName);
    }

    struct curl_slist* headers = 0;
    if (job->request().httpHeaderFields().size() > 0) {
        WebCore::HTTPHeaderMap customHeaders = job->request().httpHeaderFields();
        WebCore::HTTPHeaderMap::const_iterator end = customHeaders.end();
        for (WebCore::HTTPHeaderMap::const_iterator it = customHeaders.begin(); it != end; ++it) {
            WebCore::String key = it->first;
            WebCore::String value = it->second;
            WebCore::String headerString(key);
            headerString.append(": ");
            headerString.append(value);
            WebCore::CString headerLatin1 = headerString.latin1();
            headers = curl_slist_append(headers, headerLatin1.data());
        }
    }

    if ("GET" == job->request().httpMethod())
        curl_easy_setopt(d->m_handle, CURLOPT_HTTPGET, true);
    else if ("POST" == job->request().httpMethod())
        setupPOST(job, &headers);
    else if ("PUT" == job->request().httpMethod())
        setupPUT(job, &headers);
    else if ("HEAD" == job->request().httpMethod())
        curl_easy_setopt(d->m_handle, CURLOPT_NOBODY, true);

    if (headers) {
        curl_easy_setopt(d->m_handle, CURLOPT_HTTPHEADER, headers);
        d->m_customHeaders = headers;
    }

    m_runningJobs++;
    CURLMcode ret = curl_multi_add_handle(m_curlMultiHandle, d->m_handle);
    // don't call perform, because events must be async
    // timeout will occur and do curl_multi_perform
    if (ret && ret != CURLM_CALL_MULTI_PERFORM) {
#ifndef NDEBUG
        printf("Error %d starting job %s\n", ret, job->request().url().deprecatedString().ascii());
#endif
        job->cancel();
        return;
    }
}

void BCResourceHandleManagerCURL::cancel(ResourceHandle* job)
{
    if (removeScheduledJob(job))
        return;

    ResourceHandleInternal* d = job->getInternal();
    d->m_cancelled = true;
    if (!m_downloadTimer.isActive())
        m_downloadTimer.startOneShot(pollTimeSeconds);
}

} // namespace WebCore
