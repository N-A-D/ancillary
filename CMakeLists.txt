﻿cmake_minimum_required (VERSION 3.8)

project (ancillary)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(
	${PROJECT_NAME}
	INTERFACE
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

option(BUILD_ANCILLARY_LIBRARY_TESTS "Build the ancillary library tests")
if (BUILD_ANCILLARY_LIBRARY_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()