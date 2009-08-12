if(ENABLE_NOTIFICATIONS)
    list(APPEND WEBCORE_SRC
        notifications/Notification.cpp
        notifications/NotificationCenter.cpp
    )
    list(APPEND IDL_SRC
        notifications/Notification.idl
        notifications/NotificationCenter.idl
    )
endif(ENABLE_NOTIFICATIONS)

