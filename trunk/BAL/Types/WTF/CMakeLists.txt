if(USE_THREADS_GTHREADS)
    create_include_link(${BAL_DIR}/Types/WTF/Gtk BAL/wtf)
    aux_source_directory(${BAL_DIR}/Types/WTF/Gtk WTF_SRC)
endif(USE_THREADS_GTHREADS)

if(USE_THREADS_NONE)
    create_include_link(${BAL_DIR}/Types/WTF/None BAL/wtf)
    aux_source_directory(${BAL_DIR}/Types/WTF/None WTF_SRC)
endif(USE_THREADS_NONE)

if(USE_THREADS_PTHREADS)
    create_include_link(${BAL_DIR}/Types/WTF/Pthreads BAL/wtf)
    aux_source_directory(${BAL_DIR}/Types/WTF/Pthreads WTF_SRC)
endif(USE_THREADS_PTHREADS)

if(USE_THREADS_QT)
    create_include_link(${BAL_DIR}/Types/WTF/Qt BAL/wtf)
    owb_generate_moc(WTF_SRC ${OWB_SOURCE_DIR}/BAL/Types/WTF/Qt/BCMainThreadQt.cpp MainThreadQt.moc)
    aux_source_directory(${BAL_DIR}/Types/WTF/Qt WTF_SRC)
endif(USE_THREADS_QT)


create_include_link(${BAL_DIR}/Types/WTF BAL/wtf)

aux_source_directory(${BAL_DIR}/Types/WTF WTF_SRC)

if(NOT ENABLE_DEBUG)
    list(REMOVE_ITEM WTF_SRC
        ${BAL_DIR}/Types/WTF/BCdtoaWTF.cpp
    )
endif(NOT ENABLE_DEBUG)

