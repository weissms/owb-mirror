##################################################
# Network support part.                          #
##################################################

create_include_link(${BAL_DIR}/Network/WebCore/WK BAL)

if(USE_NETWORK_CURL)
    create_include_link(${BAL_DIR}/Network/WebCore/Curl BAL)

    aux_source_directory(${BAL_DIR}/Network/WebCore/Curl WEBCORE_SRC)
endif(USE_NETWORK_CURL)

if(USE_NETWORK_QT)
    create_include_link(${BAL_DIR}/Network/WebCore/Qt BAL)

    aux_source_directory(${BAL_DIR}/Network/WebCore/Qt WEBCORE_SRC)
    owb_generate_moc(WEBCORE_SRC
        ${OWB_SOURCE_DIR}/BAL/Network/WebCore/Qt/BCQNetworkReplyHandlerQt.h
        moc_QNetworkReplyHandler.cpp
    )
endif(USE_NETWORK_QT)

if(USE_NETWORK_SOUP)
    create_include_link(${BAL_DIR}/Network/WebCore/Soup BAL)

    aux_source_directory(${BAL_DIR}/Network/WebCore/Soup WEBCORE_SRC)
endif(USE_NETWORK_SOUP)
aux_source_directory(${BAL_DIR}/Network/WebCore/WK WEBCORE_SRC)
