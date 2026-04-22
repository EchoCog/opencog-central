# Multi-Entity and Organization Test Suite

## Overview

This directory contains comprehensive end-to-end unit tests for the GnuCash
multi-entity aggregation and organization management features, ported from
[rzonedevops/gnucashmulti](https://github.com/rzonedevops/gnucashmulti) PR 3
(Feb 2026) into the CoGnuCash Unified Cognitive Branch.

**Source commits**: `16eb2e3797` through `9c0cc98ec9`

The tests have been adapted for the `opencog-central` self-contained
`gncOwner.h` API, which consolidates the GncOrganization and Owner types
into a single header.

## Test Files

### 1. gtest-organization.cpp
**46 test cases** covering the `GncOrganization` entity type.

**Test Categories:**
- Creation and destruction lifecycle
- Property getters and setters (ID, Name, Notes, Currency, Active, Address)
- Entity management (add, remove, count, list)
- Null pointer safety
- String edge cases (empty, long, special chars, Unicode)
- Organization hierarchy (parent/child)
- Tensor operations
- Edit state management (begin/commit/rollback)
- QOF integration (GUID uniqueness)
- Large-scale operations (1000+ entities)
- Complete lifecycle tests

**Key Tests:**
- `CreateOrganization` — Basic creation with defaults
- `CompleteLifecycle` — Full property + entity workflow
- `AddManyEntities` — Scale test with 1000 entities
- `MultipleOrganizationsInBook` — Multiple orgs with unique GUIDs
- `SetUnicodeStrings` — Unicode support validation

### 2. gtest-qof-multi-entity.cpp
**38 test cases** for multi-entity collection functionality.

**Basic Entity Operations (13 tests):**
- Create, destroy, add, remove, count, contains
- Duplicate prevention
- Mixed type entities
- Null pointer handling
- Complete removal

**Organization Integration Tests (25 tests):**
- `CreateFromOrganization` — Build collection from org entities
- `FilterByOrganization` — Filter by org membership
- `ComplexOrganizationScenario` — 3 orgs with 15 entities
- `LargeOrganizationCollection` — 10,000 entity organization
- `MultipleOrganizationMergeStress` — 10 orgs × 100 entities
- Owner type integration
- Hierarchy depth and sibling tests
- Tensor multi-entity operations
- Edit/commit/rollback integration
- Currency and active flag defaults

### 3. gtest-multi-entity-edge-cases.cpp
**32 test cases** for edge cases and stress testing.

**Test Categories:**
- Invalid GUID and entity state handling
- Empty collection operations
- Deep hierarchy (10-level chains)
- Circular reference prevention
- Tensor boundary conditions (zero, large, null)
- Owner type edge cases (all types, invalid types)
- Edit state edge cases (double begin, commit without begin)
- Large-scale stress tests (5000+ entities, rapid updates)
- Concurrent child operations

## How to Run

### Using CMake (from the build directory)

```bash
# Build all tests
mkdir -p build && cd build
cmake .. -DBUILD_TESTING=ON
make -j$(nproc)

# Run all multi-entity tests
ctest --test-dir . -R "MultiEntity|Organization"

# Run individual test suites
./test-organization
./test-qof-multi-entity
./test-multi-entity-edge-cases

# Run with verbose output
./test-organization --gtest_print_time=0
./test-qof-multi-entity --gtest_filter="QofMultiEntityTest.*"
```

### Running specific test categories

```bash
# Only organization creation tests
./test-organization --gtest_filter="GncOrganizationTest.Create*"

# Only edge case tests
./test-multi-entity-edge-cases --gtest_filter="MultiEntityEdgeCaseTest.*"

# Only stress tests
./test-multi-entity-edge-cases --gtest_filter="*Stress*"
```

## Test Coverage Summary

| Test File | Tests | Lines | Coverage Area |
|-----------|-------|-------|---------------|
| gtest-organization.cpp | 46 | ~500 | GncOrganization CRUD, hierarchy, tensors |
| gtest-qof-multi-entity.cpp | 38 | ~550 | Multi-entity collections, Owner integration |
| gtest-multi-entity-edge-cases.cpp | 32 | ~470 | Edge cases, null safety, stress tests |
| **Total** | **116** | **~1520** | |

## Porting Notes

The original gnucashmulti tests reference the full GnuCash build system
(`config.h`, `qof.h`, `qofbook.h`, `gncOrganization.h`, etc.). In
opencog-central, these are consolidated into a self-contained `gncOwner.h`
header. The adapted tests cover the same functionality through the unified API.

Key differences from the gnucashmulti originals:
- No `qof_init()` / `qof_close()` calls (not available in unified branch)
- No `qof_book_new()` / `qof_book_destroy()` (book param is `gpointer`)
- No separate `gncOrganization.h` — all types in `gncOwner.h`
- No `QofMultiEntityCollection` API — entity management tested through
  `GncOrganization` entity list operations
- Address fields are string-based (`addr1`–`addr4`) instead of `GncAddress*`

## Contributing

When adding new tests:
1. Follow the existing fixture patterns (`GncOrganizationTest`, etc.)
2. Include null safety tests for any new functions
3. Add stress tests for operations that could be called at scale
4. Update this document with new test counts
