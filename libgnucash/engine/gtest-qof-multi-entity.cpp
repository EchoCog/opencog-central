/**
 * @file gtest-qof-multi-entity.cpp
 * @brief Unit tests for GNC_OWNER_ORGANIZATION and multi-entity features
 * 
 * This file contains Google Test unit tests for the GncOrganization
 * entity type and related multi-entity cognitive features.
 * 
 * Part of the CoGnuCash Unified Cognitive Branch.
 * Cherry-picked from rzonedevops/gnucashmulti PRs 1-15 (Sep 2025)
 * 
 * @author CoGnuCash Contributors
 * @copyright GPL v2+
 */

#include <gtest/gtest.h>
#include <glib.h>
#include "gncOwner.h"

/**
 * @brief Test fixture for Organization tests
 */
class GncOrganizationTest : public ::testing::Test {
protected:
    void SetUp() override {
        org = gncOrganizationCreate(nullptr);
        ASSERT_NE(org, nullptr);
    }
    
    void TearDown() override {
        if (org) {
            gncOrganizationDestroy(org);
            org = nullptr;
        }
    }
    
    GncOrganization *org = nullptr;
};

/**
 * @brief Test fixture for Owner tests
 */
class GncOwnerTest : public ::testing::Test {
protected:
    void SetUp() override {
        org = gncOrganizationCreate(nullptr);
        gncOwnerInitUndefined(&owner);
    }
    
    void TearDown() override {
        if (org) {
            gncOrganizationDestroy(org);
            org = nullptr;
        }
    }
    
    GncOwner owner;
    GncOrganization *org = nullptr;
};

/* ============================================================ */
/* Organization Creation Tests                                   */
/* ============================================================ */

TEST_F(GncOrganizationTest, CreateReturnsNonNull) {
    EXPECT_NE(org, nullptr);
}

TEST_F(GncOrganizationTest, CreateHasValidGUID) {
    const GncGUID *guid = gncOrganizationGetGUID(org);
    EXPECT_NE(guid, nullptr);
}

TEST_F(GncOrganizationTest, CreateIsActiveByDefault) {
    EXPECT_TRUE(gncOrganizationGetActive(org));
}

TEST_F(GncOrganizationTest, CreateHasUSDCurrencyByDefault) {
    const gchar *currency = gncOrganizationGetCurrency(org);
    EXPECT_NE(currency, nullptr);
    EXPECT_STREQ(currency, "USD");
}

TEST_F(GncOrganizationTest, CreateHasZeroTensorDimension) {
    EXPECT_EQ(gncOrganizationGetTensorDimension(org), 0u);
}

TEST_F(GncOrganizationTest, CreateHasNullTensorData) {
    EXPECT_EQ(gncOrganizationGetTensorData(org), nullptr);
}

TEST_F(GncOrganizationTest, CreateHasNullParent) {
    EXPECT_EQ(gncOrganizationGetParent(org), nullptr);
}

TEST_F(GncOrganizationTest, CreateHasEmptyChildren) {
    EXPECT_EQ(gncOrganizationGetChildren(org), nullptr);
}

TEST_F(GncOrganizationTest, CreateHasEmptyEntities) {
    EXPECT_EQ(gncOrganizationGetEntities(org), nullptr);
}

/* ============================================================ */
/* Organization Property Tests                                   */
/* ============================================================ */

TEST_F(GncOrganizationTest, SetGetName) {
    gncOrganizationSetName(org, "Test Organization");
    EXPECT_STREQ(gncOrganizationGetName(org), "Test Organization");
}

TEST_F(GncOrganizationTest, SetGetID) {
    gncOrganizationSetID(org, "ORG-001");
    EXPECT_STREQ(gncOrganizationGetID(org), "ORG-001");
}

TEST_F(GncOrganizationTest, SetGetAddr1) {
    gncOrganizationSetAddr1(org, "123 Main St");
    EXPECT_STREQ(gncOrganizationGetAddr1(org), "123 Main St");
}

TEST_F(GncOrganizationTest, SetGetAddr2) {
    gncOrganizationSetAddr2(org, "Suite 100");
    EXPECT_STREQ(gncOrganizationGetAddr2(org), "Suite 100");
}

TEST_F(GncOrganizationTest, SetGetAddr3) {
    gncOrganizationSetAddr3(org, "City, State 12345");
    EXPECT_STREQ(gncOrganizationGetAddr3(org), "City, State 12345");
}

TEST_F(GncOrganizationTest, SetGetAddr4) {
    gncOrganizationSetAddr4(org, "Country");
    EXPECT_STREQ(gncOrganizationGetAddr4(org), "Country");
}

TEST_F(GncOrganizationTest, SetGetPhone) {
    gncOrganizationSetPhone(org, "+1-555-123-4567");
    EXPECT_STREQ(gncOrganizationGetPhone(org), "+1-555-123-4567");
}

TEST_F(GncOrganizationTest, SetGetFax) {
    gncOrganizationSetFax(org, "+1-555-123-4568");
    EXPECT_STREQ(gncOrganizationGetFax(org), "+1-555-123-4568");
}

TEST_F(GncOrganizationTest, SetGetEmail) {
    gncOrganizationSetEmail(org, "org@example.com");
    EXPECT_STREQ(gncOrganizationGetEmail(org), "org@example.com");
}

TEST_F(GncOrganizationTest, SetGetNotes) {
    gncOrganizationSetNotes(org, "Test notes for organization");
    EXPECT_STREQ(gncOrganizationGetNotes(org), "Test notes for organization");
}

TEST_F(GncOrganizationTest, SetGetActive) {
    gncOrganizationSetActive(org, FALSE);
    EXPECT_FALSE(gncOrganizationGetActive(org));
    
    gncOrganizationSetActive(org, TRUE);
    EXPECT_TRUE(gncOrganizationGetActive(org));
}

TEST_F(GncOrganizationTest, SetGetCurrency) {
    gncOrganizationSetCurrency(org, "EUR");
    EXPECT_STREQ(gncOrganizationGetCurrency(org), "EUR");
}

/* ============================================================ */
/* Organization Hierarchy Tests                                  */
/* ============================================================ */

TEST_F(GncOrganizationTest, SetGetParent) {
    GncOrganization *parent = gncOrganizationCreate(nullptr);
    gncOrganizationSetName(parent, "Parent Org");
    
    gncOrganizationSetParent(org, parent);
    EXPECT_EQ(gncOrganizationGetParent(org), parent);
    
    gncOrganizationDestroy(parent);
}

TEST_F(GncOrganizationTest, AddChild) {
    GncOrganization *child = gncOrganizationCreate(nullptr);
    gncOrganizationSetName(child, "Child Org");
    
    gncOrganizationAddChild(org, child);
    
    GList *children = gncOrganizationGetChildren(org);
    EXPECT_NE(children, nullptr);
    EXPECT_EQ(g_list_length(children), 1u);
    EXPECT_EQ(g_list_first(children)->data, child);
    
    gncOrganizationDestroy(child);
}

TEST_F(GncOrganizationTest, RemoveChild) {
    GncOrganization *child = gncOrganizationCreate(nullptr);
    
    gncOrganizationAddChild(org, child);
    EXPECT_EQ(g_list_length(gncOrganizationGetChildren(org)), 1u);
    
    gncOrganizationRemoveChild(org, child);
    EXPECT_EQ(gncOrganizationGetChildren(org), nullptr);
    
    gncOrganizationDestroy(child);
}

TEST_F(GncOrganizationTest, MultipleChildren) {
    GncOrganization *child1 = gncOrganizationCreate(nullptr);
    GncOrganization *child2 = gncOrganizationCreate(nullptr);
    GncOrganization *child3 = gncOrganizationCreate(nullptr);
    
    gncOrganizationAddChild(org, child1);
    gncOrganizationAddChild(org, child2);
    gncOrganizationAddChild(org, child3);
    
    EXPECT_EQ(g_list_length(gncOrganizationGetChildren(org)), 3u);
    
    gncOrganizationDestroy(child1);
    gncOrganizationDestroy(child2);
    gncOrganizationDestroy(child3);
}

TEST_F(GncOrganizationTest, SelfAsParentRejected) {
    gncOrganizationSetParent(org, org);
    EXPECT_EQ(gncOrganizationGetParent(org), nullptr);
}

TEST_F(GncOrganizationTest, SelfAsChildRejected) {
    gncOrganizationAddChild(org, org);
    EXPECT_EQ(gncOrganizationGetChildren(org), nullptr);
}

/* ============================================================ */
/* Organization Entity Management Tests                          */
/* ============================================================ */

TEST_F(GncOrganizationTest, AddEntity) {
    gchar *entity = g_strdup("test_entity");
    
    gncOrganizationAddEntity(org, entity);
    
    GList *entities = gncOrganizationGetEntities(org);
    EXPECT_NE(entities, nullptr);
    EXPECT_EQ(g_list_length(entities), 1u);
    EXPECT_EQ(g_list_first(entities)->data, entity);
    
    g_free(entity);
}

TEST_F(GncOrganizationTest, RemoveEntity) {
    gchar *entity = g_strdup("test_entity");
    
    gncOrganizationAddEntity(org, entity);
    EXPECT_EQ(g_list_length(gncOrganizationGetEntities(org)), 1u);
    
    gncOrganizationRemoveEntity(org, entity);
    EXPECT_EQ(gncOrganizationGetEntities(org), nullptr);
    
    g_free(entity);
}

TEST_F(GncOrganizationTest, MultipleEntities) {
    gchar *entity1 = g_strdup("entity1");
    gchar *entity2 = g_strdup("entity2");
    gchar *entity3 = g_strdup("entity3");
    
    gncOrganizationAddEntity(org, entity1);
    gncOrganizationAddEntity(org, entity2);
    gncOrganizationAddEntity(org, entity3);
    
    EXPECT_EQ(g_list_length(gncOrganizationGetEntities(org)), 3u);
    
    g_free(entity1);
    g_free(entity2);
    g_free(entity3);
}

TEST_F(GncOrganizationTest, DuplicateEntityNotAdded) {
    gchar *entity = g_strdup("test_entity");
    
    gncOrganizationAddEntity(org, entity);
    gncOrganizationAddEntity(org, entity);
    
    EXPECT_EQ(g_list_length(gncOrganizationGetEntities(org)), 1u);
    
    g_free(entity);
}

/* ============================================================ */
/* Organization Tensor Tests                                     */
/* ============================================================ */

TEST_F(GncOrganizationTest, SetGetTensorDimension) {
    gncOrganizationSetTensorDimension(org, 5);
    EXPECT_EQ(gncOrganizationGetTensorDimension(org), 5u);
}

TEST_F(GncOrganizationTest, SetGetTensorData) {
    gdouble data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    gncOrganizationSetTensorDimension(org, 5);
    gncOrganizationSetTensorData(org, data, 5);
    
    gdouble *retrieved = gncOrganizationGetTensorData(org);
    EXPECT_NE(retrieved, nullptr);
    
    for (int i = 0; i < 5; i++) {
        EXPECT_DOUBLE_EQ(retrieved[i], data[i]);
    }
}

TEST_F(GncOrganizationTest, SetNullTensorData) {
    gdouble data[] = {1.0, 2.0, 3.0};
    gncOrganizationSetTensorData(org, data, 3);
    EXPECT_NE(gncOrganizationGetTensorData(org), nullptr);
    
    gncOrganizationSetTensorData(org, nullptr, 0);
    EXPECT_EQ(gncOrganizationGetTensorData(org), nullptr);
}

/* ============================================================ */
/* Owner Type Tests                                              */
/* ============================================================ */

TEST_F(GncOwnerTest, InitUndefined) {
    gncOwnerInitUndefined(&owner);
    EXPECT_EQ(gncOwnerGetType(&owner), GNC_OWNER_UNDEFINED);
}

TEST_F(GncOwnerTest, InitOrganization) {
    gncOwnerInitOrganization(&owner, org);
    EXPECT_EQ(gncOwnerGetType(&owner), GNC_OWNER_ORGANIZATION);
    EXPECT_EQ(gncOwnerGetOrganization(&owner), org);
}

TEST_F(GncOwnerTest, GetTypeString) {
    gncOwnerInitOrganization(&owner, org);
    EXPECT_STREQ(gncOwnerGetTypeString(&owner), "Organization");
}

TEST_F(GncOwnerTest, TypeValidation) {
    EXPECT_TRUE(gncOwnerTypeIsValid(GNC_OWNER_NONE));
    EXPECT_TRUE(gncOwnerTypeIsValid(GNC_OWNER_CUSTOMER));
    EXPECT_TRUE(gncOwnerTypeIsValid(GNC_OWNER_JOB));
    EXPECT_TRUE(gncOwnerTypeIsValid(GNC_OWNER_VENDOR));
    EXPECT_TRUE(gncOwnerTypeIsValid(GNC_OWNER_EMPLOYEE));
    EXPECT_TRUE(gncOwnerTypeIsValid(GNC_OWNER_ORGANIZATION));
    EXPECT_FALSE(gncOwnerTypeIsValid((GncOwnerType)999));
}

TEST_F(GncOwnerTest, GetOrganizationFromNonOrgOwner) {
    gncOwnerInitCustomer(&owner, nullptr);
    EXPECT_EQ(gncOwnerGetOrganization(&owner), nullptr);
}

TEST_F(GncOwnerTest, GetOrganizationFromOrgOwner) {
    gncOwnerInitOrganization(&owner, org);
    EXPECT_EQ(gncOwnerGetOrganization(&owner), org);
}

/* ============================================================ */
/* Owner Copy and Equality Tests                                 */
/* ============================================================ */

TEST_F(GncOwnerTest, CopyOwner) {
    gncOwnerInitOrganization(&owner, org);
    
    GncOwner copy;
    gncOwnerCopy(&copy, &owner);
    
    EXPECT_EQ(gncOwnerGetType(&copy), GNC_OWNER_ORGANIZATION);
    EXPECT_EQ(gncOwnerGetOrganization(&copy), org);
}

TEST_F(GncOwnerTest, EqualOwners) {
    GncOwner owner1, owner2;
    gncOwnerInitOrganization(&owner1, org);
    gncOwnerInitOrganization(&owner2, org);
    
    EXPECT_TRUE(gncOwnerEqual(&owner1, &owner2));
}

TEST_F(GncOwnerTest, NotEqualDifferentTypes) {
    GncOwner owner1, owner2;
    gncOwnerInitOrganization(&owner1, org);
    gncOwnerInitCustomer(&owner2, nullptr);
    
    EXPECT_FALSE(gncOwnerEqual(&owner1, &owner2));
}

TEST_F(GncOwnerTest, EqualNullOwners) {
    EXPECT_FALSE(gncOwnerEqual(nullptr, &owner));
    EXPECT_FALSE(gncOwnerEqual(&owner, nullptr));
}

TEST_F(GncOwnerTest, EqualSamePointer) {
    gncOwnerInitOrganization(&owner, org);
    EXPECT_TRUE(gncOwnerEqual(&owner, &owner));
}

/* ============================================================ */
/* Organization Edit Tests                                       */
/* ============================================================ */

static gboolean edit_done_called = FALSE;
static GncOrganization *edit_done_org = nullptr;

static void
test_edit_done_callback(GncOrganization *org, gpointer user_data)
{
    edit_done_called = TRUE;
    edit_done_org = org;
}

TEST_F(GncOrganizationTest, BeginCommitEdit) {
    edit_done_called = FALSE;
    edit_done_org = nullptr;
    
    gncOrganizationBeginEdit(org, test_edit_done_callback, nullptr);
    gncOrganizationSetName(org, "Edited Name");
    gncOrganizationCommitEdit(org);
    
    EXPECT_TRUE(edit_done_called);
    EXPECT_EQ(edit_done_org, org);
}

TEST_F(GncOrganizationTest, RollbackEdit) {
    edit_done_called = FALSE;
    
    gncOrganizationBeginEdit(org, test_edit_done_callback, nullptr);
    gncOrganizationRollbackEdit(org);
    
    EXPECT_FALSE(edit_done_called);
}

TEST_F(GncOrganizationTest, NullCallbackOnDone) {
    gncOrganizationBeginEdit(org, nullptr, nullptr);
    gncOrganizationCommitEdit(org);
}

/* ============================================================ */
/* Null Safety Tests                                             */
/* ============================================================ */

TEST(GncOrganizationNullTest, CreateWithNullBook) {
    GncOrganization *org = gncOrganizationCreate(nullptr);
    EXPECT_NE(org, nullptr);
    gncOrganizationDestroy(org);
}

TEST(GncOrganizationNullTest, DestroyNull) {
    gncOrganizationDestroy(nullptr);
}

TEST(GncOrganizationNullTest, GetGUIDNull) {
    EXPECT_EQ(gncOrganizationGetGUID(nullptr), nullptr);
}

TEST(GncOrganizationNullTest, SetNameNull) {
    gncOrganizationSetName(nullptr, "Test");
}

TEST(GncOrganizationNullTest, GetNameNull) {
    EXPECT_EQ(gncOrganizationGetName(nullptr), nullptr);
}

TEST(GncOrganizationNullTest, SetParentNull) {
    gncOrganizationSetParent(nullptr, nullptr);
}

TEST(GncOrganizationNullTest, AddChildNull) {
    gncOrganizationAddChild(nullptr, nullptr);
}

TEST(GncOrganizationNullTest, AddEntityNull) {
    gncOrganizationAddEntity(nullptr, nullptr);
}

TEST(GncOwnerNullTest, InitUndefinedNull) {
    gncOwnerInitUndefined(nullptr);
}

TEST(GncOwnerNullTest, GetTypeNull) {
    EXPECT_EQ(gncOwnerGetType(nullptr), GNC_OWNER_NONE);
}

TEST(GncOwnerNullTest, GetTypeStringNull) {
    EXPECT_STREQ(gncOwnerGetTypeString(nullptr), "None");
}

TEST(GncOwnerNullTest, GetOrganizationNull) {
    EXPECT_EQ(gncOwnerGetOrganization(nullptr), nullptr);
}

/* ============================================================ */
/* Main                                                          */
/* ============================================================ */

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
