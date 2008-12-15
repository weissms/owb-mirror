include_directories(
    ${BALBASE_INCLUDE_DIRS}
    ${BAL_INCLUDE_DIRS}
)
set(BAL_DIR ../BAL)

include(${BAL_DIR}/Base/CMakeLists.txt)
include(${BAL_DIR}/Database/WebCore/CMakeLists.txt)
include(${BAL_DIR}/Events/WebCore/CMakeLists.txt)
include(${BAL_DIR}/Facilities/WebCore/CMakeLists.txt)
include(${BAL_DIR}/Fonts/WebCore/CMakeLists.txt)
include(${BAL_DIR}/Graphics/WebCore/CMakeLists.txt)
include(${BAL_DIR}/ImageDecoder/WebCore/CMakeLists.txt)
include(${BAL_DIR}/Internationalization/WebCore/CMakeLists.txt)
include(${BAL_DIR}/Media/WebCore/CMakeLists.txt)
include(${BAL_DIR}/Memory/WebCore/CMakeLists.txt)
include(${BAL_DIR}/Network/WebCore/CMakeLists.txt)
include(${BAL_DIR}/SVG/WebCore/CMakeLists.txt)
include(${BAL_DIR}/Types/WebCore/CMakeLists.txt)
include(${BAL_DIR}/Widgets/WebCore/CMakeLists.txt)

