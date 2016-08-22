cmake_minimum_required(VERSION 3.3)
project(xge)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Threads REQUIRED)
find_package(PNG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig)
find_package(Freetype REQUIRED)

find_path(GLFW_INCLUDE_DIRS GLFW/glfw3.h)
find_library(GLFW_LIBRARIES NAMES GLFW glfw3 PATH_SUFFIXES lib64)
IF (NOT GLFW_LIBRARIES)
    pkg_search_module(GLFW REQUIRED glfw3)
ENDIF()

include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(core/)
include_directories(engine/)
include_directories(platform/desktop)

file(GLOB_RECURSE CORE_SOURCES "core/*.cpp")
file(GLOB_RECURSE CORE_HEADERS "core/*.h")
file(GLOB_RECURSE ENGINE_SOURCES "engine/*.cpp")
file(GLOB_RECURSE ENGINE_HEADERS "engine/*.h")
file(GLOB_RECURSE DESKTOP_SOURCES "platform/desktop/*.cpp")
file(GLOB_RECURSE DESKTOP_HEADERS "platform/desktop/*.h")
LIST(APPEND CORE_SOURCES ${CORE_HEADERS})
LIST(APPEND ENGINE_SOURCES ${ENGINE_HEADERS})
LIST(APPEND DESKTOP_SOURCES ${DESKTOP_HEADERS})

add_library(xge STATIC ${CORE_SOURCES} ${ENGINE_SOURCES} ${DESKTOP_SOURCES})
target_link_libraries(xge ${CMAKE_THREAD_LIBS_INIT} ${OPENGL_LIBRARIES} ${PNG_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${FREETYPE_LIBRARIES})
target_include_directories(xge PUBLIC ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS})

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

install(DIRECTORY engine/xge DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY core/xge DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY platform/desktop/xge DESTINATION include FILES_MATCHING PATTERN "*.h")
install(TARGETS xge DESTINATION lib)
install(FILES xgeConfig.cmake DESTINATION lib/cmake/xge)
