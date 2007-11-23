/**
 * @file BCResourceHandleCURL.cpp
 *
 * BAL Implementation of transfer job, with curl
 */

#include "config.h"
#include "BALConfiguration.h"
#include "BCResourceHandleManagerCURL.h"
#include "BCResourceHandleCURL.h"
#include "BICookieJar.h"
#include "BIObserverService.h"
#include "DeprecatedString.h"
#include "FormData.h"
#include <curl/curl.h>

using BAL::BIResourceHandle;
using BC::BCResourceHandleCURL;
using WebCore::DeprecatedString;
using WebCore::FormDataElement;

// Implementation
namespace BC {

static char error_buffer[CURL_ERROR_SIZE];

static size_t writeCallback(void* ptr, size_t size, size_t nmemb, void* obj)
{
    BCResourceHandleCURL* job = static_cast<BCResourceHandleCURL*> (obj);
    ASSERT(job);
    return job->write(ptr, size, nmemb);
}

static size_t headerCallback(char* ptr, size_t size, size_t nmemb, void* obj)
{
    BCResourceHandleCURL* job = static_cast<BCResourceHandleCURL*> (obj);
    return job->header(ptr, size, nmemb);
}



BCResourceHandleCURL::BCResourceHandleCURL(const ResourceRequest& request, ResourceHandleClient* client, bool defersLoading, bool shouldContentSniff, bool mightDownloadFromHandle)
    : BCResourceHandleCommonImplementation(this, request, client, defersLoading, shouldContentSniff, mightDownloadFromHandle)
    , m_handle(0)
    , m_url(0)
    , m_customHeaders(0)
    , m_useSimple(false)
{
    BAL::getBIObserverService()->registerObserver(String("SendCookies"), this);
    DeprecatedString surl = url().url();
    m_loading = true;
    m_response.setUrl(url());

    //remove any query part sent to a local file
    //allows http style get options to be handled by a local file
    if (url().isLocalFile()) {
        DeprecatedString query = url().query();
        if (!query.isEmpty()) {
            unsigned int idx = surl.find(query);
            surl = surl.left(idx);
        }
    }
    m_handle = curl_easy_init();
    // url ptr must remain valid through the request
    m_url =  strdup(surl.ascii());

    curl_easy_setopt(m_handle, CURLOPT_PRIVATE, this);
    curl_easy_setopt(m_handle, CURLOPT_ERRORBUFFER, error_buffer);
    curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(m_handle, CURLOPT_HEADERFUNCTION, headerCallback);
    curl_easy_setopt(m_handle, CURLOPT_WRITEHEADER, this);
    curl_easy_setopt(m_handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(m_handle, CURLOPT_MAXREDIRS, 10);
    curl_easy_setopt(m_handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    // enable gzip and deflate through Accept-Encoding:
    curl_easy_setopt(m_handle, CURLOPT_ENCODING, "");
    curl_easy_setopt(m_handle, CURLOPT_URL, m_url);
    m_request.setHTTPHeaderField("Connection", "Keep-Alive");

    //xmlHttpRequest see xml/xmlhttprequest.cpp
    HTTPHeaderMap customHeaders =  m_request.httpHeaderFields();
    if (!customHeaders.isEmpty()) {
        struct curl_slist *slist=NULL;
        HTTPHeaderMap::const_iterator end = customHeaders.end();
        
        for (HTTPHeaderMap::const_iterator it = customHeaders.begin(); it != end; ++it) {
            String key = it->first;
            String value = it->second;
            String pair = key+": "+value;
            CString cstring = pair.utf8();

            //save for later cleanup
            m_cstrings.append(cstring);
            slist = curl_slist_append(slist, cstring.data());
        }
        
        curl_easy_setopt(m_handle, CURLOPT_HTTPHEADER, slist);
        m_customHeader = slist;
    }
    
    //default to get
    curl_easy_setopt(m_handle, CURLOPT_HTTPGET, true);
    if (method() == "POST") 
        setupPOST();
    else if (method() == "PUT")
        setupPUT();
    else if (method() == "HEAD") 
        curl_easy_setopt(m_handle, CURLOPT_NOBODY, true);
    checkAndSendCookies();
}

BCResourceHandleCURL::~BCResourceHandleCURL()
{
    BAL::getBIObserverService()->removeObserver(String("SendCookies"), this);
    BAL::getBIResourceHandleManager()->cancel(this); // ensure that job is removed from manager's job list
    if (m_customHeaders)
        curl_slist_free_all(m_customHeaders);
    if (m_customHeader)
        curl_slist_free_all(m_customHeader);

	if (m_handle)
    	curl_easy_cleanup(m_handle);
}

void BCResourceHandleCURL::setURL(const char* url)
{ 
    if (m_url) free(m_url);
    m_url = strdup(url);
    
    //send the new url to curl
    curl_easy_setopt(m_handle, CURLOPT_URL, m_url);
}

void  BCResourceHandleCURL::setupPUT()
{
    if (method() != "PUT")
        return;
    curl_easy_setopt(m_handle, CURLOPT_UPLOAD, true) ;
    struct curl_slist *slist=NULL;
    //FIXME: hmmm disable Expect: 100-continue
    //just supprot Http 1.1 chuncked encoding now
    slist = curl_slist_append(slist, "Transfer-Encoding: chunked");
    curl_easy_setopt(m_handle, CURLOPT_HTTPHEADER, slist);
    m_customPostHeader = slist;
}

void  BCResourceHandleCURL::setupPOST()
{
    if (method() != "POST")
        return;
    curl_easy_setopt(m_handle, CURLOPT_POST, true);
    //disable Expect: 100-continue
    {
        struct curl_slist *slist = NULL;
        slist = curl_slist_append(slist, "Expect:");
        curl_easy_setopt(m_handle, CURLOPT_HTTPHEADER, slist);
        m_customPostHeader = slist;
    }
    
    //first add any data
    static CString data;
    data = postData()->flattenToString().latin1();
    DBGML(MODULE_NETWORK, LEVEL_WARNING, "ADD TRANSFER JOB: DATA=%s\n", data.data());

    m_request.setHTTPHeaderField("PropagateHttpHeader", "true");
    m_request.setHTTPContentType("Content-Type: application/x-www-form-urlencoded");
    if (data.length() != 0) {
        curl_easy_setopt(m_handle, CURLOPT_POSTFIELDS, data.data());
        curl_easy_setopt(m_handle, CURLOPT_POSTFIELDSIZE, data.length());
    }
        
    Vector<FormDataElement> elements = postData()->elements();
    int size = elements.size();
    struct curl_httppost *lastptr = NULL;

    for (int i = 0; i < size; i++) {
        if (elements[i].m_type == FormDataElement::encodedFile) {
            CString cstring =  elements[i].m_filename.utf8();
            //keep ref here so its not freed
            m_cstrings.append(cstring);
            const char *filename = cstring.data();

            /* Fill in the file upload field */
            curl_formadd(&m_filePost,
                         &lastptr,
                         CURLFORM_COPYNAME, "sendfile",
                         CURLFORM_FILE, filename,
                         CURLFORM_END);

            /* Fill in the filename field */
            curl_formadd(&m_filePost,
                         &lastptr,
                         CURLFORM_COPYNAME, "filename",
                         CURLFORM_COPYCONTENTS, filename,
                         CURLFORM_END);

            /* Fill in the submit field too, even if this is rarely needed */
            curl_formadd(&m_filePost,
                         &lastptr,
                         CURLFORM_COPYNAME, "submit",
                         CURLFORM_COPYCONTENTS, "send",
                         CURLFORM_END);
        }
    }
    
    
}

void BCResourceHandleCURL::clearAuthentication()
{
    //implementation from WebCore/platform/network/ResourceHandle.cpp
    m_currentWebChallenge.nullify();
}

void BCResourceHandleCURL::cancel()
{
    //FIXME: send error on cancel ?
    if (!m_sentResponse) {
        m_sentResponse = true;
    }
    finish();
    return;
}

//All this indirection for runImmediately is beacuse
//create both creates the ResourceHandle and starts it.
// if we split these or pass a boolean in its much better
// than this. I think its important to have the capability
// but the api and impl needs more thought.
Vector<char>  BCResourceHandleCURL::runImmediately(ResourceResponse& response)
{
    m_useSimple = true;
    BCResourceHandleManagerCURL* mgr = static_cast<BCResourceHandleManagerCURL*>(BAL::getBIResourceHandleManager());
    CURLcode res = mgr->runImmediately(this);
    //FIXME: make error descriptive this may be broken
    if(res != CURLE_OK ) {
        m_error = ResourceError();
        client()->didFail(this, ResourceError());
	}

    Vector<char> results = Vector<char> (m_resultData);
    m_resultData.clear();
    response = m_response;
    //copy of callback part of finish
    DBGML(MODULE_NETWORK, LEVEL_WARNING, "IMMEDIATE TRANSFER JOB: METHOD=%s\n", m_url);
    if (client()) {
        //empty page if true
        if (!m_sentResponse) {
            m_sentResponse = true;
            client()->didReceiveResponse(this, m_response);
        }
        client()->didFinishLoading(this);
    }
    deref();
    return results;
}

PassRefPtr<SharedBuffer> BCResourceHandleCURL::bufferedData()
{
    BALNotImplemented();
    return 0;
}

void BCResourceHandleCURL::setDefersLoading(bool defers)
{
    BALNotImplemented();
    return;
}

size_t BCResourceHandleCURL::write(void* ptr, size_t size, size_t nmemb)
{
    if (!client())
        return 0;
    long code = 0;
    CURLcode res = curl_easy_getinfo(handle(), CURLINFO_RESPONSE_CODE, &code);
    // avoid having redirection messages in the body ! see http://google.com/ for an example
	if (code >= 300 && code < 400) {
        return size*nmemb;
	}

    if (!m_sentResponse) {
        m_sentResponse = true;
        if(client())
            client()->didReceiveResponse(this, m_response);
    }

    int totalSize = size * nmemb;
    if(client())
        client()->didReceiveData(this, static_cast<char*> (ptr), totalSize, totalSize);

    if (m_useSimple) {
        m_resultData.append(static_cast<char*> (ptr), totalSize);
    }

    return totalSize;
}

size_t BCResourceHandleCURL::header(char* ptr, size_t size, size_t nmemb)
{
    int realsize = size * nmemb;
    String header = String(ptr, realsize);

    DBGML(MODULE_NETWORK, LEVEL_WARNING, "CURL Receive Header\n\t%s for %s \n", ptr, m_response.url().url().ascii());
    // Means 'end of header'
    if (realsize == 2 && header.contains('\n')) {
        m_sentResponse = true;
        if(client())
            client()->didReceiveResponse(this, m_response);
        return realsize;
    }

    int index = header.find(':');
    String name = header.substring(0, index + 1).stripWhiteSpace();
    String value = header.substring(index + 1).stripWhiteSpace();

    //FIXME: this should not be const
    const HTTPHeaderMap map = m_response.httpHeaderFields();
    const_cast<HTTPHeaderMap*> (&map)->add(name, value);

    // Reset CURLHeader and set status code
    if (header.contains("HTTP", true)) {
        String status = header.substring(9, realsize).left(3);
        m_response.setHTTPStatusCode(status.toInt());
        // Set URL for Response
        if (m_response.url().isEmpty()) {
            m_response.setUrl(url());
            int index = m_response.url().url().findRev('/');
            //use remove because right() method from deprecatedString seems to have a bug!
            String filename = m_response.url().url().remove(0, index + 1);
            if (!filename.isEmpty())
                m_response.setSuggestedFilename(filename);
            else {
                String defaultFilename = String("index.html");
                m_response.setSuggestedFilename(defaultFilename);
            }
        }
        return realsize;
    }

    // Set Mime type and eventually charset for Response
    if (header.contains("Content-Type: ", false)) {
        String content = header.substring(14, realsize);

        int ret = content.find("\r");
        if (ret >= 0)
            content.truncate(ret);

        int index = content.find(';');
        if (index >= 0) {
            String charset = content.substring(index + 1, content.length());
            int equal = charset.find("=");
            if (equal >= 0)
                charset.remove(0, equal + 1);
            m_response.setTextEncodingName(charset);

            content.truncate(index);
        }

        m_response.setMimeType(content);
        return realsize;
    }

    // Set expected length
    if (header.contains("Content-Length: ", false)) {
        String length = header.substring(16, realsize);
        m_response.setExpectedContentLength(length.toInt());
        return realsize;
    }

    // Set Location
    if (header.contains("Location: ", false)) {
        String location, receivedLocation = header.substring(10, realsize);
        
        //handle the case of a relative path
        if (!receivedLocation.contains("://",false)) {
            int index = 0;
            
            //relative to the domain
            if (receivedLocation.startsWith("/",false)) {
                //we are looking for the 3rd '/' of the url ex : http://www.domain.com/ <- this one
                for (int i=0; i<3; i++) {
                    while (m_url[index] != '/')
                        index++;
                    index++;
                }
                //the index should be located 2 caracters before the slash if we want the slash to be troncated
                index = index-2;
            }
            
            //relative to the url
            else {
                //we're looking backwards for the first '/' of the url to remove the name of the page
                index = strlen(m_url);
                while (m_url[index] != '/')
                    index--;
            }
            String absolute(m_url, index+1);
            absolute.append(receivedLocation);
            location = absolute;
            printf("The W3C recommands that Location headers uses absolute paths and not relative ones, this website used :' %s '\n", header.deprecatedString().ascii());
        }
        else
            location = receivedLocation;
        
        int ret = location.find("\r");
        if (ret >= 0)
            location.truncate(ret);
        
        m_request.setURL(location.deprecatedString());
        // a redirection occured, we must change the request or else we will
        // only have the main document with all dependencies messed up
        // mixing previous base url with new paths. It will end up with a
        // serie of 404 errors, so change the request now.
        
        this->setURL(location.deprecatedString().ascii());
        
        if (client())
            client()->willSendRequest(this, m_request, m_response);

        return realsize;
    }

    if (header.contains("Set-Cookie: ", false)) {
        //BCResourceHandleManagerCURL* mgr = static_cast<BCResourceHandleManagerCURL*> (BAL::getBIResourceHandleManager());
        
        int ret = header.find("\r");
        if (ret >= 0)
            header.truncate(ret);

        // Add url to cookies parameters. This url will be extracted from params in observe().
        BAL::getBIObserverService()->notifyObserver(String("Set-Cookie"), header.substring(12, realsize)+"; URL="+getURL());
    }

    return realsize;
}

void BCResourceHandleCURL::observe(const String& topic, const String& data)
{
    checkAndSendCookies(KURL(data.deprecatedString()));
}

void  BCResourceHandleCURL::processMessage(CURLMsg* msg)
{
    if (msg->msg == CURLMSG_DONE) {
        // find the node which has same m_handle as completed transfer
        if (msg->data.result != CURLE_OK) {
            char *handle_url = NULL;
            curl_easy_getinfo(m_handle, CURLINFO_EFFECTIVE_URL, &handle_url);
            const char *error =  curl_easy_strerror(msg->data.result);
            DBGML(MODULE_NETWORK, LEVEL_WARNING, "JOB:%s ERROR=%s\n", handle_url, error);
            // Set error code to '1' as it is not a HTTP error.
            ResourceError jobError(url().host(), 1, url().url(), String(error));
            //FIXME: make error descriptive
            if(client()) {
                client()->didFail(this, jobError); //didFail release resources.
                return;
            }
		    if (handle_url)
	    	   free(handle_url);
        }
        finish();
    }
}

void  BCResourceHandleCURL::finish()
{
    if (client()) {
        //empty page if true
        //FIXME: should we really do this here on a cancelled page ?
        if (!m_sentResponse) {
            m_sentResponse = true;
            client()->didReceiveResponse(this, m_response);
        }
        client()->didFinishLoading(this);
    }
    m_client = NULL;
}

void BCResourceHandleCURL::checkAndSendCookies(KURL url)
{
    //cookies are a part of the http protocol only
    if (!String(m_url).startsWith("http"))
        return;
    
    if (url == "")
        url = m_url;
    
    DBGML(MODULE_FACILITIES, LEVEL_INFO, "Method : %s \nCheck&Send Cookies for : %s \n", method().deprecatedString().ascii(), url.url().ascii());
    //prepare a cookie header if there are cookies related to this url.
    String cookiePairs;
    cookiePairs = getBICookieJar()->cookies(url);
    
    /* We choose a max size of 81920 caracters because a cookie max size is 4096 and a domain can have at max 20 cookies so 20*4096=81920 */
    //FIXME: if an url depends on more than one map, cookie string may be truncated
    static char cookieChar[81920];
    strncpy(cookieChar,cookiePairs.deprecatedString().ascii(),81920);
    
    if (!cookiePairs.isEmpty() && m_handle) {
        DBGML(MODULE_FACILITIES, LEVEL_INFO, "\n CURL POST Cookie : %s \n", cookieChar);
        curl_easy_setopt(m_handle, CURLOPT_COOKIE, cookieChar);
    }
}

} // namespace BC

//
// static members of BIResourceHandle
//

namespace BAL {

PassRefPtr<BIResourceHandle> BIResourceHandle::create(const ResourceRequest& request,
        ResourceHandleClient* client, Frame* frame, bool defersLoading, bool shouldContentSniff, bool mightDownloadFromHandle)
{
    RefPtr<ResourceHandle> newLoader(new BCResourceHandleCURL(request, client, defersLoading, shouldContentSniff, mightDownloadFromHandle));
    BAL::getBIResourceHandleManager()->add(newLoader.get());

    return newLoader;
}


void BIResourceHandle::loadResourceSynchronously(const ResourceRequest&, ResourceError& error,
        ResourceResponse&, Vector<char>& data)
{
    BALNotImplemented();
    return;
}

bool BIResourceHandle::willLoadFromCache(ResourceRequest&)
{
    BALNotImplemented();
    return false;
}

bool BIResourceHandle::supportsBufferedData()
{
    BALNotImplemented();
    return false;
}

bool BIResourceHandle::loadsBlocked()
{
    BALNotImplemented();
    return false;
}

} // namespace BAL
