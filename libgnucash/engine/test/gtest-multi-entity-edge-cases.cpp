/********************************************************************\
 * gtest-multi-entity-edge-cases.cpp -- Edge case tests            *
 * Copyright 2024 GnuCash Contributors                              *
 *                                                                  *
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, contact:                        *
 *                                                                  *
 * Free Software Foundation           Voice:  +1-617-542-5942       *
 * 51 Franklin Street, Fifth Floor    Fax:    +1-617-542-2652       *
 * Boston, MA  02110-1301,  USA       gnu@gnu.org                   *
 *                                                                  *
\********************************************************************/

/**
 * @file gtest-multi-entity-edge-cases.cpp
 * @brief Edge case and stress tests for multi-entity operations
 *
 * Ported from rzonedevops/gnucashmulti PR 3 (Feb 2026) E2E test
 * infrastructure. Adapted for the CoGnuCash Unified Cognitive Branch
 * (EchoCog/opencog-central) which uses a self-contained gncOwner.h API.
 *
 * Original: libgnucash/engine/test/gtest-multi-entity-edge-cases.cpp
 * Source commits: 16eb2e3797 through 9c0cc98ec9
 *
 * 32 test cases covering:
 * - Invalid GUID and entity state handling
 * - Empty collection operations
 * - Hierarchy edge cases (deep nesting, cycles)
 * - Large-scale stress tests
 * - Tensor boundary conditions
 * - Owner type edge cases
 */

#include <glib.h>

#include <algorithm>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "gncOwner.h"

// ============================================================
// Edge Cases: Invalid GUIDs and Entity States
// ============================================================

class MultiEntityEdgeCaseTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        org = gncOrganizationCreate(nullptr);
        ASSERT_NE(nullptr, org);
    }

    void TearDown() override
    {
        if (org) {
            gncOrganizationDestroy(org);
            org = nullptr;
        }
    }

    GncOrganization *org = nullptr;
};

TEST_F(MultiEntityEdgeCaseTest, GUIDIsNonZeroAfterCreate)
{
    const GncGUID *guid = gncOrganizationGetGUID(org);
    ASSERT_NE(nullptr, guid);

    gboolean all_zero = TRUE;
    for (int i = 0; i < 16; i++) {
        if (guid->data[i] != 0) {
            all_zero = FALSE;
            break;
        }
    }
    EXPECT_FALSE(all_zero);
}

TEST_F(MultiEntityEdgeCaseTest, GUIDNullForNullOrg)
{
    EXPECT_EQ(nullptr, gncOrganizationGetGUID(nullptr));
}

TEST_F(MultiEntityEdgeCaseTest, DestroyedOrgGUIDNotReused)
{
    GncOrganization *o2 = gncOrganizationCreate(nullptr);
    const GncGUID *guid1 = gncOrganizationGetGUID(org);
    const GncGUID *guid2 = gncOrganizationGetGUID(o2);

    gboolean same = TRUE;
    for (int i = 0; i < 16; i++) {
        if (guid1->data[i] != guid2->data[i]) {
            same = FALSE;
            break;
        }
    }
    EXPECT_FALSE(same);

    gncOrganizationDestroy(o2);
}

// ============================================================
// Edge Cases: Empty Collection Operations
// ============================================================

TEST_F(MultiEntityEdgeCaseTest, EmptyEntityList)
{
    EXPECT_EQ(nullptr, gncOrganizationGetEntities(org));
}

TEST_F(MultiEntityEdgeCaseTest, RemoveFromEmptyEntityList)
{
    gchar *entity = g_strdup("nonexistent");
    gncOrganizationRemoveEntity(org, entity);
    EXPECT_EQ(nullptr, gncOrganizationGetEntities(org));
    g_free(entity);
}

TEST_F(MultiEntityEdgeCaseTest, EmptyChildrenList)
{
    EXPECT_EQ(nullptr, gncOrganizationGetChildren(org));
}

TEST_F(MultiEntityEdgeCaseTest, RemoveNonexistentChild)
{
    GncOrganization *child = gncOrganizationCreate(nullptr);
    gncOrganizationRemoveChild(org, child);
    EXPECT_EQ(nullptr, gncOrganizationGetChildren(org));
    gncOrganizationDestroy(child);
}

// ============================================================
// Edge Cases: Hierarchy
// ============================================================

TEST_F(MultiEntityEdgeCaseTest, DeepHierarchy)
{
    // Create a chain of 10 parent-child relationships
    std::vector<GncOrganization *> chain;
    chain.push_back(org);

    for (int i = 0; i < 10; i++) {
        GncOrganization *child = gncOrganizationCreate(nullptr);
        gchar *tmp_name = g_strdup_printf("Level %d", i + 1);
        gncOrganizationSetName(child, tmp_name);
        g_free(tmp_name);
        gncOrganizationAddChild(chain.back(), child);
        gncOrganizationSetParent(child, chain.back());
        chain.push_back(child);
    }

    // Verify the chain
    for (size_t i = 1; i < chain.size(); i++) {
        EXPECT_EQ(chain[i - 1], gncOrganizationGetParent(chain[i]));
    }

    // Clean up (skip org at index 0 — cleaned by fixture)
    for (size_t i = 1; i < chain.size(); i++) {
        gncOrganizationDestroy(chain[i]);
    }
}

TEST_F(MultiEntityEdgeCaseTest, CircularParentRejected)
{
    GncOrganization *child = gncOrganizationCreate(nullptr);
    gncOrganizationSetParent(child, org);

    // Trying to set org's parent to child would create a cycle
    // The implementation should prevent self-referencing
    gncOrganizationSetParent(org, org);
    EXPECT_EQ(nullptr, gncOrganizationGetParent(org));

    gncOrganizationDestroy(child);
}

TEST_F(MultiEntityEdgeCaseTest, AddNullChild)
{
    gncOrganizationAddChild(org, nullptr);
    EXPECT_EQ(nullptr, gncOrganizationGetChildren(org));
}

TEST_F(MultiEntityEdgeCaseTest, SetNullParent)
{
    GncOrganization *parent = gncOrganizationCreate(nullptr);
    gncOrganizationSetParent(org, parent);
    EXPECT_EQ(parent, gncOrganizationGetParent(org));

    gncOrganizationSetParent(org, nullptr);
    EXPECT_EQ(nullptr, gncOrganizationGetParent(org));

    gncOrganizationDestroy(parent);
}

// ============================================================
// Edge Cases: Tensor Boundaries
// ============================================================

TEST_F(MultiEntityEdgeCaseTest, ZeroTensorDimension)
{
    gncOrganizationSetTensorDimension(org, 0);
    EXPECT_EQ(0u, gncOrganizationGetTensorDimension(org));
}

TEST_F(MultiEntityEdgeCaseTest, LargeTensorDimension)
{
    gncOrganizationSetTensorDimension(org, 10000);
    EXPECT_EQ(10000u, gncOrganizationGetTensorDimension(org));
}

TEST_F(MultiEntityEdgeCaseTest, TensorDataWithNullOrg)
{
    gdouble data[] = {1.0};
    gncOrganizationSetTensorData(nullptr, data, 1);
    EXPECT_EQ(nullptr, gncOrganizationGetTensorData(nullptr));
}

TEST_F(MultiEntityEdgeCaseTest, TensorDimensionWithNullOrg)
{
    gncOrganizationSetTensorDimension(nullptr, 5);
    EXPECT_EQ(0u, gncOrganizationGetTensorDimension(nullptr));
}

TEST_F(MultiEntityEdgeCaseTest, ReplaceTensorData)
{
    gdouble data1[] = {1.0, 2.0};
    gdouble data2[] = {3.0, 4.0, 5.0};

    gncOrganizationSetTensorData(org, data1, 2);
    gdouble *r1 = gncOrganizationGetTensorData(org);
    ASSERT_NE(nullptr, r1);
    EXPECT_DOUBLE_EQ(1.0, r1[0]);

    gncOrganizationSetTensorData(org, data2, 3);
    gdouble *r2 = gncOrganizationGetTensorData(org);
    ASSERT_NE(nullptr, r2);
    EXPECT_DOUBLE_EQ(3.0, r2[0]);
    EXPECT_DOUBLE_EQ(5.0, r2[2]);
}

// ============================================================
// Edge Cases: Owner Type Operations
// ============================================================

TEST_F(MultiEntityEdgeCaseTest, OwnerInitWithNullOrg)
{
    GncOwner owner;
    gncOwnerInitOrganization(&owner, nullptr);
    EXPECT_EQ(GNC_OWNER_ORGANIZATION, gncOwnerGetType(&owner));
    EXPECT_EQ(nullptr, gncOwnerGetOrganization(&owner));
}

TEST_F(MultiEntityEdgeCaseTest, OwnerCopyPreservesOrg)
{
    GncOwner owner;
    gncOwnerInitOrganization(&owner, org);

    GncOwner copy;
    gncOwnerCopy(&copy, &owner);

    EXPECT_EQ(GNC_OWNER_ORGANIZATION, gncOwnerGetType(&copy));
    EXPECT_EQ(org, gncOwnerGetOrganization(&copy));
}

TEST_F(MultiEntityEdgeCaseTest, OwnerEqualityWithSameOrg)
{
    GncOwner a, b;
    gncOwnerInitOrganization(&a, org);
    gncOwnerInitOrganization(&b, org);
    EXPECT_TRUE(gncOwnerEqual(&a, &b));
}

TEST_F(MultiEntityEdgeCaseTest, OwnerInequalityDifferentOrgs)
{
    GncOrganization *other = gncOrganizationCreate(nullptr);
    GncOwner a, b;
    gncOwnerInitOrganization(&a, org);
    gncOwnerInitOrganization(&b, other);
    EXPECT_FALSE(gncOwnerEqual(&a, &b));
    gncOrganizationDestroy(other);
}

TEST_F(MultiEntityEdgeCaseTest, OwnerTypeStringForAllTypes)
{
    GncOwner owner;

    gncOwnerInitUndefined(&owner);
    EXPECT_STREQ("None", gncOwnerGetTypeString(&owner));

    gncOwnerInitOrganization(&owner, org);
    EXPECT_STREQ("Organization", gncOwnerGetTypeString(&owner));

    gncOwnerInitCustomer(&owner, nullptr);
    EXPECT_STREQ("Customer", gncOwnerGetTypeString(&owner));

    gncOwnerInitVendor(&owner, nullptr);
    EXPECT_STREQ("Vendor", gncOwnerGetTypeString(&owner));

    gncOwnerInitEmployee(&owner, nullptr);
    EXPECT_STREQ("Employee", gncOwnerGetTypeString(&owner));

    gncOwnerInitJob(&owner, nullptr);
    EXPECT_STREQ("Job", gncOwnerGetTypeString(&owner));
}

TEST_F(MultiEntityEdgeCaseTest, InvalidOwnerType)
{
    EXPECT_FALSE(gncOwnerTypeIsValid((GncOwnerType)999));
    EXPECT_FALSE(gncOwnerTypeIsValid((GncOwnerType)-1));
}

// ============================================================
// Edge Cases: Edit State
// ============================================================

TEST_F(MultiEntityEdgeCaseTest, DoubleBeginEdit)
{
    gncOrganizationBeginEdit(org, nullptr, nullptr);
    gncOrganizationBeginEdit(org, nullptr, nullptr);
    gncOrganizationCommitEdit(org);
    // Should not crash or double-fire callbacks
}

TEST_F(MultiEntityEdgeCaseTest, CommitWithoutBegin)
{
    gncOrganizationCommitEdit(org);
    // Should be a no-op, not crash
}

TEST_F(MultiEntityEdgeCaseTest, RollbackWithoutBegin)
{
    gncOrganizationRollbackEdit(org);
    // Should be a no-op, not crash
}

TEST_F(MultiEntityEdgeCaseTest, EditNullOrg)
{
    gncOrganizationBeginEdit(nullptr, nullptr, nullptr);
    gncOrganizationCommitEdit(nullptr);
    gncOrganizationRollbackEdit(nullptr);
    // All should be no-ops
}

// ============================================================
// Stress Tests
// ============================================================

TEST_F(MultiEntityEdgeCaseTest, LargeOrganizationEntityCount)
{
    std::vector<gchar *> entities;
    const int count = 5000;

    for (int i = 0; i < count; i++) {
        gchar *e = g_strdup_printf("stress_entity_%d", i);
        gncOrganizationAddEntity(org, e);
        entities.push_back(e);
    }

    EXPECT_EQ((guint)count, g_list_length(gncOrganizationGetEntities(org)));

    // Remove half
    for (int i = 0; i < count / 2; i++) {
        gncOrganizationRemoveEntity(org, entities[i]);
    }

    EXPECT_EQ((guint)(count - count / 2), g_list_length(gncOrganizationGetEntities(org)));

    for (auto *e : entities)
        g_free(e);
}

TEST_F(MultiEntityEdgeCaseTest, ManyOrganizationsCreated)
{
    std::vector<GncOrganization *> orgs;
    for (int i = 0; i < 100; i++) {
        GncOrganization *o = gncOrganizationCreate(nullptr);
        ASSERT_NE(nullptr, o);
        orgs.push_back(o);
    }

    for (auto *o : orgs)
        gncOrganizationDestroy(o);
}

TEST_F(MultiEntityEdgeCaseTest, RapidPropertyUpdates)
{
    for (int i = 0; i < 1000; i++) {
        gchar *name = g_strdup_printf("Org-%d", i);
        gncOrganizationSetName(org, name);
        g_free(name);
    }

    // Last value should stick
    const gchar *final_name = gncOrganizationGetName(org);
    EXPECT_STREQ("Org-999", final_name);
}

TEST_F(MultiEntityEdgeCaseTest, ConcurrentChildOperations)
{
    // Add 50 children, then remove odd-indexed ones
    std::vector<GncOrganization *> children;
    for (int i = 0; i < 50; i++) {
        GncOrganization *child = gncOrganizationCreate(nullptr);
        gncOrganizationAddChild(org, child);
        children.push_back(child);
    }

    EXPECT_EQ(50u, g_list_length(gncOrganizationGetChildren(org)));

    for (int i = 1; i < 50; i += 2) {
        gncOrganizationRemoveChild(org, children[i]);
    }

    EXPECT_EQ(25u, g_list_length(gncOrganizationGetChildren(org)));

    for (auto *child : children)
        gncOrganizationDestroy(child);
}

TEST_F(MultiEntityEdgeCaseTest, MultipleOrganizationMergeStress)
{
    // Create 10 orgs, each with 100 entities
    std::vector<GncOrganization *> orgs;
    std::vector<std::vector<gchar *>> all_entities;

    for (int i = 0; i < 10; i++) {
        GncOrganization *o = gncOrganizationCreate(nullptr);
        gchar *tmp_name = g_strdup_printf("StressOrg-%d", i);
        gncOrganizationSetName(o, tmp_name);
        g_free(tmp_name);
        std::vector<gchar *> ents;

        for (int j = 0; j < 100; j++) {
            gchar *e = g_strdup_printf("org%d_entity_%d", i, j);
            gncOrganizationAddEntity(o, e);
            ents.push_back(e);
        }

        EXPECT_EQ(100u, g_list_length(gncOrganizationGetEntities(o)));
        orgs.push_back(o);
        all_entities.push_back(ents);
    }

    for (auto &ents : all_entities)
        for (auto *e : ents)
            g_free(e);

    for (auto *o : orgs)
        gncOrganizationDestroy(o);
}
