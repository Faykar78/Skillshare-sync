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

# Download Windows Implementation Library (WIL) - header only library
FetchContent_Declare(
    wil
    URL https://github.com/microsoft/wil/archive/refs/tags/v1.0.231216.1.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_GetProperties(wil)
if(NOT wil_POPULATED)
    FetchContent_Populate(wil)
endif()

# Download WebView2 SDK
FetchContent_Declare(
    webview2
    URL https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/1.0.2210.55
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_GetProperties(webview2)
if(NOT webview2_POPULATED)
    FetchContent_Populate(webview2)
endif()

# Find WIL include directory (handles nested folder from zip)
if(EXISTS "${wil_SOURCE_DIR}/include")
    set(WIL_INCLUDE_DIR ${wil_SOURCE_DIR}/include)
else()
    # Handle the zip structure: wil-X.X.X/include
    file(GLOB WIL_SUBDIRS "${wil_SOURCE_DIR}/wil-*")
    if(WIL_SUBDIRS)
        list(GET WIL_SUBDIRS 0 WIL_SUBDIR)
        set(WIL_INCLUDE_DIR ${WIL_SUBDIR}/include)
    else()
        set(WIL_INCLUDE_DIR ${wil_SOURCE_DIR}/include)
    endif()
endif()

# Find WebView2 headers and libraries
set(WEBVIEW2_INCLUDE_DIR ${webview2_SOURCE_DIR}/build/native/include)
set(WEBVIEW2_LIB_DIR ${webview2_SOURCE_DIR}/build/native/x64)

# Print debug info
message(STATUS "WIL include dir: ${WIL_INCLUDE_DIR}")
message(STATUS "WebView2 include dir: ${WEBVIEW2_INCLUDE_DIR}")
message(STATUS "WebView2 lib dir: ${WEBVIEW2_LIB_DIR}")

# Verify paths exist
if(NOT EXISTS "${WIL_INCLUDE_DIR}/wil")
    message(WARNING "WIL headers not found at ${WIL_INCLUDE_DIR}")
endif()
if(NOT EXISTS "${WEBVIEW2_INCLUDE_DIR}/WebView2.h")
    message(WARNING "WebView2 headers not found at ${WEBVIEW2_INCLUDE_DIR}")
endif()

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
    ${WIL_INCLUDE_DIR}
)

# Link libraries (MSVC only)
if(MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${WEBVIEW2_LIB_DIR}/WebView2LoaderStatic.lib
        shlwapi
        shell32
        user32
        ole32
        oleaut32
    )
endif()

# Copy WebView2 loader DLL
if(EXISTS "${WEBVIEW2_LIB_DIR}/WebView2Loader.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WEBVIEW2_LIB_DIR}/WebView2Loader.dll"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
endif()

# Copy inject.js
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/resources/inject.js"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

# Installation
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES "${CMAKE_SOURCE_DIR}/resources/inject.js" DESTINATION bin)
