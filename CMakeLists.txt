cmake_minimum_required(VERSION 3.20.0)
project(of_core_jni VERSION 1.0.1 DESCRIPTION "OpenFiles JNI")

if (APPLE)
  set(ENV{JAVA_HOME} /Library/Java/JavaVirtualMachines/openlogic-openjdk-11.jdk/Contents/Home)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Android")
  set(JAVA_AWT_LIBRARY c)
  set(JAVA_JVM_LIBRARY c)
  set(JAVA_INCLUDE_PATH2 c)
  set(JAVA_AWT_INCLUDE_PATH c)
endif()

find_package(JNI REQUIRED)

if (JNI_FOUND)
  message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
  message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
endif()

include_directories(
	${JNI_INCLUDE_DIRS}
	${of_core_BINARY_DIR}
        ${of_core_SOURCE_DIR}/include
	${of_resolver_fs_SOURCE_DIR}/include
	include
)

if(ANDROID)
	unset(ANDROID_SOURCES)
	set(ANDROID_SOURCES
	    src/com_connectedway_io_Resolver.c
	    )
endif()

set(SRCS
	src/com_connectedway_io_Filesystem.c
	src/com_connectedway_io_Framework.c
	src/com_connectedway_io_Utils.c
	${ANDROID_SOURCES}
        )

add_library(of_core_jni SHARED ${SRCS})

set_target_properties(of_core_jni PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(of_core_jni PROPERTIES SOVERSION 1)

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
   set(additional_libs android)
endif()

target_link_libraries(of_core_jni PRIVATE of_core_static ${JNI_LIBRARIES}
     ${additional_libs})

add_subdirectory(java)

add_subdirectory(test)
