﻿cmake_minimum_required (VERSION 3.8)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory("${PROJECT_SOURCE_DIR}/extern/googletest" "extern/googletest")

mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

set_target_properties(gtest PROPERTIES FOLDER extern)
set_target_properties(gtest_main PROPERTIES FOLDER extern)
set_target_properties(gmock PROPERTIES FOLDER extern)
set_target_properties(gmock_main PROPERTIES FOLDER extern)

macro(package_add_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})
    target_link_libraries(${TESTNAME} gtest gmock gtest_main ${PROJECT_NAME})
    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()

package_add_test(flat_set_tests src/flat_set.cpp)
package_add_test(flat_map_tests src/flat_map.cpp)
package_add_test(flat_multiset_tests src/flat_multiset.cpp)
package_add_test(flat_multimap_tests src/flat_multimap.cpp)
package_add_test(heap_tests src/heap.cpp)
package_add_test(sparse_set_tests src/sparse_set.cpp)
package_add_test(deque_tests src/deque.cpp)
package_add_test(partition_tests src/partition.cpp)
package_add_test(bitset_tests src/bitset.cpp)