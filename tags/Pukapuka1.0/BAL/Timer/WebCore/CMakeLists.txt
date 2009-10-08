##################################################
# Timer support part.                            #
##################################################

create_include_link(${BAL_DIR}/Timer/WebCore/WK BAL)
aux_source_directory(${BAL_DIR}/Timer/WebCore/WK WEBCORE_SRC)

if(USE_TIMER_GLIB)
    aux_source_directory(${BAL_DIR}/Timer/WebCore/Gtk WEBCORE_SRC)
endif(USE_TIMER_GLIB)

if(USE_TIMER_LINUX)
    create_include_link(${BAL_DIR}/Timer/WebCore/Linux BAL)
    aux_source_directory(${BAL_DIR}/Timer/WebCore/Linux WEBCORE_SRC)
endif(USE_TIMER_LINUX)

if(USE_TIMER_QT)
    aux_source_directory(${BAL_DIR}/Timer/WebCore/Qt WEBCORE_SRC)
endif(USE_TIMER_QT)

