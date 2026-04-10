/**
 * @file gncOwner.h
 * @brief GnuCash Owner Entity Types - Multi-Entity Cognitive Extension
 * 
 * This file defines the GncOwner structure and related entity types for
 * the GnuCash multi-entity cognitive accounting system. It extends the
 * standard GnuCash owner types with GNC_OWNER_ORGANIZATION for supporting
 * multi-entity organizational structures.
 * 
 * Part of the CoGnuCash Unified Cognitive Branch.
 * Cherry-picked from rzonedevops/gnucashmulti PRs 1-15 (Sep 2025)
 * 
 * @author CoGnuCash Contributors
 * @copyright GPL v2+
 */

#ifndef GNC_OWNER_H_
#define GNC_OWNER_H_

#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @brief QOF ID type for owners
 */
#define GNC_ID_OWNER "gncOwner"

/**
 * @brief QOF ID type for organizations
 */
#define GNC_ID_ORGANIZATION "gncOrganization"

/**
 * @brief Forward declarations
 */
typedef struct _GncOwner GncOwner;
typedef struct _GncOrganization GncOrganization;
typedef struct _GncCustomer GncCustomer;
typedef struct _GncEmployee GncEmployee;
typedef struct _GncVendor GncVendor;
typedef struct _GncJob GncJob;

/**
 * @brief Enumeration of owner types in GnuCash
 * 
 * This enumeration defines all possible owner types that can be associated
 * with transactions and accounts. The GNC_OWNER_ORGANIZATION type was added
 * to support multi-entity accounting structures for cognitive agents.
 */
typedef enum {
    GNC_OWNER_NONE = 0,          /**< No owner type */
    GNC_OWNER_UNDEFINED = 0,     /**< Undefined owner (alias for NONE) */
    GNC_OWNER_CUSTOMER,          /**< Customer owner type */
    GNC_OWNER_JOB,               /**< Job owner type */
    GNC_OWNER_VENDOR,            /**< Vendor owner type */
    GNC_OWNER_EMPLOYEE,          /**< Employee owner type */
    GNC_OWNER_ORGANIZATION       /**< Organization owner type (Layer 0 cognitive) */
} GncOwnerType;

/**
 * @brief Structure representing an owner in GnuCash
 * 
 * The GncOwner structure is a union-like type that can represent
 * any of the owner subtypes (Customer, Job, Vendor, Employee, Organization).
 */
struct _GncOwner {
    GncOwnerType type;           /**< The type of this owner */
    union {
        GncCustomer     *customer;
        GncJob          *job;
        GncVendor       *vendor;
        GncEmployee     *employee;
        GncOrganization *organization;
    } owner;
};

/**
 * @brief GUID type for entity identification
 */
typedef struct {
    guchar data[16];
} GncGUID;

/**
 * @brief Structure representing an organization entity
 * 
 * GncOrganization is the new owner type added as Layer 0 for the
 * multi-entity cognitive branch. It represents a legal entity or
 * organizational unit that can own accounts, transactions, and
 * other financial objects.
 * 
 * This type is essential for:
 * - Multi-entity consolidation
 * - Organizational hypergraph construction
 * - Cognitive agent entity reasoning
 */
struct _GncOrganization {
    GncGUID guid;                /**< Unique identifier */
    gchar *id;                   /**< Human-readable ID */
    gchar *name;                 /**< Organization name */
    gchar *addr1;                /**< Address line 1 */
    gchar *addr2;                /**< Address line 2 */
    gchar *addr3;                /**< Address line 3 */
    gchar *addr4;                /**< Address line 4 */
    gchar *phone;                /**< Phone number */
    gchar *fax;                  /**< Fax number */
    gchar *email;                /**< Email address */
    gchar *notes;                /**< Notes/comments */
    gchar *tax_table_override;   /**< Tax table override */
    gchar *currency;             /**< Default currency */
    gboolean active;             /**< Whether organization is active */
    gint32 version;              /**< Object version for dirty tracking */
    
    /* Multi-entity tensor fields */
    guint dimension;             /**< Tensor rank for multi-entity operations */
    gdouble *tensor_data;        /**< Multi-dimensional financial data */
    
    /* Organizational hierarchy */
    GncOrganization *parent;     /**< Parent organization (NULL if root) */
    GList *children;             /**< List of child organizations */
    GList *entities;             /**< List of owned entities (accounts, etc.) */
};

/* ============================================================ */
/* Owner Functions                                               */
/* ============================================================ */

/**
 * @brief Initialize an owner structure
 * @param owner Pointer to owner structure to initialize
 */
void gncOwnerInitUndefined(GncOwner *owner);

/**
 * @brief Initialize an owner from a customer
 * @param owner Pointer to owner structure
 * @param customer Pointer to customer to set
 */
void gncOwnerInitCustomer(GncOwner *owner, GncCustomer *customer);

/**
 * @brief Initialize an owner from a vendor
 * @param owner Pointer to owner structure
 * @param vendor Pointer to vendor to set
 */
void gncOwnerInitVendor(GncOwner *owner, GncVendor *vendor);

/**
 * @brief Initialize an owner from an employee
 * @param owner Pointer to owner structure
 * @param employee Pointer to employee to set
 */
void gncOwnerInitEmployee(GncOwner *owner, GncEmployee *employee);

/**
 * @brief Initialize an owner from a job
 * @param owner Pointer to owner structure
 * @param job Pointer to job to set
 */
void gncOwnerInitJob(GncOwner *owner, GncJob *job);

/**
 * @brief Initialize an owner from an organization
 * @param owner Pointer to owner structure
 * @param organization Pointer to organization to set
 */
void gncOwnerInitOrganization(GncOwner *owner, GncOrganization *organization);

/**
 * @brief Get the type of an owner
 * @param owner Pointer to owner structure
 * @return The GncOwnerType of this owner
 */
GncOwnerType gncOwnerGetType(const GncOwner *owner);

/**
 * @brief Get a human-readable type name for the owner
 * @param owner Pointer to owner structure
 * @return String representation of owner type
 */
const gchar *gncOwnerGetTypeString(const GncOwner *owner);

/**
 * @brief Check if owner type is valid
 * @param type Owner type to check
 * @return TRUE if valid, FALSE otherwise
 */
gboolean gncOwnerTypeIsValid(GncOwnerType type);

/**
 * @brief Get the organization from an owner
 * @param owner Pointer to owner structure
 * @return Pointer to organization, or NULL if not an organization owner
 */
GncOrganization *gncOwnerGetOrganization(const GncOwner *owner);

/**
 * @brief Copy an owner structure
 * @param dest Destination owner
 * @param src Source owner
 */
void gncOwnerCopy(GncOwner *dest, const GncOwner *src);

/**
 * @brief Compare two owners for equality
 * @param a First owner
 * @param b Second owner
 * @return TRUE if equal, FALSE otherwise
 */
gboolean gncOwnerEqual(const GncOwner *a, const GncOwner *b);

/* ============================================================ */
/* Organization CRUD Functions                                   */
/* ============================================================ */

/**
 * @brief Create a new organization
 * @param book QOF Book to create organization in
 * @return Newly created organization, or NULL on failure
 */
GncOrganization *gncOrganizationCreate(gpointer book);

/**
 * @brief Destroy an organization
 * @param org Organization to destroy
 */
void gncOrganizationDestroy(GncOrganization *org);

/**
 * @brief Get the GUID of an organization
 * @param org Organization
 * @return Pointer to GUID
 */
const GncGUID *gncOrganizationGetGUID(const GncOrganization *org);

/**
 * @brief Set the ID of an organization
 * @param org Organization
 * @param id New ID
 */
void gncOrganizationSetID(GncOrganization *org, const gchar *id);

/**
 * @brief Get the ID of an organization
 * @param org Organization
 * @return Organization ID
 */
const gchar *gncOrganizationGetID(const GncOrganization *org);

/**
 * @brief Set the name of an organization
 * @param org Organization
 * @param name New name
 */
void gncOrganizationSetName(GncOrganization *org, const gchar *name);

/**
 * @brief Get the name of an organization
 * @param org Organization
 * @return Organization name
 */
const gchar *gncOrganizationGetName(const GncOrganization *org);

/**
 * @brief Set address line 1
 * @param org Organization
 * @param addr Address line 1
 */
void gncOrganizationSetAddr1(GncOrganization *org, const gchar *addr);

/**
 * @brief Get address line 1
 * @param org Organization
 * @return Address line 1
 */
const gchar *gncOrganizationGetAddr1(const GncOrganization *org);

/**
 * @brief Set address line 2
 * @param org Organization
 * @param addr Address line 2
 */
void gncOrganizationSetAddr2(GncOrganization *org, const gchar *addr);

/**
 * @brief Get address line 2
 * @param org Organization
 * @return Address line 2
 */
const gchar *gncOrganizationGetAddr2(const GncOrganization *org);

/**
 * @brief Set address line 3
 * @param org Organization
 * @param addr Address line 3
 */
void gncOrganizationSetAddr3(GncOrganization *org, const gchar *addr);

/**
 * @brief Get address line 3
 * @param org Organization
 * @return Address line 3
 */
const gchar *gncOrganizationGetAddr3(const GncOrganization *org);

/**
 * @brief Set address line 4
 * @param org Organization
 * @param addr Address line 4
 */
void gncOrganizationSetAddr4(GncOrganization *org, const gchar *addr);

/**
 * @brief Get address line 4
 * @param org Organization
 * @return Address line 4
 */
const gchar *gncOrganizationGetAddr4(const GncOrganization *org);

/**
 * @brief Set phone number
 * @param org Organization
 * @param phone Phone number
 */
void gncOrganizationSetPhone(GncOrganization *org, const gchar *phone);

/**
 * @brief Get phone number
 * @param org Organization
 * @return Phone number
 */
const gchar *gncOrganizationGetPhone(const GncOrganization *org);

/**
 * @brief Set fax number
 * @param org Organization
 * @param fax Fax number
 */
void gncOrganizationSetFax(GncOrganization *org, const gchar *fax);

/**
 * @brief Get fax number
 * @param org Organization
 * @return Fax number
 */
const gchar *gncOrganizationGetFax(const GncOrganization *org);

/**
 * @brief Set email address
 * @param org Organization
 * @param email Email address
 */
void gncOrganizationSetEmail(GncOrganization *org, const gchar *email);

/**
 * @brief Get email address
 * @param org Organization
 * @return Email address
 */
const gchar *gncOrganizationGetEmail(const GncOrganization *org);

/**
 * @brief Set notes
 * @param org Organization
 * @param notes Notes text
 */
void gncOrganizationSetNotes(GncOrganization *org, const gchar *notes);

/**
 * @brief Get notes
 * @param org Organization
 * @return Notes text
 */
const gchar *gncOrganizationGetNotes(const GncOrganization *org);

/**
 * @brief Set active status
 * @param org Organization
 * @param active Active status
 */
void gncOrganizationSetActive(GncOrganization *org, gboolean active);

/**
 * @brief Get active status
 * @param org Organization
 * @return TRUE if active, FALSE otherwise
 */
gboolean gncOrganizationGetActive(const GncOrganization *org);

/**
 * @brief Set currency
 * @param org Organization
 * @param currency Currency code
 */
void gncOrganizationSetCurrency(GncOrganization *org, const gchar *currency);

/**
 * @brief Get currency
 * @param org Organization
 * @return Currency code
 */
const gchar *gncOrganizationGetCurrency(const GncOrganization *org);

/* ============================================================ */
/* Organization Hierarchy Functions                              */
/* ============================================================ */

/**
 * @brief Set parent organization
 * @param org Child organization
 * @param parent Parent organization (NULL for root)
 */
void gncOrganizationSetParent(GncOrganization *org, GncOrganization *parent);

/**
 * @brief Get parent organization
 * @param org Organization
 * @return Parent organization, or NULL if root
 */
GncOrganization *gncOrganizationGetParent(const GncOrganization *org);

/**
 * @brief Add a child organization
 * @param org Parent organization
 * @param child Child organization to add
 */
void gncOrganizationAddChild(GncOrganization *org, GncOrganization *child);

/**
 * @brief Remove a child organization
 * @param org Parent organization
 * @param child Child organization to remove
 */
void gncOrganizationRemoveChild(GncOrganization *org, GncOrganization *child);

/**
 * @brief Get list of child organizations
 * @param org Parent organization
 * @return GList of child organizations
 */
GList *gncOrganizationGetChildren(const GncOrganization *org);

/**
 * @brief Add an entity to the organization
 * @param org Organization
 * @param entity Entity to add (account, transaction, etc.)
 */
void gncOrganizationAddEntity(GncOrganization *org, gpointer entity);

/**
 * @brief Remove an entity from the organization
 * @param org Organization
 * @param entity Entity to remove
 */
void gncOrganizationRemoveEntity(GncOrganization *org, gpointer entity);

/**
 * @brief Get list of entities owned by organization
 * @param org Organization
 * @return GList of entities
 */
GList *gncOrganizationGetEntities(const GncOrganization *org);

/* ============================================================ */
/* Multi-Entity Tensor Functions                                 */
/* ============================================================ */

/**
 * @brief Set tensor dimension for multi-entity operations
 * @param org Organization
 * @param dimension Tensor rank
 */
void gncOrganizationSetTensorDimension(GncOrganization *org, guint dimension);

/**
 * @brief Get tensor dimension
 * @param org Organization
 * @return Tensor dimension
 */
guint gncOrganizationGetTensorDimension(const GncOrganization *org);

/**
 * @brief Set tensor data for multi-entity operations
 * @param org Organization
 * @param data Tensor data array
 * @param size Size of data array
 */
void gncOrganizationSetTensorData(GncOrganization *org, gdouble *data, gsize size);

/**
 * @brief Get tensor data
 * @param org Organization
 * @return Pointer to tensor data
 */
gdouble *gncOrganizationGetTensorData(const GncOrganization *org);

/* ============================================================ */
/* QOF Registration                                              */
/* ============================================================ */

/**
 * @brief Register Organization with QOF
 * 
 * This function registers the GncOrganization type with the QOF
 * (Query Object Framework) so it can be queried, persisted, and
 * manipulated like other GnuCash business objects.
 */
void gncOrganizationRegister(void);

/**
 * @brief Callback type for organization completion
 * 
 * Fixed null-pointer bug (Feb 2026) where GncOrganizationOnDone
 * was not properly initialized.
 */
typedef void (*GncOrganizationOnDone)(GncOrganization *org, gpointer user_data);

/**
 * @brief Begin organization edit with callback on completion
 * @param org Organization to edit
 * @param on_done Callback when editing is done (may be NULL)
 * @param user_data User data for callback
 */
void gncOrganizationBeginEdit(GncOrganization *org, 
                              GncOrganizationOnDone on_done,
                              gpointer user_data);

/**
 * @brief Commit organization edit
 * @param org Organization to commit
 */
void gncOrganizationCommitEdit(GncOrganization *org);

/**
 * @brief Rollback organization edit
 * @param org Organization to rollback
 */
void gncOrganizationRollbackEdit(GncOrganization *org);

#ifdef __cplusplus
}
#endif

#endif /* GNC_OWNER_H_ */
