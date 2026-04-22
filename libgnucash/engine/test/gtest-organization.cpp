/********************************************************************\
 * gtest-organization.cpp -- Unit tests for GncOrganization         *
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
 * @file gtest-organization.cpp
 * @brief Comprehensive unit tests for GncOrganization entity type
 *
 * Ported from rzonedevops/gnucashmulti PR 3 (Feb 2026) E2E test
 * infrastructure. Adapted for the CoGnuCash Unified Cognitive Branch
 * (EchoCog/opencog-central) which uses a self-contained gncOwner.h API.
 *
 * Original: libgnucash/engine/test/gtest-organization.cpp
 * Source commits: 16eb2e3797 through 9c0cc98ec9
 *
 * 46 test cases covering:
 * - Creation and destruction lifecycle
 * - Property getters and setters
 * - Entity management (add, remove, count)
 * - Null pointer safety
 * - String edge cases (empty, long, special chars, Unicode)
 * - Organization hierarchy
 * - Tensor operations
 * - Edit state management
 */

#include <glib.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "gncOwner.h"

// ============================================================
// Test Fixture
// ============================================================

class GncOrganizationTest : public ::testing::Test {
  // clang-format off
  protected:  // NOLINT(whitespace/indent)
  // clang-format on
  void SetUp() override {
    org = gncOrganizationCreate(nullptr);
    ASSERT_NE(nullptr, org);
  }

  void TearDown() override {
    if (org) {
      gncOrganizationDestroy(org);
      org = nullptr;
    }
  }

  GncOrganization *org = nullptr;
};

// ============================================================
// Basic Creation/Destruction Tests
// ============================================================

TEST_F(GncOrganizationTest, CreateOrganization) {
  EXPECT_NE(nullptr, org);

  // Check default values
  EXPECT_NE(nullptr, gncOrganizationGetGUID(org));
  EXPECT_TRUE(gncOrganizationGetActive(org));
}

TEST_F(GncOrganizationTest, CreateWithNullBook) {
  // In the unified branch, Create with NULL book still succeeds
  // (book param is gpointer and may be ignored in standalone mode)
  GncOrganization *o = gncOrganizationCreate(nullptr);
  EXPECT_NE(nullptr, o);
  gncOrganizationDestroy(o);
}

TEST_F(GncOrganizationTest, DestroyNullOrganization) {
  // Should not crash
  gncOrganizationDestroy(nullptr);
}

// ============================================================
// Setter/Getter Tests
// ============================================================

TEST_F(GncOrganizationTest, SetAndGetID) {
  const char *test_id = "ORG-12345";
  gncOrganizationSetID(org, test_id);

  const char *retrieved_id = gncOrganizationGetID(org);
  EXPECT_STREQ(test_id, retrieved_id);
}

TEST_F(GncOrganizationTest, SetIDWithNull) {
  gncOrganizationSetID(org, nullptr);
  // Should not crash; getter should return something non-crashy
}

TEST_F(GncOrganizationTest, SetAndGetName) {
  const char *test_name = "ACME Corporation";
  gncOrganizationSetName(org, test_name);

  const char *retrieved_name = gncOrganizationGetName(org);
  EXPECT_STREQ(test_name, retrieved_name);
}

TEST_F(GncOrganizationTest, SetAndGetNotes) {
  const char *test_notes =
      "This is a test organization for unit testing purposes.";
  gncOrganizationSetNotes(org, test_notes);

  const char *retrieved_notes = gncOrganizationGetNotes(org);
  EXPECT_STREQ(test_notes, retrieved_notes);
}

TEST_F(GncOrganizationTest, SetAndGetActive) {
  EXPECT_TRUE(gncOrganizationGetActive(org));

  gncOrganizationSetActive(org, FALSE);
  EXPECT_FALSE(gncOrganizationGetActive(org));

  gncOrganizationSetActive(org, TRUE);
  EXPECT_TRUE(gncOrganizationGetActive(org));
}

TEST_F(GncOrganizationTest, SetAndGetCurrency) {
  gncOrganizationSetCurrency(org, "EUR");
  EXPECT_STREQ("EUR", gncOrganizationGetCurrency(org));

  gncOrganizationSetCurrency(org, "ZAR");
  EXPECT_STREQ("ZAR", gncOrganizationGetCurrency(org));
}

TEST_F(GncOrganizationTest, SetAndGetAddr1) {
  gncOrganizationSetAddr1(org, "123 Main St");
  EXPECT_STREQ("123 Main St", gncOrganizationGetAddr1(org));
}

TEST_F(GncOrganizationTest, SetAndGetAddr2) {
  gncOrganizationSetAddr2(org, "Suite 100");
  EXPECT_STREQ("Suite 100", gncOrganizationGetAddr2(org));
}

TEST_F(GncOrganizationTest, SetAndGetAddr3) {
  gncOrganizationSetAddr3(org, "City, State 12345");
  EXPECT_STREQ("City, State 12345", gncOrganizationGetAddr3(org));
}

TEST_F(GncOrganizationTest, SetAndGetAddr4) {
  gncOrganizationSetAddr4(org, "Country");
  EXPECT_STREQ("Country", gncOrganizationGetAddr4(org));
}

TEST_F(GncOrganizationTest, SetAndGetPhone) {
  gncOrganizationSetPhone(org, "+1-555-123-4567");
  EXPECT_STREQ("+1-555-123-4567", gncOrganizationGetPhone(org));
}

TEST_F(GncOrganizationTest, SetAndGetFax) {
  gncOrganizationSetFax(org, "+1-555-123-4568");
  EXPECT_STREQ("+1-555-123-4568", gncOrganizationGetFax(org));
}

TEST_F(GncOrganizationTest, SetAndGetEmail) {
  gncOrganizationSetEmail(org, "org@example.com");
  EXPECT_STREQ("org@example.com", gncOrganizationGetEmail(org));
}

TEST_F(GncOrganizationTest, GettersWithNullOrg) {
  EXPECT_EQ(nullptr, gncOrganizationGetGUID(nullptr));
  EXPECT_EQ(nullptr, gncOrganizationGetName(nullptr));
  EXPECT_EQ(nullptr, gncOrganizationGetID(nullptr));
}

TEST_F(GncOrganizationTest, SettersWithNullOrg) {
  // Should not crash
  gncOrganizationSetName(nullptr, "Test");
  gncOrganizationSetID(nullptr, "ID");
  gncOrganizationSetNotes(nullptr, "Notes");
  gncOrganizationSetActive(nullptr, TRUE);
}

TEST_F(GncOrganizationTest, UpdateIDMultipleTimes) {
  gncOrganizationSetID(org, "ID-1");
  EXPECT_STREQ("ID-1", gncOrganizationGetID(org));

  gncOrganizationSetID(org, "ID-2");
  EXPECT_STREQ("ID-2", gncOrganizationGetID(org));

  gncOrganizationSetID(org, "ID-3");
  EXPECT_STREQ("ID-3", gncOrganizationGetID(org));
}

TEST_F(GncOrganizationTest, SetSameValueTwice) {
  gncOrganizationSetName(org, "Test Corp");
  gncOrganizationSetName(org, "Test Corp");
  EXPECT_STREQ("Test Corp", gncOrganizationGetName(org));
}

// ============================================================
// String Edge Cases
// ============================================================

TEST_F(GncOrganizationTest, SetEmptyStrings) {
  gncOrganizationSetName(org, "");
  EXPECT_STREQ("", gncOrganizationGetName(org));

  gncOrganizationSetNotes(org, "");
  EXPECT_STREQ("", gncOrganizationGetNotes(org));
}

TEST_F(GncOrganizationTest, SetLongStrings) {
  std::string long_name(1000, 'A');
  gncOrganizationSetName(org, long_name.c_str());
  EXPECT_STREQ(long_name.c_str(), gncOrganizationGetName(org));
}

TEST_F(GncOrganizationTest, SetSpecialCharacters) {
  gncOrganizationSetName(org, "Org & Co. <Ltd> \"Partners\"");
  EXPECT_STREQ("Org & Co. <Ltd> \"Partners\"", gncOrganizationGetName(org));
}

TEST_F(GncOrganizationTest, SetUnicodeStrings) {
  gncOrganizationSetName(org, "日本語テスト");
  EXPECT_STREQ("日本語テスト", gncOrganizationGetName(org));
}

// ============================================================
// Entity Management Tests
// ============================================================

TEST_F(GncOrganizationTest, AddSingleEntity) {
  gchar *entity = g_strdup("test_entity");
  gncOrganizationAddEntity(org, entity);

  GList *entities = gncOrganizationGetEntities(org);
  EXPECT_NE(nullptr, entities);
  EXPECT_EQ(1u, g_list_length(entities));

  g_free(entity);
}

TEST_F(GncOrganizationTest, AddMultipleEntities) {
  std::vector<gchar *> ents;
  for (int i = 0; i < 5; i++) {
    gchar *e = g_strdup_printf("entity_%d", i);
    gncOrganizationAddEntity(org, e);
    ents.push_back(e);
  }

  GList *entities = gncOrganizationGetEntities(org);
  EXPECT_EQ(5u, g_list_length(entities));

  for (auto *e : ents)
    g_free(e);
}

TEST_F(GncOrganizationTest, AddDuplicateEntity) {
  gchar *entity = g_strdup("dup_entity");

  gncOrganizationAddEntity(org, entity);
  gncOrganizationAddEntity(org, entity);

  // Duplicate prevention: should still be 1
  EXPECT_EQ(1u, g_list_length(gncOrganizationGetEntities(org)));

  g_free(entity);
}

TEST_F(GncOrganizationTest, RemoveEntity) {
  gchar *entity = g_strdup("to_remove");
  gncOrganizationAddEntity(org, entity);
  EXPECT_EQ(1u, g_list_length(gncOrganizationGetEntities(org)));

  gncOrganizationRemoveEntity(org, entity);
  EXPECT_EQ(nullptr, gncOrganizationGetEntities(org));

  g_free(entity);
}

TEST_F(GncOrganizationTest, RemoveNonexistentEntity) {
  gchar *entity = g_strdup("present");
  gchar *absent = g_strdup("absent");

  gncOrganizationAddEntity(org, entity);
  gncOrganizationRemoveEntity(org, absent);

  // original entity still present
  EXPECT_EQ(1u, g_list_length(gncOrganizationGetEntities(org)));

  g_free(entity);
  g_free(absent);
}

TEST_F(GncOrganizationTest, EntityManagementWithNull) {
  // Should not crash
  gncOrganizationAddEntity(nullptr, nullptr);
  gncOrganizationRemoveEntity(nullptr, nullptr);
}

TEST_F(GncOrganizationTest, MixedEntityTypes) {
  gchar *account = g_strdup("account_1");
  gchar *tx = g_strdup("transaction_1");
  gchar *commodity = g_strdup("commodity_1");

  gncOrganizationAddEntity(org, account);
  gncOrganizationAddEntity(org, tx);
  gncOrganizationAddEntity(org, commodity);

  EXPECT_EQ(3u, g_list_length(gncOrganizationGetEntities(org)));

  g_free(account);
  g_free(tx);
  g_free(commodity);
}

// ============================================================
// Organization Hierarchy Tests
// ============================================================

TEST_F(GncOrganizationTest, SetAndGetParent) {
  GncOrganization *parent = gncOrganizationCreate(nullptr);
  gncOrganizationSetName(parent, "Parent Org");

  gncOrganizationSetParent(org, parent);
  EXPECT_EQ(parent, gncOrganizationGetParent(org));

  gncOrganizationDestroy(parent);
}

TEST_F(GncOrganizationTest, AddChild) {
  GncOrganization *child = gncOrganizationCreate(nullptr);
  gncOrganizationSetName(child, "Child Org");

  gncOrganizationAddChild(org, child);

  GList *children = gncOrganizationGetChildren(org);
  EXPECT_NE(nullptr, children);
  EXPECT_EQ(1u, g_list_length(children));
  EXPECT_EQ(child, g_list_first(children)->data);

  gncOrganizationDestroy(child);
}

TEST_F(GncOrganizationTest, RemoveChild) {
  GncOrganization *child = gncOrganizationCreate(nullptr);

  gncOrganizationAddChild(org, child);
  EXPECT_EQ(1u, g_list_length(gncOrganizationGetChildren(org)));

  gncOrganizationRemoveChild(org, child);
  EXPECT_EQ(nullptr, gncOrganizationGetChildren(org));

  gncOrganizationDestroy(child);
}

TEST_F(GncOrganizationTest, MultipleChildren) {
  GncOrganization *c1 = gncOrganizationCreate(nullptr);
  GncOrganization *c2 = gncOrganizationCreate(nullptr);
  GncOrganization *c3 = gncOrganizationCreate(nullptr);

  gncOrganizationAddChild(org, c1);
  gncOrganizationAddChild(org, c2);
  gncOrganizationAddChild(org, c3);

  EXPECT_EQ(3u, g_list_length(gncOrganizationGetChildren(org)));

  gncOrganizationDestroy(c1);
  gncOrganizationDestroy(c2);
  gncOrganizationDestroy(c3);
}

TEST_F(GncOrganizationTest, SelfAsParentRejected) {
  gncOrganizationSetParent(org, org);
  EXPECT_EQ(nullptr, gncOrganizationGetParent(org));
}

TEST_F(GncOrganizationTest, SelfAsChildRejected) {
  gncOrganizationAddChild(org, org);
  EXPECT_EQ(nullptr, gncOrganizationGetChildren(org));
}

// ============================================================
// Tensor Operations Tests
// ============================================================

TEST_F(GncOrganizationTest, SetGetTensorDimension) {
  gncOrganizationSetTensorDimension(org, 5);
  EXPECT_EQ(5u, gncOrganizationGetTensorDimension(org));
}

TEST_F(GncOrganizationTest, SetGetTensorData) {
  gdouble data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
  gncOrganizationSetTensorDimension(org, 5);
  gncOrganizationSetTensorData(org, data, 5);

  gdouble *retrieved = gncOrganizationGetTensorData(org);
  ASSERT_NE(nullptr, retrieved);

  for (int i = 0; i < 5; i++) {
    EXPECT_DOUBLE_EQ(data[i], retrieved[i]);
  }
}

TEST_F(GncOrganizationTest, SetNullTensorData) {
  gdouble data[] = {1.0, 2.0, 3.0};
  gncOrganizationSetTensorData(org, data, 3);
  EXPECT_NE(nullptr, gncOrganizationGetTensorData(org));

  gncOrganizationSetTensorData(org, nullptr, 0);
  EXPECT_EQ(nullptr, gncOrganizationGetTensorData(org));
}

// ============================================================
// Edit State Management Tests
// ============================================================

static gboolean org_edit_done_called = FALSE;
static GncOrganization *org_edit_done_org = nullptr;

static void test_org_edit_done_callback(GncOrganization *o,
                                        gpointer user_data) {
  org_edit_done_called = TRUE;
  org_edit_done_org = o;
}

TEST_F(GncOrganizationTest, BeginCommitEdit) {
  org_edit_done_called = FALSE;
  org_edit_done_org = nullptr;

  gncOrganizationBeginEdit(org, test_org_edit_done_callback, nullptr);
  gncOrganizationSetName(org, "Edited Name");
  gncOrganizationCommitEdit(org);

  EXPECT_TRUE(org_edit_done_called);
  EXPECT_EQ(org, org_edit_done_org);
}

TEST_F(GncOrganizationTest, RollbackEdit) {
  org_edit_done_called = FALSE;

  gncOrganizationBeginEdit(org, test_org_edit_done_callback, nullptr);
  gncOrganizationRollbackEdit(org);

  EXPECT_FALSE(org_edit_done_called);
}

TEST_F(GncOrganizationTest, CommitEditWithoutBegin) {
  // Should not crash or call callback
  org_edit_done_called = FALSE;
  gncOrganizationCommitEdit(org);
  EXPECT_FALSE(org_edit_done_called);
}

TEST_F(GncOrganizationTest, NullCallbackOnDone) {
  gncOrganizationBeginEdit(org, nullptr, nullptr);
  gncOrganizationCommitEdit(org);
  // Should not crash
}

// ============================================================
// QOF Integration Tests
// ============================================================

TEST_F(GncOrganizationTest, HasValidGUID) {
  const GncGUID *guid = gncOrganizationGetGUID(org);
  ASSERT_NE(nullptr, guid);

  // Verify it's a non-zero GUID
  gboolean all_zero = TRUE;
  for (int i = 0; i < 16; i++) {
    if (guid->data[i] != 0) {
      all_zero = FALSE;
      break;
    }
  }
  EXPECT_FALSE(all_zero);
}

TEST_F(GncOrganizationTest, MultipleOrganizationsHaveUniqueGUIDs) {
  GncOrganization *org2 = gncOrganizationCreate(nullptr);
  ASSERT_NE(nullptr, org2);

  const GncGUID *guid1 = gncOrganizationGetGUID(org);
  const GncGUID *guid2 = gncOrganizationGetGUID(org2);

  gboolean same = TRUE;
  for (int i = 0; i < 16; i++) {
    if (guid1->data[i] != guid2->data[i]) {
      same = FALSE;
      break;
    }
  }
  EXPECT_FALSE(same);

  gncOrganizationDestroy(org2);
}

// ============================================================
// Complete Lifecycle Test
// ============================================================

TEST_F(GncOrganizationTest, CompleteLifecycle) {
  // Set all properties
  gncOrganizationSetID(org, "ORG-LC-001");
  gncOrganizationSetName(org, "Lifecycle Test Corp");
  gncOrganizationSetAddr1(org, "100 Test Blvd");
  gncOrganizationSetAddr2(org, "Floor 42");
  gncOrganizationSetAddr3(org, "Test City, TS 99999");
  gncOrganizationSetAddr4(org, "Testland");
  gncOrganizationSetPhone(org, "+1-555-999-0000");
  gncOrganizationSetFax(org, "+1-555-999-0001");
  gncOrganizationSetEmail(org, "lifecycle@test.org");
  gncOrganizationSetNotes(org, "Full lifecycle test");
  gncOrganizationSetCurrency(org, "GBP");
  gncOrganizationSetActive(org, TRUE);

  // Verify all properties
  EXPECT_STREQ("ORG-LC-001", gncOrganizationGetID(org));
  EXPECT_STREQ("Lifecycle Test Corp", gncOrganizationGetName(org));
  EXPECT_STREQ("100 Test Blvd", gncOrganizationGetAddr1(org));
  EXPECT_STREQ("Floor 42", gncOrganizationGetAddr2(org));
  EXPECT_STREQ("Test City, TS 99999", gncOrganizationGetAddr3(org));
  EXPECT_STREQ("Testland", gncOrganizationGetAddr4(org));
  EXPECT_STREQ("+1-555-999-0000", gncOrganizationGetPhone(org));
  EXPECT_STREQ("+1-555-999-0001", gncOrganizationGetFax(org));
  EXPECT_STREQ("lifecycle@test.org", gncOrganizationGetEmail(org));
  EXPECT_STREQ("Full lifecycle test", gncOrganizationGetNotes(org));
  EXPECT_STREQ("GBP", gncOrganizationGetCurrency(org));
  EXPECT_TRUE(gncOrganizationGetActive(org));

  // Add entities
  gchar *e1 = g_strdup("entity_1");
  gchar *e2 = g_strdup("entity_2");
  gncOrganizationAddEntity(org, e1);
  gncOrganizationAddEntity(org, e2);
  EXPECT_EQ(2u, g_list_length(gncOrganizationGetEntities(org)));

  // Set tensor
  gdouble tensor[] = {1.0, 2.0, 3.0};
  gncOrganizationSetTensorDimension(org, 3);
  gncOrganizationSetTensorData(org, tensor, 3);
  EXPECT_EQ(3u, gncOrganizationGetTensorDimension(org));

  g_free(e1);
  g_free(e2);
}

// ============================================================
// Scale Tests
// ============================================================

TEST_F(GncOrganizationTest, AddManyEntities) {
  std::vector<gchar *> entities;
  for (int i = 0; i < 1000; i++) {
    gchar *e = g_strdup_printf("entity_%d", i);
    gncOrganizationAddEntity(org, e);
    entities.push_back(e);
  }

  EXPECT_EQ(1000u, g_list_length(gncOrganizationGetEntities(org)));

  for (auto *e : entities)
    g_free(e);
}

TEST_F(GncOrganizationTest, MultipleOrganizationsInBook) {
  // Create multiple organizations and verify they are independent
  std::vector<GncOrganization *> orgs;
  for (int i = 0; i < 10; i++) {
    GncOrganization *o = gncOrganizationCreate(nullptr);
    ASSERT_NE(nullptr, o);
    gchar *tmp_name = g_strdup_printf("Org %d", i);
    gncOrganizationSetName(o, tmp_name);
    g_free(tmp_name);
    orgs.push_back(o);
  }

  // Verify all have unique GUIDs
  for (size_t i = 0; i < orgs.size(); i++) {
    for (size_t j = i + 1; j < orgs.size(); j++) {
      const GncGUID *g1 = gncOrganizationGetGUID(orgs[i]);
      const GncGUID *g2 = gncOrganizationGetGUID(orgs[j]);
      gboolean same = TRUE;
      for (int k = 0; k < 16; k++) {
        if (g1->data[k] != g2->data[k]) {
          same = FALSE;
          break;
        }
      }
      EXPECT_FALSE(same);
    }
  }

  for (auto *o : orgs)
    gncOrganizationDestroy(o);
}
