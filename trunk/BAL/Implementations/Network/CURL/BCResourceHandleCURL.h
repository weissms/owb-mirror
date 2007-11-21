#ifndef BCResourceHandleCURL_H_
#define BCResourceHandleCURL_H_

#include "BALConfiguration.h"
#include "../Common/BCResourceHandleCommonImplementation.h"
#include "BIResourceHandle.h"
#include "BTFormData.h"
#include "BTLogHelper.h"
#include "CString.h"
#include <curl/curl.h>
#include "DeprecatedString.h"
#include "PlatformString.h"
#include <SharedBuffer.h>
#include "StringHash.h"
#include "Timer.h"
#include <Vector.h>
#include <wtf/HashMap.h>
#include "BIObserver.h"

class KURL;

using WebCore::CString;
using WebCore::DeprecatedString;
using WebCore::FormData;
using WebCore::SharedBuffer;
using WebCore::String;
using BAL::BIObserver;

namespace BC {

/**
 * BAL Transfer Job Implementation, with CURL.
 */
    class BCResourceHandleCURL : public BAL::BIObserver, public BAL::BIResourceHandle, public BCResourceHandleCommonImplementation {
public:
    BCResourceHandleCURL(const ResourceRequest&, ResourceHandleClient*, bool defersLoading, bool shouldContentSniff, bool mightDownloadFromHandle);

    virtual ~BCResourceHandleCURL();

    virtual KURL url() const { return m_request.url(); }
    virtual WebCore::String method() const { return m_request.httpMethod(); }
    virtual PassRefPtr<FormData> postData() const { return m_request.httpBody(); }
    // solve ambiguities of these 3 methods to BCResourceHandleCommonImplementation
    virtual ResourceHandleClient* client() const { return BCResourceHandleCommonImplementation::client(); }
    virtual void setClient(ResourceHandleClient* client) { BCResourceHandleCommonImplementation::setClient(client); }
    virtual const ResourceRequest& request() const { return BCResourceHandleCommonImplementation::request(); }
    virtual const HTTPHeaderMap& requestHeaders() const { return m_request.httpHeaderFields(); }

    CURL* handle() { return m_handle; }
    void setHandle( CURL* a ) { m_handle = a; }
    const char* getURL() { return m_url; }
    void setURL(const char* url);
    void setHeaders(struct curl_slist* headers) { m_customHeaders = headers; }

    bool loadsBlocked();
    void clearAuthentication();
    void cancel();
    PassRefPtr<WebCore::SharedBuffer> bufferedData();
    void setDefersLoading(bool defers);

    void  setupPUT();
    void  setupPOST();
    void  processMessage(CURLMsg* msg);
    size_t write(void* ptr, size_t size, size_t nmemb);
    size_t header(char* ptr, size_t size, size_t nmemb);
    
    /**
     * Link cookie manager to an observer.
     * @param(in) the topic to register
     * @param(in) the cookie
     */
    void observe(const String&, const String&);
    
private:
    void  finish();
    Vector<char> runImmediately(ResourceResponse& response);
    
    /**
     * Send the cookies we have related to the domain we're connected to
     * Uses CURL
     */
    void checkAndSendCookies(KURL url = "");

protected:
    CURL* m_handle;
    char* m_url;
    struct curl_slist* m_customHeaders;
    /**
     * execute the full request on callback
     * don't multiplex
     */
     bool m_useSimple;
     /**
      * Stores entire response if job is m_useSimple
      */
     Vector<char> m_resultData;
     struct curl_slist* m_customHeader;
     struct curl_slist* m_customPostHeader;
     struct curl_httppost* m_filePost;
     Vector<char> m_postBytes;
     Vector<CString> m_cstrings;
     ResourceResponse m_response;
     ResourceError m_error;
     bool m_sentResponse;
     bool m_loading;

};

}

#endif // BCResourceHandleCURL_H_
