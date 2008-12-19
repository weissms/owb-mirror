if(USE_I18N STREQUAL "ICU")
    find_package(ICU REQUIRED)
    if(NOT ICU_FOUND)
        message(FATAL_ERROR "icu package not found. Install it to be able to compile owb.")
    endif(NOT ICU_FOUND)
    set(I18N_INCLUDE_DIRS ${ICU_INCLUDE})
    set(I18N_LIBRARIES ${ICU_LIBRARY})

    set(USE_I18N_ICU TRUE)
    mark_as_advanced(USE_I18N_ICU)
endif(USE_I18N STREQUAL "ICU")

if(USE_I18N STREQUAL "GENERIC")
    set(I18N_INCLUDE_DIRS ${OWB_SOURCE_DIR}/JavaScriptCore/icu)

    set(USE_I18N_GENERIC TRUE)
    mark_as_advanced(USE_I18N_GENERIC)
endif(USE_I18N STREQUAL "GENERIC")
