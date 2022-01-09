cmake_minimum_required(VERSION 3.20.0)
project(HelloWorld VERSION 1.0.1 DESCRIPTION "OpenFiles JNI")

find_package(Java REQUIRED)
include(UseJava)

add_jar(HelloWorld HelloWorld.java)

add_test(NAME javabuild COMMAND ${Java_JAVA_EXECUTABLE} -cp ${HelloWorld_BINARY_DIR}/HelloWorld.jar HelloWorld)
