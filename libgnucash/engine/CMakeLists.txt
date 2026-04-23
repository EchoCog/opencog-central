# GnuCash Engine Library - Multi-Entity Cognitive Extension
# Part of the CoGnuCash Unified Cognitive Branch
# Cherry-picked from rzonedevops/gnucashmulti PRs 1-15 (Sep 2025)

cmake_minimum_required(VERSION 3.14)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB2 REQUIRED glib-2.0)

# Library sources
set(ENGINE_SOURCES
    gncOwner.cpp
)

# Library headers
set(ENGINE_HEADERS
    gncOwner.h
)

# Create the engine library
add_library(gnucash-engine STATIC ${ENGINE_SOURCES})

target_include_directories(gnucash-engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${GLIB2_INCLUDE_DIRS}
)

target_link_libraries(gnucash-engine
    PUBLIC
        ${GLIB2_LIBRARIES}
)

target_compile_features(gnucash-engine PUBLIC cxx_std_17)

# Install headers
install(FILES ${ENGINE_HEADERS}
    DESTINATION include/gnucash/engine
)

# Install library
install(TARGETS gnucash-engine
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Tests
option(BUILD_TESTING "Build the testing tree." ON)

if(BUILD_TESTING)
    find_package(GTest QUIET)
    
    if(GTest_FOUND)
        enable_testing()
        
        add_executable(test-qof-multi-entity
            gtest-qof-multi-entity.cpp
            ${ENGINE_SOURCES}
        )
        
        target_include_directories(test-qof-multi-entity
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}
                ${GLIB2_INCLUDE_DIRS}
        )
        
        target_link_libraries(test-qof-multi-entity
            PRIVATE
                GTest::gtest
                GTest::gtest_main
                ${GLIB2_LIBRARIES}
        )
        
        target_compile_features(test-qof-multi-entity PRIVATE cxx_std_17)
        
        add_test(NAME QofMultiEntityTests COMMAND test-qof-multi-entity)
        
        # Set test properties
        set_tests_properties(QofMultiEntityTests PROPERTIES
            TIMEOUT 30
            LABELS "unit;qof;multi-entity"
        )
        
        # E2E Test Infrastructure (cherry-picked from gnucashmulti PR 3)
        # Source commits: 16eb2e3797 through 9c0cc98ec9
        
        # Organization unit tests (46 test cases)
        add_executable(test-organization
            test/gtest-organization.cpp
            ${ENGINE_SOURCES}
        )
        
        target_include_directories(test-organization
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}
                ${GLIB2_INCLUDE_DIRS}
        )
        
        target_link_libraries(test-organization
            PRIVATE
                GTest::gtest
                GTest::gtest_main
                ${GLIB2_LIBRARIES}
        )
        
        target_compile_features(test-organization PRIVATE cxx_std_17)
        
        add_test(NAME OrganizationTests COMMAND test-organization)
        set_tests_properties(OrganizationTests PROPERTIES
            TIMEOUT 60
            LABELS "unit;organization;multi-entity"
        )
        
        # Multi-entity collection tests (38 test cases)
        add_executable(test-qof-multi-entity-e2e
            test/gtest-qof-multi-entity.cpp
            ${ENGINE_SOURCES}
        )
        
        target_include_directories(test-qof-multi-entity-e2e
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}
                ${GLIB2_INCLUDE_DIRS}
        )
        
        target_link_libraries(test-qof-multi-entity-e2e
            PRIVATE
                GTest::gtest
                GTest::gtest_main
                ${GLIB2_LIBRARIES}
        )
        
        target_compile_features(test-qof-multi-entity-e2e PRIVATE cxx_std_17)
        
        add_test(NAME QofMultiEntityE2ETests COMMAND test-qof-multi-entity-e2e)
        set_tests_properties(QofMultiEntityE2ETests PROPERTIES
            TIMEOUT 60
            LABELS "unit;qof;multi-entity;e2e"
        )
        
        # Edge case and stress tests (32 test cases)
        add_executable(test-multi-entity-edge-cases
            test/gtest-multi-entity-edge-cases.cpp
            ${ENGINE_SOURCES}
        )
        
        target_include_directories(test-multi-entity-edge-cases
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}
                ${GLIB2_INCLUDE_DIRS}
        )
        
        target_link_libraries(test-multi-entity-edge-cases
            PRIVATE
                GTest::gtest
                GTest::gtest_main
                ${GLIB2_LIBRARIES}
        )
        
        target_compile_features(test-multi-entity-edge-cases PRIVATE cxx_std_17)
        
        add_test(NAME MultiEntityEdgeCaseTests COMMAND test-multi-entity-edge-cases)
        set_tests_properties(MultiEntityEdgeCaseTests PROPERTIES
            TIMEOUT 120
            LABELS "unit;multi-entity;edge-cases;stress"
        )
    else()
        message(STATUS "GTest not found, tests will not be built")
    endif()
endif()

# Export configuration
export(TARGETS gnucash-engine
    FILE "${CMAKE_CURRENT_BINARY_DIR}/gnucash-engine-targets.cmake"
)
