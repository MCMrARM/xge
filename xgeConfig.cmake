add_library(xge STATIC IMPORTED)

find_package(GLEW REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)

find_library(XGE_LIBRARY "xge" HINTS "${CMAKE_CURRENT_LIST_DIR}/../../")
set_target_properties(xge PROPERTIES IMPORTED_LOCATION "${XGE_LIBRARY}")

set(XGE_LIBRARIES xge ${GLFW_STATIC_LIBRARIES} ${GLEW_LIBRARIES})