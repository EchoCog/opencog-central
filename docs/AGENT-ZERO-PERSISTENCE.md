# Agent-Zero Hypergraph State Persistence

This document describes the hypergraph state persistence implementation for Agent-Zero Genesis cognitive kernels.

## Overview

The hypergraph state persistence system enables saving and restoring cognitive kernel states including:
- Tensor field shapes and encodings
- Attention allocation weights  
- Meta-cognitive levels
- AtomSpace data representations

## Quick Start

```scheme
;; Load modules
(use-modules (agent-zero kernel)
             (agent-zero persistence))

;; Create a cognitive kernel
(define kernel (spawn-cognitive-kernel '(64 32) 0.75))

;; Save the kernel state
(define saved (save-hypergraph-state kernel "my-kernel-state"))

;; Load the kernel state later
(define loaded (load-hypergraph-state "my-kernel-state"))
(define restored-kernel (persistent-kernel-kernel loaded))
```

## API Reference

### Core Persistence Functions

#### `save-hypergraph-state kernel state-id`
Saves a cognitive kernel's hypergraph state to persistent storage.
- `kernel`: The cognitive kernel to save
- `state-id`: Unique identifier for the saved state
- Returns: `persistent-kernel` record with metadata

#### `load-hypergraph-state state-id`
Loads a hypergraph state from persistent storage.
- `state-id`: Identifier of the state to load
- Returns: `persistent-kernel` record with restored kernel

#### `serialize-hypergraph-state kernel`
Serializes hypergraph state to a data structure for persistence.
- `kernel`: The cognitive kernel to serialize
- Returns: Serialized state data structure

#### `deserialize-hypergraph-state serialized-data`
Deserializes hypergraph state data to restore a kernel.
- `serialized-data`: Previously serialized state data
- Returns: Restored cognitive kernel

### Convenience Functions

#### `save-kernel-state kernel state-id`
Convenience function to save a kernel's complete state.

#### `load-kernel-state state-id`
Convenience function to load a kernel's complete state.

### State Management

#### `list-saved-states`
Lists all saved hypergraph state identifiers.
- Returns: List of state IDs

#### `delete-saved-state state-id`
Deletes a saved hypergraph state.
- `state-id`: ID of state to delete
- Returns: `#t` if successful, `#f` if state not found

### Bulk Operations

#### `export-hypergraph-states state-ids export-file`
Exports multiple states to a single file.
- `state-ids`: List of state IDs to export
- `export-file`: File path for export

#### `import-hypergraph-states import-file`
Imports multiple states from a file.
- `import-file`: File path to import from
- Returns: List of imported state IDs

## Data Format

Hypergraph states are saved as S-expressions with the following structure:

```scheme
((version . "1.0")
 (timestamp . <unix-timestamp>)
 (kernel-metadata . ((type . agent-zero-cognitive-kernel)
                     (creation-timestamp . <timestamp>)))
 (hypergraph-state . ((atomspace . <atomspace-data>)
                      (tensor-shape . <shape-list>)
                      (attention . <attention-weight>)
                      (meta-level . <meta-level>)))
 (tensor-encoding . <encoding-data>)
 (hypergraph-tensor-encoding . <hypergraph-encoding>)
 (cognitive-metadata . <metadata>))
```

## Storage Location

States are stored in `~/.agent-zero/states/` by default. Each state is saved as:
- File: `<state-id>.hgstate`
- Format: Pretty-printed S-expressions

## Integration with Kernel Module

The persistence module integrates with the kernel module through:

```scheme
;; Set up persistence integration
(set-persistence-functions! save-kernel-state load-kernel-state)

;; Use integrated functions
(save-kernel-to-state kernel "state-id")
(load-kernel-from-state "state-id")
```

## Persistent Kernel Records

The `persistent-kernel` record type tracks metadata:

```scheme
(define-record-type <persistent-kernel>
  (make-persistent-kernel kernel state-id save-timestamp load-timestamp)
  persistent-kernel?
  (kernel persistent-kernel-kernel)
  (state-id persistent-kernel-state-id)
  (save-timestamp persistent-kernel-save-timestamp)
  (load-timestamp persistent-kernel-load-timestamp))
```

## Error Handling

- Loading non-existent states throws an error
- Version mismatches are detected and reported
- File system errors are propagated to the caller

## Examples

### Basic Save/Load Cycle

```scheme
;; Create and configure kernel
(define kernel (spawn-cognitive-kernel '(128 64) 0.8))

;; Save state
(save-hypergraph-state kernel "experiment-1")

;; Load state later
(define loaded (load-hypergraph-state "experiment-1"))
(define restored (persistent-kernel-kernel loaded))

;; Verify integrity
(equal? (kernel-tensor-shape kernel) 
        (kernel-tensor-shape restored)) ; => #t
```

### State Management

```scheme
;; List all saved states
(list-saved-states) ; => ("experiment-1" "test-state" "backup")

;; Clean up old states
(delete-saved-state "old-experiment")
```

### Bulk Operations

```scheme
;; Export multiple states
(export-hypergraph-states '("exp-1" "exp-2") "/tmp/experiments.hgstates")

;; Import states on another system
(import-hypergraph-states "/tmp/experiments.hgstates")
```

## Testing

Run persistence tests with:

```bash
# In Guix environment
guile tests/agent-zero/persistence-tests.scm

# Or run simple test
guile tests/agent-zero/test-persistence-simple.scm
```

## Future Enhancements

- Compression for large states
- Encryption for sensitive cognitive data
- Remote persistence backends
- Incremental state updates
- Automatic versioning and migration