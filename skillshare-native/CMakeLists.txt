cmake_minimum_required(VERSION 3.20)
project(SkillshareSync VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows-specific settings
if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE ON)
endif()

# Include FetchContent
include(FetchContent)

# Download Windows Implementation Library (WIL)
FetchContent_Declare(
    wil
    GIT_REPOSITORY https://github.com/microsoft/wil.git
    GIT_TAG v1.0.231216.1
)
FetchContent_MakeAvailable(wil)

# Download WebView2 SDK
FetchContent_Declare(
    webview2
    URL https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/1.0.2210.55
    DOWNLOAD_NAME webview2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(webview2)

# Find WebView2 headers and libraries
set(WEBVIEW2_INCLUDE_DIR ${webview2_SOURCE_DIR}/build/native/include)
set(WEBVIEW2_LIB_DIR ${webview2_SOURCE_DIR}/build/native/x64)

# Source files
set(SOURCES
    src/main.cpp
    src/app_window.cpp
    src/webview_handler.cpp
    src/downloader.cpp
)

set(HEADERS
    include/app_window.h
    include/webview_handler.h
    include/downloader.h
)

# Create executable
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${WEBVIEW2_INCLUDE_DIR}
    ${wil_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${WEBVIEW2_LIB_DIR}/WebView2LoaderStatic.lib
    shlwapi
    shell32
    user32
    ole32
    oleaut32
)

# Copy WebView2 loader DLL to output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${WEBVIEW2_LIB_DIR}/WebView2Loader.dll"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

# Copy inject.js to output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/resources/inject.js"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

# Installation
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES "${WEBVIEW2_LIB_DIR}/WebView2Loader.dll" DESTINATION bin)
install(FILES "${CMAKE_SOURCE_DIR}/resources/inject.js" DESTINATION bin)
