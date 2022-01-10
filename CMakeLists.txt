cmake_minimum_required(VERSION 3.20.0)
project(of_core_jni VERSION 1.0.1 DESCRIPTION "OpenFiles JNI")

find_package(JNI REQUIRED)

if (JNI_FOUND)
  message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
  message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
endif()

include_directories(
	${JNI_INCLUDE_DIRS}
	${of_core_BINARY_DIR}
        ${of_core_SOURCE_DIR}/include
	include
)

set(SRCS
	src/com_connectedway_io_Filesystem.c
	src/com_connectedway_io_Framework.c
	src/com_connectedway_io_Utils.c
        )

add_library(of_core_jni SHARED ${SRCS})

set_target_properties(of_core_jni PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(of_core_jni PROPERTIES SOVERSION 1)

target_link_libraries(of_core_jni PRIVATE of_core_static ${JNI_LIBRARIES})

add_subdirectory(java)


add_subdirectory(test)