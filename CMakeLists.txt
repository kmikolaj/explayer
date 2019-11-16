cmake_minimum_required(VERSION 3.7)

set(PACKAGE_NAME "ExPlayer")
set(PACKAGE_VERSION "0.1")

project(${PACKAGE_NAME} CXX)

# set bin directories
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)

# include src directory
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/ui)

# simplify Qt5 development
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

# generate meta-information for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find Qt5 and required components
find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets X11Extras)

# find pkg-config
find_package(PkgConfig REQUIRED)

# use pkg-config to configure dependencies
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(GSTREAMER-AUDIO REQUIRED gstreamer-audio-1.0)
pkg_check_modules(GSTREAMER-VIDEO REQUIRED gstreamer-video-1.0)
pkg_check_modules(GSTREAMER-PBUTILS REQUIRED gstreamer-pbutils-1.0)
pkg_check_modules(XCB-DPMS REQUIRED xcb-dpms)

# include dependencies
include_directories(
	SYSTEM ${GSTREAMER_INCLUDE_DIRS}
	SYSTEM ${GSTREAMER-AUDIO_INCLUDE_DIRS}
	SYSTEM ${GSTREAMER-VIDEO_INCLUDE_DIRS}
	SYSTEM ${GSTREAMER-PBUTILS_INCLUDE_DIRS}
	SYSTEM ${XCB-DPMS_INCLUDE_DIRS}
)

# link dependencies
link_directories(
	${GSTREAMER_LIBRARY_DIRS}
	${GSTREAMER-AUDIO_LIBRARY_DIRS}
	${GSTREAMER-VIDEO_LIBRARY_DIRS}
	${GSTREAMER-PBUTILS_LIBRARY_DIRS}
	${XCB-DPMS_LIBRARY_DIRS}
)

# compile options
add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-Wpedantic)
add_compile_options(-Wshadow)
add_compile_options(-Wcast-qual)
add_compile_options(-Wnon-virtual-dtor)
add_compile_options(-Wformat=2)
add_compile_options(-std=c++11)

# release build compile options
add_compile_options(
	$<$<CONFIG:RELEASE>:-Wno-deprecated-declarations>
	$<$<CONFIG:RELEASE>:-Wno-unused-parameter>
)

# debug build compile options
add_compile_options(
	$<$<CONFIG:DEBUG>:-Werror>
)

# source files
set(SOURCES
	src/ui/mainwindow.cpp
	src/ui/jumpdialog.cpp
	src/ui/controls.cpp
	src/main.cpp
	src/settings.cpp
	src/metadata.cpp
	src/subtitleeditor.cpp
	src/utils.cpp
	src/player.cpp
	src/dpms.cpp
	src/utime.cpp
	src/videowidget.cpp
	src/gstreamer/discoverer.cpp
	src/gstreamer/balance.cpp
	src/gstreamer/gstreamer.cpp
	src/gstreamer/videofilter.cpp
	src/gstreamer/osd.cpp
)

# header files
set(HEADERS
	src/ui/mainwindow.h
	src/ui/controls.h
	src/ui/jumpdialog.h
	src/settings.h
	src/subtitleeditor.h
	src/metadata.h
	src/version.h
	src/utils.h
	src/player.h
	src/dpms.h
	src/utime.h
	src/videowidget.h
	src/gstreamer/discoverer.h
	src/gstreamer/balance.h
	src/gstreamer/gstreamer.h
	src/gstreamer/videofilter.h
	src/gstreamer/osd.h
)

# ui files
set(FORMS
	src/ui/mainwindow.ui
	src/ui/controls.ui
	src/ui/jumpdialog.ui
)

add_executable(ExPlayer
	${SOURCES}
	${HEADERS}
	${FORMS}
)

target_link_libraries(ExPlayer
	PUBLIC
	Qt5::Core
	Qt5::Gui
	Qt5::Widgets
	Qt5::X11Extras
	${GSTREAMER_LIBRARIES}
	${GSTREAMER-AUDIO_LIBRARIES}
	${GSTREAMER-VIDEO_LIBRARIES}
	${GSTREAMER-PBUTILS_LIBRARIES}
	${XCB-DPMS_LIBRARIES}
)


