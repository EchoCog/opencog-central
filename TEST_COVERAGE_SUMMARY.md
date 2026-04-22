# Multi-Entity Test Coverage Summary

## Overview

This document summarizes the comprehensive end-to-end test coverage implemented
for the GnuCash Multi-Entity functionality in the CoGnuCash Unified Cognitive
Branch.

Ported from [rzonedevops/gnucashmulti](https://github.com/rzonedevops/gnucashmulti)
PR 3 (Feb 2026). Source commits: `16eb2e3797` through `9c0cc98ec9`.

## Test Files Created/Modified

### 1. **gtest-organization.cpp** (NEW)
- **Location**: `libgnucash/engine/test/gtest-organization.cpp`
- **Test Cases**: 46
- **Lines of Code**: ~500
- **Purpose**: Comprehensive testing of GncOrganization entity type

#### Organization Test Categories

**Basic Creation/Destruction** (3 tests)
- `CreateOrganization` — Verify successful creation with valid defaults
- `CreateWithNullBook` — Null book parameter handling
- `DestroyNullOrganization` — Null pointer safety

**Getter/Setter Functions** (12 tests)
- `SetAndGetID` — ID property
- `SetIDWithNull` — Null ID handling
- `SetAndGetName` — Name property
- `SetAndGetNotes` — Notes property
- `SetAndGetActive` — Active flag
- `SetAndGetCurrency` — Currency code
- `SetAndGetAddr1`–`SetAndGetAddr4` — Address lines
- `SetAndGetPhone` — Phone number
- `SetAndGetFax` — Fax number
- `SetAndGetEmail` — Email address

**Null Safety** (2 tests)
- `GettersWithNullOrg` — Null safety for getters
- `SettersWithNullOrg` — Null safety for setters

**Update Operations** (2 tests)
- `UpdateIDMultipleTimes` — Multiple updates
- `SetSameValueTwice` — Idempotent updates

**String Edge Cases** (4 tests)
- `SetEmptyStrings` — Empty string handling
- `SetLongStrings` — 1000 character strings
- `SetSpecialCharacters` — Special char handling (`& < > "`)
- `SetUnicodeStrings` — Unicode support (日本語)

**Entity Management** (8 tests)
- `AddSingleEntity` — Add one entity
- `AddMultipleEntities` — Add 5 entities
- `AddDuplicateEntity` — Duplicate prevention
- `RemoveEntity` — Remove specific entity
- `RemoveNonexistentEntity` — Remove non-member entity
- `EntityManagementWithNull` — Null pointer safety
- `MixedEntityTypes` — Multiple entity types

**Hierarchy** (5 tests)
- `SetAndGetParent` — Parent assignment
- `AddChild` — Child addition
- `RemoveChild` — Child removal
- `MultipleChildren` — Multiple children
- `SelfAsParentRejected` / `SelfAsChildRejected` — Self-reference prevention

**Tensor Operations** (3 tests)
- `SetGetTensorDimension` — Dimension setting
- `SetGetTensorData` — Data array operations
- `SetNullTensorData` — Null data handling

**Edit State** (4 tests)
- `BeginCommitEdit` — Full edit cycle with callback
- `RollbackEdit` — Rollback without callback
- `CommitEditWithoutBegin` — No-op on uncommitted
- `NullCallbackOnDone` — Null callback safety

**QOF/GUID** (2 tests)
- `HasValidGUID` — Non-zero GUID after creation
- `MultipleOrganizationsHaveUniqueGUIDs` — GUID uniqueness

**Integration** (1 test)
- `CompleteLifecycle` — Full property + entity workflow

**Scale** (2 tests)
- `AddManyEntities` — 1000 entities
- `MultipleOrganizationsInBook` — 10 orgs with unique GUIDs

### 2. **gtest-qof-multi-entity.cpp** (NEW — E2E version)
- **Location**: `libgnucash/engine/test/gtest-qof-multi-entity.cpp`
- **Test Cases**: 38
- **Lines of Code**: ~550
- **Purpose**: Multi-entity aggregation and Owner integration tests

#### Multi-Entity Test Categories

**Basic Entity Operations** (13 tests)
- Collection create/destroy, add/remove, count, contains
- Duplicate prevention, mixed types, null handling
- Complete removal and iteration

**Organization Integration** (25 tests)
- Organization creation with entities
- Multi-org filtering and complex scenarios
- Large-scale (10,000 entities) and stress tests
- Owner type integration (init, compare, copy)
- Hierarchy depth and sibling management
- Tensor multi-entity operations
- Edit/commit/rollback integration
- Default values (currency, active flag)
- Entity add/remove cycles

### 3. **gtest-multi-entity-edge-cases.cpp** (NEW)
- **Location**: `libgnucash/engine/test/gtest-multi-entity-edge-cases.cpp`
- **Test Cases**: 32
- **Lines of Code**: ~470
- **Purpose**: Edge cases, boundary conditions, and stress testing

#### Edge Case Test Categories

**GUID Edge Cases** (3 tests)
- Non-zero GUID after creation
- Null GUID for null org
- GUID uniqueness across instances

**Empty Collection Operations** (4 tests)
- Empty entity/children lists
- Remove from empty lists

**Hierarchy Edge Cases** (4 tests)
- Deep hierarchy (10 levels)
- Circular reference prevention
- Null child/parent handling

**Tensor Boundaries** (5 tests)
- Zero and large dimensions
- Null org tensor operations
- Data replacement

**Owner Type Edge Cases** (6 tests)
- Null org in owner
- Copy preservation
- Equality/inequality
- All type strings
- Invalid type validation

**Edit State Edge Cases** (4 tests)
- Double begin edit
- Commit/rollback without begin
- Null org edit operations

**Stress Tests** (6 tests)
- 5,000 entities with partial removal
- 100 organizations created
- 1,000 rapid property updates
- 50 concurrent child operations
- 10 orgs × 100 entities merge stress

## Overall Coverage Statistics

| Metric | Count |
|--------|-------|
| Total Test Files | 3 (new) + 1 (existing) |
| Total Test Cases | 116 (new) + existing |
| GncOrganization Functions Covered | All 22+ CRUD functions |
| GncOwner Functions Covered | All 8 owner type functions |
| Null Safety Tests | 20+ |
| Stress/Scale Tests | 8 |
| Edge Case Tests | 32 |

## Bugfix Ported

The original gnucashmulti commit `9d9e71b023` fixed a null pointer bug in
`gncOrganizationOnDone` where `gncAddressClearDirty(organization->addr)` was
called without checking if `addr` was NULL. In the unified branch
(opencog-central), the address model uses individual string fields
(`addr1`–`addr4`) instead of a `GncAddress*` object, so this specific bug
does not apply. The equivalent null safety is inherently handled by the
`safe_free()` helper pattern used in `gncOwner.cpp`.
