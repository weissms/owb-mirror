##################################################
# Network support part.                          #
##################################################

create_include_link(${BAL_DIR}/Network/WebCore/WK/BCAuthenticationChallengeBaseWK.h BAL/AuthenticationChallengeBase.h)
create_include_link(${BAL_DIR}/Network/WebCore/WK/BCCredentialWK.h BAL/Credential.h)
create_include_link(${BAL_DIR}/Network/WebCore/WK/BCCookieJarWK.h BAL/CookieJar.h)
create_include_link(${BAL_DIR}/Network/WebCore/WK/BCDNSWK.h BAL/DNS.h)
create_include_link(${BAL_DIR}/Network/WebCore/WK/BCFormDataWK.h BAL/FormData.h)
create_include_link(${BAL_DIR}/Network/WebCore/WK/BCFormDataBuilderWK.h BAL/FormDataBuilder.h)
create_include_link(${BAL_DIR}/Network/WebCore/WK/BCNetworkStateNotifierWK.h BAL/NetworkStateNotifier.h)
create_include_link(${BAL_DIR}/Network/WebCore/WK/BCProtectionSpaceWK.h BAL/ProtectionSpace.h)
create_include_link(${BAL_DIR}/Network/WebCore/WK/BCResourceErrorBaseWK.h BAL/ResourceErrorBase.h)
create_include_link(${BAL_DIR}/Network/WebCore/WK/BCResourceHandleClientWK.h BAL/ResourceHandleClient.h)
create_include_link(${BAL_DIR}/Network/WebCore/WK/BCResourceHandleInternalWK.h BAL/ResourceHandleInternal.h)
create_include_link(${BAL_DIR}/Network/WebCore/WK/BCResourceHandleWK.h BAL/ResourceHandle.h)
create_include_link(${BAL_DIR}/Network/WebCore/WK/BCResourceRequestBaseWK.h BAL/ResourceRequestBase.h)
create_include_link(${BAL_DIR}/Network/WebCore/WK/BCResourceResponseBaseWK.h BAL/ResourceResponseBase.h)


if(USE_NETWORK_CURL)
    create_include_link(${BAL_DIR}/Network/WebCore/Curl/BCAuthenticationChallengeCurl.h BAL/AuthenticationChallenge.h)
    create_include_link(${BAL_DIR}/Network/WebCore/Curl/BCCookieCurl.h BAL/Cookie.h)
    create_include_link(${BAL_DIR}/Network/WebCore/Curl/BCCookieMapCurl.h BAL/CookieMap.h)
    create_include_link(${BAL_DIR}/Network/WebCore/Curl/BCCookieManagerCurl.h BAL/CookieManager.h)
    create_include_link(${BAL_DIR}/Network/WebCore/Curl/BCResourceErrorCurl.h BAL/ResourceError.h)
    create_include_link(${BAL_DIR}/Network/WebCore/Curl/BCResourceHandleManagerCurl.h BAL/ResourceHandleManager.h)
    create_include_link(${BAL_DIR}/Network/WebCore/Curl/BCResourceRequestCurl.h BAL/ResourceRequest.h)
    create_include_link(${BAL_DIR}/Network/WebCore/Curl/BCResourceResponseCurl.h BAL/ResourceResponse.h)
    create_include_link(${BAL_DIR}/Network/WebCore/Curl/BCFormDataStreamCurl.h BAL/FormDataStreamCurl.h)

    aux_source_directory(${BAL_DIR}/Network/WebCore/Curl WEBCORE_SRC)
endif(USE_NETWORK_CURL)

if(USE_NETWORK_QT)
    create_include_link(${BAL_DIR}/Network/WebCore/Qt/BCAuthenticationChallengeQt.h BAL/AuthenticationChallenge.h)
    create_include_link(${BAL_DIR}/Network/WebCore/Qt/BCQNetworkReplyHandlerQt.h BAL/QNetworkReplyHandler.h)
    create_include_link(${BAL_DIR}/Network/WebCore/Qt/BCResourceErrorQt.h BAL/ResourceError.h)
    create_include_link(${BAL_DIR}/Network/WebCore/Qt/BCResourceRequestQt.h BAL/ResourceRequest.h)
    create_include_link(${BAL_DIR}/Network/WebCore/Qt/BCResourceResponseQt.h BAL/ResourceResponse.h)

    aux_source_directory(${BAL_DIR}/Network/WebCore/Qt WEBCORE_SRC)
    owb_generate_moc(WEBCORE_SRC
        ${OWB_SOURCE_DIR}/BAL/Network/WebCore/Qt/BCQNetworkReplyHandlerQt.h
        moc_QNetworkReplyHandler.cpp
    )
endif(USE_NETWORK_QT)

aux_source_directory(${BAL_DIR}/Network/WebCore/WK WEBCORE_SRC)
