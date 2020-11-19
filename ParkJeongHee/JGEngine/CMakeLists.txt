cmake_minimum_required(VERSION 3.15.2)

project(JGEngine)
set (CMAKE_BUILD_TYPE Debug)
set(JGENGINE_BIN_DIR ${PROJECT_SOURCE_DIR}/Bin)
set(JGENGINE_LIB_DIR ${JGENGINE_BIN_DIR}/lib)
set(JGENGINE_EXE_DIR ${JGENGINE_BIN_DIR}/exe)


set_property(GLOBAL PROPERTY USE_FOLDERS on)



set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${JGENGINE_BIN_DIR})
set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${JGENGINE_LIB_DIR})
set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${JGENGINE_LIB_DIR})











# JGCore
# static 라이브러리 생성
set(JGCORE_SOURCE_DIRECTROY Source/JGEngine/JGCore)
file(GLOB JGCore_SourceFile 
    ${JGCORE_SOURCE_DIRECTROY}/**.cpp 
    ${JGCORE_SOURCE_DIRECTROY}/**.h)
add_library(JGCore STATIC ${JGCore_SourceFile})

set_target_properties(JGCore PROPERTIES FOLDER JGEngine)
target_include_directories(JGCore PRIVATE ${JGCORE_SOURCE_DIRECTROY})






# UnitTest
# ConsoleTest
set(UNITTEST_CONSOLETEST_SOURCE_DIRECTORY Source/UnitTest/ConsoleTest)
file(GLOB UnitTest_ConsoleTest_SourceFile 
    ${UNITTEST_CONSOLETEST_SOURCE_DIRECTORY}/**.h
    ${UNITTEST_CONSOLETEST_SOURCE_DIRECTORY}/**.cpp)
add_executable(ConsoleTest ${UnitTest_ConsoleTest_SourceFile})
set_target_properties(ConsoleTest PROPERTIES FOLDER UnitTest)
target_include_directories(ConsoleTest PRIVATE ${JGCORE_SOURCE_DIRECTROY})
