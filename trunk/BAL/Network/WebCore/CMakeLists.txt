##################################################
# Network support part.                          #
##################################################

create_include_link(${BAL_DIR}/Network/WebCore/WK BAL)

if(USE_NETWORK_CURL)
    create_include_link(${BAL_DIR}/Network/WebCore/Curl BAL)
    aux_source_directory(${BAL_DIR}/Network/WebCore/Curl WEBCORE_SRC)

    # Choose the backing store for the cookies.
    if(ENABLE_DATABASE)
        create_include_link(${BAL_DIR}/Network/WebCore/Curl/CookieDatabaseBackingStore BAL)
        aux_source_directory(${BAL_DIR}/Network/WebCore/Curl/CookieDatabaseBackingStore WEBCORE_SRC)
    else(ENABLE_DATABASE)
        create_include_link(${BAL_DIR}/Network/WebCore/Curl/CookieEmptyBackingStore BAL)
        aux_source_directory(${BAL_DIR}/Network/WebCore/Curl/CookieEmptyBackingStore WEBCORE_SRC)
    endif(ENABLE_DATABASE)
endif(USE_NETWORK_CURL)

if(USE_NETWORK_QT)
    create_include_link(${BAL_DIR}/Network/WebCore/Qt BAL)

    aux_source_directory(${BAL_DIR}/Network/WebCore/Qt WEBCORE_SRC)
    owb_generate_moc(WEBCORE_SRC
        ${OWB_SOURCE_DIR}/BAL/Network/WebCore/Qt/BCQNetworkReplyHandlerQt.h
        moc_QNetworkReplyHandler.cpp
    )

    if(NOT ENABLE_QT_BEARER)
        list(REMOVE_ITEM WEBCORE_SRC
            ${BAL_DIR}/Network/WebCore/Qt/BCNetworkStateNotifierQt.cpp
        )
    endif(NOT ENABLE_QT_BEARER)

endif(USE_NETWORK_QT)

if(USE_NETWORK_SOUP)
    create_include_link(${BAL_DIR}/Network/WebCore/Soup BAL)

    aux_source_directory(${BAL_DIR}/Network/WebCore/Soup WEBCORE_SRC)
endif(USE_NETWORK_SOUP)
aux_source_directory(${BAL_DIR}/Network/WebCore/WK WEBCORE_SRC)
