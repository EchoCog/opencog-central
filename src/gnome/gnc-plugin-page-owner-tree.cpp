/**
 * @file gnc-plugin-page-owner-tree.cpp
 * @brief GnuCash Owner Tree Plugin Page - Multi-Entity Cognitive Extension
 * 
 * This file implements the UI plugin page for displaying and managing
 * owner entities in GnuCash. It includes support for the new
 * GNC_OWNER_ORGANIZATION entity type.
 * 
 * Part of the CoGnuCash Unified Cognitive Branch.
 * Cherry-picked from rzonedevops/gnucashmulti PRs 1-15 (Sep 2025)
 * 
 * @author CoGnuCash Contributors
 * @copyright GPL v2+
 */

#include <string.h>
#include <glib.h>
#include "../libgnucash/engine/gncOwner.h"

/* ============================================================ */
/* Type Definitions                                              */
/* ============================================================ */

/**
 * @brief Plugin page structure for owner tree
 */
typedef struct {
    gpointer parent_instance;
    GncOwnerType owner_type;
    gpointer tree_view;
    gpointer book;
    gboolean show_inactive;
} GncPluginPageOwnerTree;

/**
 * @brief Owner tree model entry
 */
typedef struct {
    GncOwnerType type;
    gpointer owner;
    gchar *name;
    gchar *id;
    gboolean active;
} OwnerTreeEntry;

/* ============================================================ */
/* Owner Type String Conversion                                  */
/* ============================================================ */

/**
 * @brief Get display name for owner type
 * @param type Owner type
 * @return Display name string
 */
const gchar *
gnc_owner_type_get_label(GncOwnerType type)
{
    switch (type) {
        case GNC_OWNER_NONE:
        case GNC_OWNER_UNDEFINED:
            return "None";
        case GNC_OWNER_CUSTOMER:
            return "Customer";
        case GNC_OWNER_JOB:
            return "Job";
        case GNC_OWNER_VENDOR:
            return "Vendor";
        case GNC_OWNER_EMPLOYEE:
            return "Employee";
        case GNC_OWNER_ORGANIZATION:
            return "Organization";
        default:
            return "Unknown";
    }
}

/**
 * @brief Get icon name for owner type
 * @param type Owner type
 * @return Icon name string
 */
const gchar *
gnc_owner_type_get_icon(GncOwnerType type)
{
    switch (type) {
        case GNC_OWNER_NONE:
        case GNC_OWNER_UNDEFINED:
            return "gnc-owner-none";
        case GNC_OWNER_CUSTOMER:
            return "gnc-owner-customer";
        case GNC_OWNER_JOB:
            return "gnc-owner-job";
        case GNC_OWNER_VENDOR:
            return "gnc-owner-vendor";
        case GNC_OWNER_EMPLOYEE:
            return "gnc-owner-employee";
        case GNC_OWNER_ORGANIZATION:
            return "gnc-owner-organization";
        default:
            return "gnc-owner-unknown";
    }
}

/**
 * @brief Get menu action path for owner type
 * @param type Owner type
 * @return Menu path string
 */
const gchar *
gnc_owner_type_get_menu_path(GncOwnerType type)
{
    switch (type) {
        case GNC_OWNER_NONE:
        case GNC_OWNER_UNDEFINED:
            return NULL;
        case GNC_OWNER_CUSTOMER:
            return "/menubar/Business/CustomerMenu";
        case GNC_OWNER_JOB:
            return "/menubar/Business/JobMenu";
        case GNC_OWNER_VENDOR:
            return "/menubar/Business/VendorMenu";
        case GNC_OWNER_EMPLOYEE:
            return "/menubar/Business/EmployeeMenu";
        case GNC_OWNER_ORGANIZATION:
            return "/menubar/Business/OrganizationMenu";
        default:
            return NULL;
    }
}

/* ============================================================ */
/* Owner Type Validation                                         */
/* ============================================================ */

/**
 * @brief Check if owner type supports invoices
 * @param type Owner type
 * @return TRUE if type supports invoices
 */
gboolean
gnc_owner_type_has_invoices(GncOwnerType type)
{
    switch (type) {
        case GNC_OWNER_NONE:
        case GNC_OWNER_UNDEFINED:
            return FALSE;
        case GNC_OWNER_CUSTOMER:
            return TRUE;
        case GNC_OWNER_JOB:
            return TRUE;
        case GNC_OWNER_VENDOR:
            return TRUE;
        case GNC_OWNER_EMPLOYEE:
            return TRUE;
        case GNC_OWNER_ORGANIZATION:
            return TRUE;
        default:
            return FALSE;
    }
}

/**
 * @brief Check if owner type supports jobs
 * @param type Owner type
 * @return TRUE if type supports jobs
 */
gboolean
gnc_owner_type_has_jobs(GncOwnerType type)
{
    switch (type) {
        case GNC_OWNER_NONE:
        case GNC_OWNER_UNDEFINED:
            return FALSE;
        case GNC_OWNER_CUSTOMER:
            return TRUE;
        case GNC_OWNER_JOB:
            return FALSE;
        case GNC_OWNER_VENDOR:
            return TRUE;
        case GNC_OWNER_EMPLOYEE:
            return FALSE;
        case GNC_OWNER_ORGANIZATION:
            return TRUE;
        default:
            return FALSE;
    }
}

/**
 * @brief Check if owner type can be a parent in hierarchy
 * @param type Owner type
 * @return TRUE if type can have children
 */
gboolean
gnc_owner_type_can_have_children(GncOwnerType type)
{
    switch (type) {
        case GNC_OWNER_NONE:
        case GNC_OWNER_UNDEFINED:
            return FALSE;
        case GNC_OWNER_CUSTOMER:
            return TRUE;
        case GNC_OWNER_JOB:
            return FALSE;
        case GNC_OWNER_VENDOR:
            return TRUE;
        case GNC_OWNER_EMPLOYEE:
            return FALSE;
        case GNC_OWNER_ORGANIZATION:
            return TRUE;
        default:
            return FALSE;
    }
}

/* ============================================================ */
/* Owner Data Access                                             */
/* ============================================================ */

/**
 * @brief Get owner name from owner structure
 * @param owner Owner structure
 * @return Name string (do not free)
 */
const gchar *
gnc_owner_get_name(const GncOwner *owner)
{
    if (!owner) return NULL;
    
    switch (owner->type) {
        case GNC_OWNER_NONE:
        case GNC_OWNER_UNDEFINED:
            return NULL;
        case GNC_OWNER_CUSTOMER:
            return NULL;
        case GNC_OWNER_JOB:
            return NULL;
        case GNC_OWNER_VENDOR:
            return NULL;
        case GNC_OWNER_EMPLOYEE:
            return NULL;
        case GNC_OWNER_ORGANIZATION:
            return gncOrganizationGetName(owner->owner.organization);
        default:
            return NULL;
    }
}

/**
 * @brief Get owner ID from owner structure
 * @param owner Owner structure
 * @return ID string (do not free)
 */
const gchar *
gnc_owner_get_id(const GncOwner *owner)
{
    if (!owner) return NULL;
    
    switch (owner->type) {
        case GNC_OWNER_NONE:
        case GNC_OWNER_UNDEFINED:
            return NULL;
        case GNC_OWNER_CUSTOMER:
            return NULL;
        case GNC_OWNER_JOB:
            return NULL;
        case GNC_OWNER_VENDOR:
            return NULL;
        case GNC_OWNER_EMPLOYEE:
            return NULL;
        case GNC_OWNER_ORGANIZATION:
            return gncOrganizationGetID(owner->owner.organization);
        default:
            return NULL;
    }
}

/**
 * @brief Check if owner is active
 * @param owner Owner structure
 * @return TRUE if active
 */
gboolean
gnc_owner_get_active(const GncOwner *owner)
{
    if (!owner) return FALSE;
    
    switch (owner->type) {
        case GNC_OWNER_NONE:
        case GNC_OWNER_UNDEFINED:
            return FALSE;
        case GNC_OWNER_CUSTOMER:
            return TRUE;
        case GNC_OWNER_JOB:
            return TRUE;
        case GNC_OWNER_VENDOR:
            return TRUE;
        case GNC_OWNER_EMPLOYEE:
            return TRUE;
        case GNC_OWNER_ORGANIZATION:
            return gncOrganizationGetActive(owner->owner.organization);
        default:
            return FALSE;
    }
}

/* ============================================================ */
/* Owner Tree Model Operations                                   */
/* ============================================================ */

/**
 * @brief Create tree model entry from owner
 * @param owner Owner to create entry from
 * @return Newly allocated entry (caller owns)
 */
static OwnerTreeEntry *
owner_tree_entry_new(const GncOwner *owner)
{
    if (!owner) return NULL;
    
    OwnerTreeEntry *entry = g_new0(OwnerTreeEntry, 1);
    entry->type = owner->type;
    entry->owner = (gpointer)owner;
    entry->name = g_strdup(gnc_owner_get_name(owner));
    entry->id = g_strdup(gnc_owner_get_id(owner));
    entry->active = gnc_owner_get_active(owner);
    
    return entry;
}

/**
 * @brief Free tree model entry
 * @param entry Entry to free
 */
static void
owner_tree_entry_free(OwnerTreeEntry *entry)
{
    if (!entry) return;
    
    g_free(entry->name);
    g_free(entry->id);
    g_free(entry);
}

/* ============================================================ */
/* Plugin Page Creation                                          */
/* ============================================================ */

/**
 * @brief Create a new owner tree plugin page
 * @param owner_type Type of owners to display
 * @return New plugin page
 */
GncPluginPageOwnerTree *
gnc_plugin_page_owner_tree_new(GncOwnerType owner_type)
{
    GncPluginPageOwnerTree *page;
    
    page = g_new0(GncPluginPageOwnerTree, 1);
    page->owner_type = owner_type;
    page->tree_view = NULL;
    page->book = NULL;
    page->show_inactive = FALSE;
    
    return page;
}

/**
 * @brief Destroy owner tree plugin page
 * @param page Page to destroy
 */
void
gnc_plugin_page_owner_tree_destroy(GncPluginPageOwnerTree *page)
{
    if (!page) return;
    g_free(page);
}

/**
 * @brief Get owner type from plugin page
 * @param page Plugin page
 * @return Owner type
 */
GncOwnerType
gnc_plugin_page_owner_tree_get_owner_type(GncPluginPageOwnerTree *page)
{
    if (!page) return GNC_OWNER_NONE;
    return page->owner_type;
}

/**
 * @brief Set show inactive flag
 * @param page Plugin page
 * @param show_inactive Whether to show inactive owners
 */
void
gnc_plugin_page_owner_tree_set_show_inactive(GncPluginPageOwnerTree *page,
                                              gboolean show_inactive)
{
    if (!page) return;
    page->show_inactive = show_inactive;
}

/* ============================================================ */
/* Owner Type Actions                                            */
/* ============================================================ */

/**
 * @brief Get action entries for owner type
 * @param type Owner type
 * @param n_entries Output: number of entries
 * @return Array of action entries
 */
const gchar **
gnc_owner_type_get_actions(GncOwnerType type, gint *n_entries)
{
    static const gchar *customer_actions[] = {
        "CustomerNewAction",
        "CustomerFindAction",
        "CustomerEditAction",
        "CustomerDeleteAction",
        NULL
    };
    
    static const gchar *vendor_actions[] = {
        "VendorNewAction",
        "VendorFindAction",
        "VendorEditAction",
        "VendorDeleteAction",
        NULL
    };
    
    static const gchar *employee_actions[] = {
        "EmployeeNewAction",
        "EmployeeFindAction",
        "EmployeeEditAction",
        "EmployeeDeleteAction",
        NULL
    };
    
    static const gchar *job_actions[] = {
        "JobNewAction",
        "JobFindAction",
        "JobEditAction",
        "JobDeleteAction",
        NULL
    };
    
    static const gchar *organization_actions[] = {
        "OrganizationNewAction",
        "OrganizationFindAction",
        "OrganizationEditAction",
        "OrganizationDeleteAction",
        "OrganizationHierarchyAction",
        "OrganizationTensorAction",
        NULL
    };
    
    if (n_entries) *n_entries = 0;
    
    switch (type) {
        case GNC_OWNER_NONE:
        case GNC_OWNER_UNDEFINED:
            return NULL;
        case GNC_OWNER_CUSTOMER:
            if (n_entries) *n_entries = 4;
            return customer_actions;
        case GNC_OWNER_JOB:
            if (n_entries) *n_entries = 4;
            return job_actions;
        case GNC_OWNER_VENDOR:
            if (n_entries) *n_entries = 4;
            return vendor_actions;
        case GNC_OWNER_EMPLOYEE:
            if (n_entries) *n_entries = 4;
            return employee_actions;
        case GNC_OWNER_ORGANIZATION:
            if (n_entries) *n_entries = 6;
            return organization_actions;
        default:
            return NULL;
    }
}

/* ============================================================ */
/* Organization-Specific UI Actions                              */
/* ============================================================ */

/**
 * @brief Open organization hierarchy dialog
 * @param org Organization to show hierarchy for
 */
void
gnc_organization_show_hierarchy(GncOrganization *org)
{
    if (!org) return;
    
    GncOrganization *parent = gncOrganizationGetParent(org);
    GList *children = gncOrganizationGetChildren(org);
    
    g_print("Organization Hierarchy for: %s\n", 
            gncOrganizationGetName(org) ? gncOrganizationGetName(org) : "(unnamed)");
    
    if (parent) {
        g_print("  Parent: %s\n", 
                gncOrganizationGetName(parent) ? gncOrganizationGetName(parent) : "(unnamed)");
    }
    
    if (children) {
        g_print("  Children (%d):\n", g_list_length(children));
        for (GList *l = children; l != NULL; l = l->next) {
            GncOrganization *child = (GncOrganization *)l->data;
            g_print("    - %s\n", 
                    gncOrganizationGetName(child) ? gncOrganizationGetName(child) : "(unnamed)");
        }
    }
}

/**
 * @brief Open organization tensor view dialog
 * @param org Organization to show tensor data for
 */
void
gnc_organization_show_tensor(GncOrganization *org)
{
    if (!org) return;
    
    guint dimension = gncOrganizationGetTensorDimension(org);
    gdouble *data = gncOrganizationGetTensorData(org);
    
    g_print("Organization Tensor for: %s\n", 
            gncOrganizationGetName(org) ? gncOrganizationGetName(org) : "(unnamed)");
    g_print("  Dimension: %u\n", dimension);
    
    if (data && dimension > 0) {
        g_print("  Data: [");
        for (guint i = 0; i < dimension && i < 10; i++) {
            g_print("%.4f", data[i]);
            if (i < dimension - 1 && i < 9) g_print(", ");
        }
        if (dimension > 10) g_print(", ...");
        g_print("]\n");
    } else {
        g_print("  Data: (not set)\n");
    }
}

/* ============================================================ */
/* Report Generation                                             */
/* ============================================================ */

/**
 * @brief Get report type for owner
 * @param type Owner type
 * @return Report type string
 */
const gchar *
gnc_owner_type_get_report(GncOwnerType type)
{
    switch (type) {
        case GNC_OWNER_NONE:
        case GNC_OWNER_UNDEFINED:
            return NULL;
        case GNC_OWNER_CUSTOMER:
            return "customer-report";
        case GNC_OWNER_JOB:
            return "job-report";
        case GNC_OWNER_VENDOR:
            return "vendor-report";
        case GNC_OWNER_EMPLOYEE:
            return "employee-report";
        case GNC_OWNER_ORGANIZATION:
            return "organization-report";
        default:
            return NULL;
    }
}

/**
 * @brief Generate report for organization
 * @param org Organization to report on
 * @param include_children Whether to include child organizations
 * @param include_entities Whether to include owned entities
 */
void
gnc_organization_generate_report(GncOrganization *org,
                                  gboolean include_children,
                                  gboolean include_entities)
{
    if (!org) return;
    
    g_print("\n=== Organization Report ===\n");
    g_print("Name: %s\n", gncOrganizationGetName(org) ? gncOrganizationGetName(org) : "(unnamed)");
    g_print("ID: %s\n", gncOrganizationGetID(org) ? gncOrganizationGetID(org) : "(none)");
    g_print("Active: %s\n", gncOrganizationGetActive(org) ? "Yes" : "No");
    g_print("Currency: %s\n", gncOrganizationGetCurrency(org) ? gncOrganizationGetCurrency(org) : "USD");
    
    if (include_children) {
        GList *children = gncOrganizationGetChildren(org);
        g_print("\nChild Organizations: %d\n", children ? g_list_length(children) : 0);
        
        for (GList *l = children; l != NULL; l = l->next) {
            GncOrganization *child = (GncOrganization *)l->data;
            g_print("  - %s (%s)\n", 
                    gncOrganizationGetName(child) ? gncOrganizationGetName(child) : "(unnamed)",
                    gncOrganizationGetID(child) ? gncOrganizationGetID(child) : "(no id)");
        }
    }
    
    if (include_entities) {
        GList *entities = gncOrganizationGetEntities(org);
        g_print("\nOwned Entities: %d\n", entities ? g_list_length(entities) : 0);
    }
    
    guint tensor_dim = gncOrganizationGetTensorDimension(org);
    if (tensor_dim > 0) {
        g_print("\nTensor Dimension: %u\n", tensor_dim);
    }
    
    g_print("=== End Report ===\n\n");
}

/* ============================================================ */
/* Module Initialization                                         */
/* ============================================================ */

/**
 * @brief Initialize owner tree plugin module
 */
void
gnc_plugin_page_owner_tree_init(void)
{
    gncOrganizationRegister();
}

/**
 * @brief Finalize owner tree plugin module
 */
void
gnc_plugin_page_owner_tree_finalize(void)
{
}
