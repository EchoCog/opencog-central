/**
 * @file gncOwner.cpp
 * @brief GnuCash Owner Entity Implementation - Multi-Entity Cognitive Extension
 * 
 * Implementation of GncOwner and GncOrganization types for the
 * GnuCash multi-entity cognitive accounting system.
 * 
 * Part of the CoGnuCash Unified Cognitive Branch.
 * Cherry-picked from rzonedevops/gnucashmulti PRs 1-15 (Sep 2025)
 * 
 * @author CoGnuCash Contributors
 * @copyright GPL v2+
 */

#include <string.h>
#include <stdlib.h>
#include "gncOwner.h"

/* ============================================================ */
/* Static Helper Functions                                       */
/* ============================================================ */

static void
generate_guid(GncGUID *guid)
{
    for (int i = 0; i < 16; i++) {
        guid->data[i] = (guchar)(rand() & 0xFF);
    }
    guid->data[6] = (guid->data[6] & 0x0F) | 0x40;
    guid->data[8] = (guid->data[8] & 0x3F) | 0x80;
}

static gchar *
safe_strdup(const gchar *str)
{
    return str ? g_strdup(str) : NULL;
}

static void
safe_free(gchar **str)
{
    if (str && *str) {
        g_free(*str);
        *str = NULL;
    }
}

/* ============================================================ */
/* Owner Functions Implementation                                */
/* ============================================================ */

void
gncOwnerInitUndefined(GncOwner *owner)
{
    if (!owner) return;
    
    owner->type = GNC_OWNER_UNDEFINED;
    owner->owner.customer = NULL;
}

void
gncOwnerInitCustomer(GncOwner *owner, GncCustomer *customer)
{
    if (!owner) return;
    
    owner->type = GNC_OWNER_CUSTOMER;
    owner->owner.customer = customer;
}

void
gncOwnerInitVendor(GncOwner *owner, GncVendor *vendor)
{
    if (!owner) return;
    
    owner->type = GNC_OWNER_VENDOR;
    owner->owner.vendor = vendor;
}

void
gncOwnerInitEmployee(GncOwner *owner, GncEmployee *employee)
{
    if (!owner) return;
    
    owner->type = GNC_OWNER_EMPLOYEE;
    owner->owner.employee = employee;
}

void
gncOwnerInitJob(GncOwner *owner, GncJob *job)
{
    if (!owner) return;
    
    owner->type = GNC_OWNER_JOB;
    owner->owner.job = job;
}

void
gncOwnerInitOrganization(GncOwner *owner, GncOrganization *organization)
{
    if (!owner) return;
    
    owner->type = GNC_OWNER_ORGANIZATION;
    owner->owner.organization = organization;
}

GncOwnerType
gncOwnerGetType(const GncOwner *owner)
{
    if (!owner) return GNC_OWNER_NONE;
    return owner->type;
}

const gchar *
gncOwnerGetTypeString(const GncOwner *owner)
{
    if (!owner) return "None";
    
    switch (owner->type) {
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

gboolean
gncOwnerTypeIsValid(GncOwnerType type)
{
    switch (type) {
        case GNC_OWNER_NONE:
        case GNC_OWNER_CUSTOMER:
        case GNC_OWNER_JOB:
        case GNC_OWNER_VENDOR:
        case GNC_OWNER_EMPLOYEE:
        case GNC_OWNER_ORGANIZATION:
            return TRUE;
        default:
            return FALSE;
    }
}

GncOrganization *
gncOwnerGetOrganization(const GncOwner *owner)
{
    if (!owner) return NULL;
    if (owner->type != GNC_OWNER_ORGANIZATION) return NULL;
    return owner->owner.organization;
}

void
gncOwnerCopy(GncOwner *dest, const GncOwner *src)
{
    if (!dest || !src) return;
    
    dest->type = src->type;
    
    switch (src->type) {
        case GNC_OWNER_NONE:
        case GNC_OWNER_UNDEFINED:
            dest->owner.customer = NULL;
            break;
        case GNC_OWNER_CUSTOMER:
            dest->owner.customer = src->owner.customer;
            break;
        case GNC_OWNER_JOB:
            dest->owner.job = src->owner.job;
            break;
        case GNC_OWNER_VENDOR:
            dest->owner.vendor = src->owner.vendor;
            break;
        case GNC_OWNER_EMPLOYEE:
            dest->owner.employee = src->owner.employee;
            break;
        case GNC_OWNER_ORGANIZATION:
            dest->owner.organization = src->owner.organization;
            break;
    }
}

gboolean
gncOwnerEqual(const GncOwner *a, const GncOwner *b)
{
    if (a == b) return TRUE;
    if (!a || !b) return FALSE;
    if (a->type != b->type) return FALSE;
    
    switch (a->type) {
        case GNC_OWNER_NONE:
        case GNC_OWNER_UNDEFINED:
            return TRUE;
        case GNC_OWNER_CUSTOMER:
            return a->owner.customer == b->owner.customer;
        case GNC_OWNER_JOB:
            return a->owner.job == b->owner.job;
        case GNC_OWNER_VENDOR:
            return a->owner.vendor == b->owner.vendor;
        case GNC_OWNER_EMPLOYEE:
            return a->owner.employee == b->owner.employee;
        case GNC_OWNER_ORGANIZATION:
            return a->owner.organization == b->owner.organization;
        default:
            return FALSE;
    }
}

/* ============================================================ */
/* Organization CRUD Functions Implementation                    */
/* ============================================================ */

GncOrganization *
gncOrganizationCreate(gpointer book)
{
    GncOrganization *org;
    
    org = g_new0(GncOrganization, 1);
    if (!org) return NULL;
    
    generate_guid(&org->guid);
    
    org->id = NULL;
    org->name = NULL;
    org->addr1 = NULL;
    org->addr2 = NULL;
    org->addr3 = NULL;
    org->addr4 = NULL;
    org->phone = NULL;
    org->fax = NULL;
    org->email = NULL;
    org->notes = NULL;
    org->tax_table_override = NULL;
    org->currency = g_strdup("USD");
    org->active = TRUE;
    org->version = 1;
    
    org->dimension = 0;
    org->tensor_data = NULL;
    
    org->parent = NULL;
    org->children = NULL;
    org->entities = NULL;
    
    return org;
}

void
gncOrganizationDestroy(GncOrganization *org)
{
    if (!org) return;
    
    safe_free(&org->id);
    safe_free(&org->name);
    safe_free(&org->addr1);
    safe_free(&org->addr2);
    safe_free(&org->addr3);
    safe_free(&org->addr4);
    safe_free(&org->phone);
    safe_free(&org->fax);
    safe_free(&org->email);
    safe_free(&org->notes);
    safe_free(&org->tax_table_override);
    safe_free(&org->currency);
    
    if (org->tensor_data) {
        g_free(org->tensor_data);
        org->tensor_data = NULL;
    }
    
    if (org->children) {
        g_list_free(org->children);
        org->children = NULL;
    }
    
    if (org->entities) {
        g_list_free(org->entities);
        org->entities = NULL;
    }
    
    g_free(org);
}

const GncGUID *
gncOrganizationGetGUID(const GncOrganization *org)
{
    if (!org) return NULL;
    return &org->guid;
}

void
gncOrganizationSetID(GncOrganization *org, const gchar *id)
{
    if (!org) return;
    safe_free(&org->id);
    org->id = safe_strdup(id);
    org->version++;
}

const gchar *
gncOrganizationGetID(const GncOrganization *org)
{
    if (!org) return NULL;
    return org->id;
}

void
gncOrganizationSetName(GncOrganization *org, const gchar *name)
{
    if (!org) return;
    safe_free(&org->name);
    org->name = safe_strdup(name);
    org->version++;
}

const gchar *
gncOrganizationGetName(const GncOrganization *org)
{
    if (!org) return NULL;
    return org->name;
}

void
gncOrganizationSetAddr1(GncOrganization *org, const gchar *addr)
{
    if (!org) return;
    safe_free(&org->addr1);
    org->addr1 = safe_strdup(addr);
    org->version++;
}

const gchar *
gncOrganizationGetAddr1(const GncOrganization *org)
{
    if (!org) return NULL;
    return org->addr1;
}

void
gncOrganizationSetAddr2(GncOrganization *org, const gchar *addr)
{
    if (!org) return;
    safe_free(&org->addr2);
    org->addr2 = safe_strdup(addr);
    org->version++;
}

const gchar *
gncOrganizationGetAddr2(const GncOrganization *org)
{
    if (!org) return NULL;
    return org->addr2;
}

void
gncOrganizationSetAddr3(GncOrganization *org, const gchar *addr)
{
    if (!org) return;
    safe_free(&org->addr3);
    org->addr3 = safe_strdup(addr);
    org->version++;
}

const gchar *
gncOrganizationGetAddr3(const GncOrganization *org)
{
    if (!org) return NULL;
    return org->addr3;
}

void
gncOrganizationSetAddr4(GncOrganization *org, const gchar *addr)
{
    if (!org) return;
    safe_free(&org->addr4);
    org->addr4 = safe_strdup(addr);
    org->version++;
}

const gchar *
gncOrganizationGetAddr4(const GncOrganization *org)
{
    if (!org) return NULL;
    return org->addr4;
}

void
gncOrganizationSetPhone(GncOrganization *org, const gchar *phone)
{
    if (!org) return;
    safe_free(&org->phone);
    org->phone = safe_strdup(phone);
    org->version++;
}

const gchar *
gncOrganizationGetPhone(const GncOrganization *org)
{
    if (!org) return NULL;
    return org->phone;
}

void
gncOrganizationSetFax(GncOrganization *org, const gchar *fax)
{
    if (!org) return;
    safe_free(&org->fax);
    org->fax = safe_strdup(fax);
    org->version++;
}

const gchar *
gncOrganizationGetFax(const GncOrganization *org)
{
    if (!org) return NULL;
    return org->fax;
}

void
gncOrganizationSetEmail(GncOrganization *org, const gchar *email)
{
    if (!org) return;
    safe_free(&org->email);
    org->email = safe_strdup(email);
    org->version++;
}

const gchar *
gncOrganizationGetEmail(const GncOrganization *org)
{
    if (!org) return NULL;
    return org->email;
}

void
gncOrganizationSetNotes(GncOrganization *org, const gchar *notes)
{
    if (!org) return;
    safe_free(&org->notes);
    org->notes = safe_strdup(notes);
    org->version++;
}

const gchar *
gncOrganizationGetNotes(const GncOrganization *org)
{
    if (!org) return NULL;
    return org->notes;
}

void
gncOrganizationSetActive(GncOrganization *org, gboolean active)
{
    if (!org) return;
    org->active = active;
    org->version++;
}

gboolean
gncOrganizationGetActive(const GncOrganization *org)
{
    if (!org) return FALSE;
    return org->active;
}

void
gncOrganizationSetCurrency(GncOrganization *org, const gchar *currency)
{
    if (!org) return;
    safe_free(&org->currency);
    org->currency = safe_strdup(currency);
    org->version++;
}

const gchar *
gncOrganizationGetCurrency(const GncOrganization *org)
{
    if (!org) return NULL;
    return org->currency;
}

/* ============================================================ */
/* Organization Hierarchy Functions Implementation               */
/* ============================================================ */

void
gncOrganizationSetParent(GncOrganization *org, GncOrganization *parent)
{
    if (!org) return;
    if (org == parent) return;
    
    if (org->parent) {
        gncOrganizationRemoveChild(org->parent, org);
    }
    
    org->parent = parent;
    
    if (parent) {
        gncOrganizationAddChild(parent, org);
    }
    
    org->version++;
}

GncOrganization *
gncOrganizationGetParent(const GncOrganization *org)
{
    if (!org) return NULL;
    return org->parent;
}

void
gncOrganizationAddChild(GncOrganization *org, GncOrganization *child)
{
    if (!org || !child) return;
    if (org == child) return;
    
    if (!g_list_find(org->children, child)) {
        org->children = g_list_append(org->children, child);
        org->version++;
    }
}

void
gncOrganizationRemoveChild(GncOrganization *org, GncOrganization *child)
{
    if (!org || !child) return;
    
    GList *link = g_list_find(org->children, child);
    if (link) {
        org->children = g_list_delete_link(org->children, link);
        org->version++;
    }
}

GList *
gncOrganizationGetChildren(const GncOrganization *org)
{
    if (!org) return NULL;
    return org->children;
}

void
gncOrganizationAddEntity(GncOrganization *org, gpointer entity)
{
    if (!org || !entity) return;
    
    if (!g_list_find(org->entities, entity)) {
        org->entities = g_list_append(org->entities, entity);
        org->version++;
    }
}

void
gncOrganizationRemoveEntity(GncOrganization *org, gpointer entity)
{
    if (!org || !entity) return;
    
    GList *link = g_list_find(org->entities, entity);
    if (link) {
        org->entities = g_list_delete_link(org->entities, link);
        org->version++;
    }
}

GList *
gncOrganizationGetEntities(const GncOrganization *org)
{
    if (!org) return NULL;
    return org->entities;
}

/* ============================================================ */
/* Multi-Entity Tensor Functions Implementation                  */
/* ============================================================ */

void
gncOrganizationSetTensorDimension(GncOrganization *org, guint dimension)
{
    if (!org) return;
    org->dimension = dimension;
    org->version++;
}

guint
gncOrganizationGetTensorDimension(const GncOrganization *org)
{
    if (!org) return 0;
    return org->dimension;
}

void
gncOrganizationSetTensorData(GncOrganization *org, gdouble *data, gsize size)
{
    if (!org) return;
    
    if (org->tensor_data) {
        g_free(org->tensor_data);
    }
    
    if (data && size > 0) {
        org->tensor_data = (gdouble *)g_malloc(size * sizeof(gdouble));
        if (org->tensor_data) {
            memcpy(org->tensor_data, data, size * sizeof(gdouble));
        }
    } else {
        org->tensor_data = NULL;
    }
    
    org->version++;
}

gdouble *
gncOrganizationGetTensorData(const GncOrganization *org)
{
    if (!org) return NULL;
    return org->tensor_data;
}

/* ============================================================ */
/* QOF Registration Implementation                               */
/* ============================================================ */

/**
 * QOF parameter getters/setters for Organization
 */

static gpointer
qof_org_get_guid(gpointer obj)
{
    return (gpointer)gncOrganizationGetGUID((GncOrganization *)obj);
}

static void
qof_org_set_id(gpointer obj, const gchar *value)
{
    gncOrganizationSetID((GncOrganization *)obj, value);
}

static gpointer
qof_org_get_id(gpointer obj)
{
    return (gpointer)gncOrganizationGetID((GncOrganization *)obj);
}

static void
qof_org_set_name(gpointer obj, const gchar *value)
{
    gncOrganizationSetName((GncOrganization *)obj, value);
}

static gpointer
qof_org_get_name(gpointer obj)
{
    return (gpointer)gncOrganizationGetName((GncOrganization *)obj);
}

static void
qof_org_set_active(gpointer obj, gboolean value)
{
    gncOrganizationSetActive((GncOrganization *)obj, value);
}

static gboolean
qof_org_get_active(gpointer obj)
{
    return gncOrganizationGetActive((GncOrganization *)obj);
}

void
gncOrganizationRegister(void)
{
    /**
     * QOF class registration for GncOrganization
     * 
     * This would normally call qof_class_register() with the parameter
     * definitions. The actual registration depends on the QOF library
     * being available.
     * 
     * QofParam organization_params[] = {
     *     { QOF_PARAM_GUID, QOF_TYPE_GUID, qof_org_get_guid, NULL },
     *     { "id", QOF_TYPE_STRING, qof_org_get_id, qof_org_set_id },
     *     { "name", QOF_TYPE_STRING, qof_org_get_name, qof_org_set_name },
     *     { "active", QOF_TYPE_BOOLEAN, qof_org_get_active, qof_org_set_active },
     *     { NULL }
     * };
     * qof_class_register(GNC_ID_ORGANIZATION, NULL, organization_params);
     */
}

/* ============================================================ */
/* Organization Edit Functions Implementation                    */
/* ============================================================ */

/**
 * Edit state structure for tracking in-progress edits
 */
typedef struct {
    GncOrganization *org;
    GncOrganizationOnDone on_done;
    gpointer user_data;
    gboolean in_edit;
} GncOrganizationEditState;

static GHashTable *edit_states = NULL;

static GncOrganizationEditState *
get_edit_state(GncOrganization *org)
{
    if (!edit_states) {
        edit_states = g_hash_table_new_full(g_direct_hash, g_direct_equal, 
                                            NULL, g_free);
    }
    
    GncOrganizationEditState *state = (GncOrganizationEditState *)
        g_hash_table_lookup(edit_states, org);
    
    if (!state) {
        state = g_new0(GncOrganizationEditState, 1);
        state->org = org;
        state->on_done = NULL;
        state->user_data = NULL;
        state->in_edit = FALSE;
        g_hash_table_insert(edit_states, org, state);
    }
    
    return state;
}

void
gncOrganizationBeginEdit(GncOrganization *org, 
                         GncOrganizationOnDone on_done,
                         gpointer user_data)
{
    if (!org) return;
    
    GncOrganizationEditState *state = get_edit_state(org);
    
    state->on_done = on_done;
    state->user_data = user_data;
    state->in_edit = TRUE;
}

void
gncOrganizationCommitEdit(GncOrganization *org)
{
    if (!org) return;
    
    GncOrganizationEditState *state = get_edit_state(org);
    
    if (!state->in_edit) return;
    
    state->in_edit = FALSE;
    
    if (state->on_done) {
        state->on_done(org, state->user_data);
    }
    
    state->on_done = NULL;
    state->user_data = NULL;
}

void
gncOrganizationRollbackEdit(GncOrganization *org)
{
    if (!org) return;
    
    GncOrganizationEditState *state = get_edit_state(org);
    
    state->in_edit = FALSE;
    state->on_done = NULL;
    state->user_data = NULL;
}
