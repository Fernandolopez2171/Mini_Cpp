cmake_minimum_required (VERSION 3.14)
project(FLexer)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(RE2C REQUIRED)

add_custom_command(
  OUTPUT ExprLexerImpl.cpp
  COMMAND ${RE2C} -o ExprLexerImpl.cpp ${PROJECT_SOURCE_DIR}/ExprLexer.re
  MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/ExprLexer.re
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME}  ExprLexer.hpp
                                ExprLexerImpl.cpp
                                ExprParser.hpp
                                ExprParser.cpp
                                main.cpp)
