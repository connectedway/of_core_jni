cmake_minimum_required(VERSION 3.20.0)
project(JavaOpenFiles VERSION 1.0.1 DESCRIPTION "OpenFiles Classes")

find_package(Java REQUIRED)
include(UseJava)

add_jar(JavaOpenFiles
	com/connectedway/io/FileDescriptor.java
	com/connectedway/io/FileSystem.java
	com/connectedway/io/FileOutputStream.java
	com/connectedway/io/FileInputStream.java
	com/connectedway/io/FileWriter.java
	com/connectedway/io/Framework.java
	com/connectedway/io/RandomAccessFile.java
	com/connectedway/io/File.java
	)

