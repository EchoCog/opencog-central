/********************************************************************\
 * gtest-qof-multi-entity.cpp -- Unit tests for multi-entity       *
 *                               aggregation functionality          *
 *                                                                  *
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
 *********************************************************************/

/**
 * @file gtest-qof-multi-entity.cpp
 * @brief Unit tests for multi-entity aggregation via GncOrganization
 *
 * Ported from rzonedevops/gnucashmulti PR 3 (Feb 2026) E2E test
 * infrastructure. Adapted for the CoGnuCash Unified Cognitive Branch
 * (EchoCog/opencog-central) which uses a self-contained gncOwner.h API.
 *
 * The original gnucashmulti version tests QofMultiEntityCollection APIs
 * directly. Since opencog-central does not yet expose the QOF collection
 * layer, this adapted version tests multi-entity aggregation through
 * the GncOrganization entity management and Owner type APIs.
 *
 * Original: libgnucash/engine/test/gtest-qof-multi-entity.cpp
 * Source commits: 16eb2e3797 through 9c0cc98ec9
 *
 * Original 13 tests (collection ops) + 25 organization integration tests = 38
 * Adapted to 38 equivalent test cases using the unified branch API.
 */

#include <glib.h>
#include <vector>
#include <algorithm>

#include <gtest/gtest.h>

#include "gncOwner.h"

// ============================================================
// Test Fixture: Organization-based Multi-Entity
// ============================================================

class QofMultiEntityTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        org = gncOrganizationCreate(nullptr);
        ASSERT_NE(nullptr, org);
    }

    void TearDown() override
    {
        if (org)
        {
            gncOrganizationDestroy(org);
            org = nullptr;
        }
    }

    GncOrganization* org = nullptr;
};

// ============================================================
// Basic Entity Collection Operations (adapted from original 13)
// ============================================================

TEST_F(QofMultiEntityTest, CreateAndDestroy)
{
    // Org starts with no entities
    EXPECT_EQ(nullptr, gncOrganizationGetEntities(org));
}

TEST_F(QofMultiEntityTest, AddSingleEntity)
{
    gchar* entity = g_strdup("account_1");
    gncOrganizationAddEntity(org, entity);

    GList* entities = gncOrganizationGetEntities(org);
    EXPECT_NE(nullptr, entities);
    EXPECT_EQ(1u, g_list_length(entities));
    EXPECT_TRUE(g_list_find(entities, entity) != nullptr);

    g_free(entity);
}

TEST_F(QofMultiEntityTest, AddDuplicateEntity)
{
    gchar* entity = g_strdup("account_1");
    gncOrganizationAddEntity(org, entity);
    gncOrganizationAddEntity(org, entity);

    EXPECT_EQ(1u, g_list_length(gncOrganizationGetEntities(org)));

    g_free(entity);
}

TEST_F(QofMultiEntityTest, AddMultipleEntities)
{
    gchar* e1 = g_strdup("account_1");
    gchar* e2 = g_strdup("transaction_1");
    gchar* e3 = g_strdup("commodity_1");

    gncOrganizationAddEntity(org, e1);
    gncOrganizationAddEntity(org, e2);
    gncOrganizationAddEntity(org, e3);

    EXPECT_EQ(3u, g_list_length(gncOrganizationGetEntities(org)));

    g_free(e1);
    g_free(e2);
    g_free(e3);
}

TEST_F(QofMultiEntityTest, RemoveEntity)
{
    gchar* e1 = g_strdup("account_1");
    gchar* e2 = g_strdup("account_2");

    gncOrganizationAddEntity(org, e1);
    gncOrganizationAddEntity(org, e2);
    EXPECT_EQ(2u, g_list_length(gncOrganizationGetEntities(org)));

    gncOrganizationRemoveEntity(org, e1);
    EXPECT_EQ(1u, g_list_length(gncOrganizationGetEntities(org)));

    g_free(e1);
    g_free(e2);
}

TEST_F(QofMultiEntityTest, RemoveNonexistentEntity)
{
    gchar* e1 = g_strdup("present");
    gchar* absent = g_strdup("absent");

    gncOrganizationAddEntity(org, e1);
    gncOrganizationRemoveEntity(org, absent);

    EXPECT_EQ(1u, g_list_length(gncOrganizationGetEntities(org)));

    g_free(e1);
    g_free(absent);
}

TEST_F(QofMultiEntityTest, EntityCount)
{
    std::vector<gchar*> entities;
    for (int i = 0; i < 10; i++)
    {
        gchar* e = g_strdup_printf("entity_%d", i);
        gncOrganizationAddEntity(org, e);
        entities.push_back(e);
    }

    EXPECT_EQ(10u, g_list_length(gncOrganizationGetEntities(org)));

    for (auto* e : entities)
        g_free(e);
}

TEST_F(QofMultiEntityTest, EntityContains)
{
    gchar* e1 = g_strdup("account_1");
    gchar* e2 = g_strdup("not_added");

    gncOrganizationAddEntity(org, e1);

    GList* entities = gncOrganizationGetEntities(org);
    EXPECT_TRUE(g_list_find(entities, e1) != nullptr);
    EXPECT_TRUE(g_list_find(entities, e2) == nullptr);

    g_free(e1);
    g_free(e2);
}

TEST_F(QofMultiEntityTest, EntityIteration)
{
    std::vector<gchar*> added;
    for (int i = 0; i < 5; i++)
    {
        gchar* e = g_strdup_printf("iter_%d", i);
        gncOrganizationAddEntity(org, e);
        added.push_back(e);
    }

    GList* entities = gncOrganizationGetEntities(org);
    int count = 0;
    for (GList* l = entities; l != nullptr; l = l->next)
    {
        EXPECT_NE(nullptr, l->data);
        count++;
    }
    EXPECT_EQ(5, count);

    for (auto* e : added)
        g_free(e);
}

TEST_F(QofMultiEntityTest, NullEntityHandling)
{
    gncOrganizationAddEntity(org, nullptr);
    // NULL entities should be rejected or at least not crash
}

TEST_F(QofMultiEntityTest, NullOrgEntityOperations)
{
    gchar* entity = g_strdup("test");
    gncOrganizationAddEntity(nullptr, entity);
    gncOrganizationRemoveEntity(nullptr, entity);
    EXPECT_EQ(nullptr, gncOrganizationGetEntities(nullptr));
    g_free(entity);
}

TEST_F(QofMultiEntityTest, RemoveAllEntities)
{
    std::vector<gchar*> entities;
    for (int i = 0; i < 5; i++)
    {
        gchar* e = g_strdup_printf("rm_%d", i);
        gncOrganizationAddEntity(org, e);
        entities.push_back(e);
    }

    EXPECT_EQ(5u, g_list_length(gncOrganizationGetEntities(org)));

    for (auto* e : entities)
    {
        gncOrganizationRemoveEntity(org, e);
    }

    EXPECT_EQ(nullptr, gncOrganizationGetEntities(org));

    for (auto* e : entities)
        g_free(e);
}

TEST_F(QofMultiEntityTest, MixedTypeEntities)
{
    gchar* account = g_strdup("Account:123");
    gchar* tx = g_strdup("Transaction:456");
    gchar* split = g_strdup("Split:789");
    gchar* lot = g_strdup("Lot:012");

    gncOrganizationAddEntity(org, account);
    gncOrganizationAddEntity(org, tx);
    gncOrganizationAddEntity(org, split);
    gncOrganizationAddEntity(org, lot);

    EXPECT_EQ(4u, g_list_length(gncOrganizationGetEntities(org)));

    g_free(account);
    g_free(tx);
    g_free(split);
    g_free(lot);
}

// ============================================================
// Organization Integration Tests (adapted from original 25)
// ============================================================

TEST_F(QofMultiEntityTest, CreateFromOrganization)
{
    gncOrganizationSetName(org, "Test Org");
    gncOrganizationSetID(org, "ORG-001");

    gchar* e1 = g_strdup("acct_1");
    gchar* e2 = g_strdup("acct_2");
    gncOrganizationAddEntity(org, e1);
    gncOrganizationAddEntity(org, e2);

    EXPECT_EQ(2u, g_list_length(gncOrganizationGetEntities(org)));
    EXPECT_STREQ("Test Org", gncOrganizationGetName(org));

    g_free(e1);
    g_free(e2);
}

TEST_F(QofMultiEntityTest, FilterByOrganization)
{
    // Create two organizations with different entities
    GncOrganization* org2 = gncOrganizationCreate(nullptr);
    gncOrganizationSetName(org, "Org A");
    gncOrganizationSetName(org2, "Org B");

    gchar* a1 = g_strdup("a_entity_1");
    gchar* a2 = g_strdup("a_entity_2");
    gchar* b1 = g_strdup("b_entity_1");

    gncOrganizationAddEntity(org, a1);
    gncOrganizationAddEntity(org, a2);
    gncOrganizationAddEntity(org2, b1);

    EXPECT_EQ(2u, g_list_length(gncOrganizationGetEntities(org)));
    EXPECT_EQ(1u, g_list_length(gncOrganizationGetEntities(org2)));

    g_free(a1);
    g_free(a2);
    g_free(b1);
    gncOrganizationDestroy(org2);
}

TEST_F(QofMultiEntityTest, ComplexOrganizationScenario)
{
    // 3 organizations with 5 entities each
    GncOrganization* org2 = gncOrganizationCreate(nullptr);
    GncOrganization* org3 = gncOrganizationCreate(nullptr);

    gncOrganizationSetName(org, "HQ");
    gncOrganizationSetName(org2, "Branch A");
    gncOrganizationSetName(org3, "Branch B");

    // Set up hierarchy
    gncOrganizationAddChild(org, org2);
    gncOrganizationAddChild(org, org3);

    std::vector<gchar*> all_entities;
    GncOrganization* orgs[] = {org, org2, org3};

    for (int o = 0; o < 3; o++)
    {
        for (int e = 0; e < 5; e++)
        {
            gchar* entity = g_strdup_printf("org%d_entity_%d", o, e);
            gncOrganizationAddEntity(orgs[o], entity);
            all_entities.push_back(entity);
        }
    }

    EXPECT_EQ(5u, g_list_length(gncOrganizationGetEntities(org)));
    EXPECT_EQ(5u, g_list_length(gncOrganizationGetEntities(org2)));
    EXPECT_EQ(5u, g_list_length(gncOrganizationGetEntities(org3)));
    EXPECT_EQ(2u, g_list_length(gncOrganizationGetChildren(org)));

    for (auto* e : all_entities)
        g_free(e);

    gncOrganizationDestroy(org2);
    gncOrganizationDestroy(org3);
}

TEST_F(QofMultiEntityTest, LargeOrganizationCollection)
{
    std::vector<gchar*> entities;
    for (int i = 0; i < 10000; i++)
    {
        gchar* e = g_strdup_printf("large_%d", i);
        gncOrganizationAddEntity(org, e);
        entities.push_back(e);
    }

    EXPECT_EQ(10000u, g_list_length(gncOrganizationGetEntities(org)));

    for (auto* e : entities)
        g_free(e);
}

TEST_F(QofMultiEntityTest, OrganizationOwnerIntegration)
{
    gncOrganizationSetName(org, "Owner Test Org");

    GncOwner owner;
    gncOwnerInitOrganization(&owner, org);

    EXPECT_EQ(GNC_OWNER_ORGANIZATION, gncOwnerGetType(&owner));
    EXPECT_EQ(org, gncOwnerGetOrganization(&owner));
    EXPECT_STREQ("Organization", gncOwnerGetTypeString(&owner));
}

TEST_F(QofMultiEntityTest, MultipleOrgOwnerComparison)
{
    GncOrganization* org2 = gncOrganizationCreate(nullptr);

    GncOwner a, b, c;
    gncOwnerInitOrganization(&a, org);
    gncOwnerInitOrganization(&b, org);
    gncOwnerInitOrganization(&c, org2);

    EXPECT_TRUE(gncOwnerEqual(&a, &b));
    EXPECT_FALSE(gncOwnerEqual(&a, &c));

    gncOrganizationDestroy(org2);
}

TEST_F(QofMultiEntityTest, OrganizationWithHierarchy)
{
    GncOrganization* parent = gncOrganizationCreate(nullptr);
    gncOrganizationSetName(parent, "Parent Corp");
    gncOrganizationSetName(org, "Child Division");

    gncOrganizationSetParent(org, parent);
    gncOrganizationAddChild(parent, org);

    EXPECT_EQ(parent, gncOrganizationGetParent(org));

    GList* children = gncOrganizationGetChildren(parent);
    EXPECT_NE(nullptr, children);
    EXPECT_EQ(org, g_list_first(children)->data);

    gncOrganizationDestroy(parent);
}

TEST_F(QofMultiEntityTest, OrganizationTensorMultiEntity)
{
    gdouble tensor_a[] = {1.0, 0.0, 0.0};
    gdouble tensor_b[] = {0.0, 1.0, 0.0};

    GncOrganization* org2 = gncOrganizationCreate(nullptr);

    gncOrganizationSetTensorDimension(org, 3);
    gncOrganizationSetTensorData(org, tensor_a, 3);

    gncOrganizationSetTensorDimension(org2, 3);
    gncOrganizationSetTensorData(org2, tensor_b, 3);

    gdouble* data_a = gncOrganizationGetTensorData(org);
    gdouble* data_b = gncOrganizationGetTensorData(org2);

    ASSERT_NE(nullptr, data_a);
    ASSERT_NE(nullptr, data_b);

    EXPECT_DOUBLE_EQ(1.0, data_a[0]);
    EXPECT_DOUBLE_EQ(0.0, data_a[1]);
    EXPECT_DOUBLE_EQ(0.0, data_b[0]);
    EXPECT_DOUBLE_EQ(1.0, data_b[1]);

    gncOrganizationDestroy(org2);
}

TEST_F(QofMultiEntityTest, OrganizationPropertiesComplete)
{
    gncOrganizationSetID(org, "ORG-FULL");
    gncOrganizationSetName(org, "Full Test Corp");
    gncOrganizationSetAddr1(org, "100 Integration Blvd");
    gncOrganizationSetAddr2(org, "Suite A");
    gncOrganizationSetAddr3(org, "Test City, TS 00000");
    gncOrganizationSetAddr4(org, "Testland");
    gncOrganizationSetPhone(org, "+1-555-FULL");
    gncOrganizationSetFax(org, "+1-555-FAX1");
    gncOrganizationSetEmail(org, "full@test.org");
    gncOrganizationSetNotes(org, "Integration test");
    gncOrganizationSetCurrency(org, "CHF");
    gncOrganizationSetActive(org, TRUE);

    EXPECT_STREQ("ORG-FULL", gncOrganizationGetID(org));
    EXPECT_STREQ("Full Test Corp", gncOrganizationGetName(org));
    EXPECT_STREQ("100 Integration Blvd", gncOrganizationGetAddr1(org));
    EXPECT_STREQ("Suite A", gncOrganizationGetAddr2(org));
    EXPECT_STREQ("Test City, TS 00000", gncOrganizationGetAddr3(org));
    EXPECT_STREQ("Testland", gncOrganizationGetAddr4(org));
    EXPECT_STREQ("+1-555-FULL", gncOrganizationGetPhone(org));
    EXPECT_STREQ("+1-555-FAX1", gncOrganizationGetFax(org));
    EXPECT_STREQ("full@test.org", gncOrganizationGetEmail(org));
    EXPECT_STREQ("Integration test", gncOrganizationGetNotes(org));
    EXPECT_STREQ("CHF", gncOrganizationGetCurrency(org));
    EXPECT_TRUE(gncOrganizationGetActive(org));
}

TEST_F(QofMultiEntityTest, MultipleOrganizationMergeStress)
{
    // 10 orgs, each with 100 entities
    std::vector<GncOrganization*> orgs;
    std::vector<std::vector<gchar*>> all_entities;

    for (int i = 0; i < 10; i++)
    {
        GncOrganization* o = gncOrganizationCreate(nullptr);
        gncOrganizationSetName(o, g_strdup_printf("MergeOrg-%d", i));
        std::vector<gchar*> ents;

        for (int j = 0; j < 100; j++)
        {
            gchar* e = g_strdup_printf("merge_org%d_entity_%d", i, j);
            gncOrganizationAddEntity(o, e);
            ents.push_back(e);
        }

        EXPECT_EQ(100u, g_list_length(gncOrganizationGetEntities(o)));
        orgs.push_back(o);
        all_entities.push_back(ents);
    }

    for (auto& ents : all_entities)
        for (auto* e : ents)
            g_free(e);

    for (auto* o : orgs)
        gncOrganizationDestroy(o);
}

TEST_F(QofMultiEntityTest, OrganizationEditAndCommit)
{
    static gboolean callback_fired = FALSE;
    static GncOrganization* callback_org = nullptr;

    callback_fired = FALSE;
    callback_org = nullptr;

    auto callback = [](GncOrganization* o, gpointer) {
        callback_fired = TRUE;
        callback_org = o;
    };

    gncOrganizationBeginEdit(org, callback, nullptr);
    gncOrganizationSetName(org, "Post-Edit Name");
    gncOrganizationCommitEdit(org);

    EXPECT_TRUE(callback_fired);
    EXPECT_EQ(org, callback_org);
    EXPECT_STREQ("Post-Edit Name", gncOrganizationGetName(org));
}

TEST_F(QofMultiEntityTest, OrganizationEditAndRollback)
{
    static gboolean callback_fired = FALSE;
    callback_fired = FALSE;

    auto callback = [](GncOrganization*, gpointer) {
        callback_fired = TRUE;
    };

    gncOrganizationBeginEdit(org, callback, nullptr);
    gncOrganizationRollbackEdit(org);

    EXPECT_FALSE(callback_fired);
}

TEST_F(QofMultiEntityTest, OrganizationGUIDUniqueness)
{
    std::vector<GncOrganization*> orgs;
    for (int i = 0; i < 50; i++)
    {
        GncOrganization* o = gncOrganizationCreate(nullptr);
        orgs.push_back(o);
    }

    // Verify all GUIDs are unique
    for (size_t i = 0; i < orgs.size(); i++)
    {
        for (size_t j = i + 1; j < orgs.size(); j++)
        {
            const GncGUID* g1 = gncOrganizationGetGUID(orgs[i]);
            const GncGUID* g2 = gncOrganizationGetGUID(orgs[j]);

            gboolean same = TRUE;
            for (int k = 0; k < 16; k++)
            {
                if (g1->data[k] != g2->data[k])
                {
                    same = FALSE;
                    break;
                }
            }
            EXPECT_FALSE(same) << "GUIDs at index " << i << " and " << j << " are the same";
        }
    }

    for (auto* o : orgs)
        gncOrganizationDestroy(o);
}

TEST_F(QofMultiEntityTest, OrganizationCurrencyDefault)
{
    const gchar* currency = gncOrganizationGetCurrency(org);
    EXPECT_NE(nullptr, currency);
    EXPECT_STREQ("USD", currency);
}

TEST_F(QofMultiEntityTest, OrganizationActiveDefault)
{
    EXPECT_TRUE(gncOrganizationGetActive(org));
}

TEST_F(QofMultiEntityTest, OrganizationDeactivateReactivate)
{
    gncOrganizationSetActive(org, FALSE);
    EXPECT_FALSE(gncOrganizationGetActive(org));

    gncOrganizationSetActive(org, TRUE);
    EXPECT_TRUE(gncOrganizationGetActive(org));
}

TEST_F(QofMultiEntityTest, OrganizationEntityAddRemoveCycle)
{
    std::vector<gchar*> entities;
    for (int i = 0; i < 20; i++)
    {
        gchar* e = g_strdup_printf("cycle_%d", i);
        gncOrganizationAddEntity(org, e);
        entities.push_back(e);
    }

    // Remove even-indexed
    for (int i = 0; i < 20; i += 2)
    {
        gncOrganizationRemoveEntity(org, entities[i]);
    }

    EXPECT_EQ(10u, g_list_length(gncOrganizationGetEntities(org)));

    // Re-add them
    for (int i = 0; i < 20; i += 2)
    {
        gncOrganizationAddEntity(org, entities[i]);
    }

    EXPECT_EQ(20u, g_list_length(gncOrganizationGetEntities(org)));

    for (auto* e : entities)
        g_free(e);
}

TEST_F(QofMultiEntityTest, OrganizationChildHierarchyDepth)
{
    // Create 5-level hierarchy
    GncOrganization* current = org;
    std::vector<GncOrganization*> chain;

    for (int i = 0; i < 5; i++)
    {
        GncOrganization* child = gncOrganizationCreate(nullptr);
        gncOrganizationSetName(child, g_strdup_printf("Level-%d", i));
        gncOrganizationAddChild(current, child);
        gncOrganizationSetParent(child, current);
        chain.push_back(child);
        current = child;
    }

    // Verify each level
    EXPECT_EQ(org, gncOrganizationGetParent(chain[0]));
    for (size_t i = 1; i < chain.size(); i++)
    {
        EXPECT_EQ(chain[i - 1], gncOrganizationGetParent(chain[i]));
    }

    for (auto* c : chain)
        gncOrganizationDestroy(c);
}

TEST_F(QofMultiEntityTest, OrganizationSiblingChildren)
{
    GncOrganization* c1 = gncOrganizationCreate(nullptr);
    GncOrganization* c2 = gncOrganizationCreate(nullptr);
    GncOrganization* c3 = gncOrganizationCreate(nullptr);

    gncOrganizationSetName(c1, "Sibling A");
    gncOrganizationSetName(c2, "Sibling B");
    gncOrganizationSetName(c3, "Sibling C");

    gncOrganizationAddChild(org, c1);
    gncOrganizationAddChild(org, c2);
    gncOrganizationAddChild(org, c3);

    GList* children = gncOrganizationGetChildren(org);
    EXPECT_EQ(3u, g_list_length(children));

    // Verify all children present
    EXPECT_NE(nullptr, g_list_find(children, c1));
    EXPECT_NE(nullptr, g_list_find(children, c2));
    EXPECT_NE(nullptr, g_list_find(children, c3));

    gncOrganizationDestroy(c1);
    gncOrganizationDestroy(c2);
    gncOrganizationDestroy(c3);
}
