cmake_minimum_required(VERSION 3.20.0)
project(jni_test VERSION 1.0.1 DESCRIPTION "OpenFiles JNI Tests")

find_package(Java REQUIRED)
include(UseJava)

add_jar(OfcExplorer
	SOURCES OfcExplorer.java
	INCLUDE_JARS ${JavaOpenFiles_BINARY_DIR}/JavaOpenFiles.jar
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(OF_CLASSPATH "${jni_test_BINARY_DIR}/OfcExplorer.jar\;${JavaOpenFiles_BINARY_DIR}/JavaOpenFiles.jar")
else()
  set(OF_CLASSPATH "${jni_test_BINARY_DIR}/OfcExplorer.jar:${JavaOpenFiles_BINARY_DIR}/JavaOpenFiles.jar")
endif()

add_test(NAME ofc_explorer COMMAND ${Java_JAVA_EXECUTABLE} -Djava.library.path=${of_core_jni_BINARY_DIR} -cp ${OF_CLASSPATH} OfcExplorer ${openfiles_SOURCE_DIR}/configs/java_debug.xml)


