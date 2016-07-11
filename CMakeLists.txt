cmake_minimum_required(VERSION 3.5)
project(xge)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Threads REQUIRED)
find_package(PNG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)

include_directories(core/)
include_directories(engine/)
include_directories(platform/linux)

file(GLOB_RECURSE CORE_SOURCES "core/*.cpp")
file(GLOB_RECURSE CORE_HEADERS "core/*.h")
file(GLOB_RECURSE ENGINE_SOURCES "engine/*.cpp")
file(GLOB_RECURSE ENGINE_HEADERS "engine/*.h")
file(GLOB_RECURSE LINUX_SOURCES "platform/linux/*.cpp")
file(GLOB_RECURSE LINUX_HEADERS "platform/linux/*.h")
LIST(APPEND CORE_SOURCES ${CORE_HEADERS})
LIST(APPEND ENGINE_SOURCES ${ENGINE_HEADERS})
LIST(APPEND LINUX_SOURCES ${LINUX_HEADERS})

add_library(xge STATIC ${CORE_SOURCES} ${ENGINE_SOURCES} ${LINUX_SOURCES})
target_link_libraries(xge ${CMAKE_THREAD_LIBS_INIT} ${OPENGL_LIBRARIES} ${PNG_LIBRARIES} ${GLFW_STATIC_LIBRARIES} ${GLEW_LIBRARIES})
target_include_directories(xge PUBLIC ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

install(DIRECTORY engine/xge DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY core/xge DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY platform/linux/xge DESTINATION include FILES_MATCHING PATTERN "*.h")
install(TARGETS xge DESTINATION lib)
install(FILES xgeConfig.cmake DESTINATION lib/cmake/xge)
