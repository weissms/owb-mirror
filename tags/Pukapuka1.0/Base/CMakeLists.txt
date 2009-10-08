include(Base/Customer/CMakeLists.txt OPTIONAL)
configure_file(
    ${OWB_SOURCE_DIR}/Base/BALBase.h.in 
    ${OWB_SOURCE_DIR}/Base/BALBase.h
    @ONLY
)
configure_file(
    ${OWB_SOURCE_DIR}/Base/WebKitTypes.h.in 
    ${OWB_SOURCE_DIR}/Base/WebKitTypes.h
    @ONLY
)

