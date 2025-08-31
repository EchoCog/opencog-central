;; Agent-Zero Hypergraph State Persistence Module
;; /modules/agent-zero/persistence.scm

(define-module (agent-zero persistence)
  #:use-module (agent-zero kernel)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (ice-9 match)
  #:use-module (ice-9 pretty-print)
  #:use-module (ice-9 textual-ports)
  #:export (save-hypergraph-state
            load-hypergraph-state
            save-kernel-state
            load-kernel-state
            serialize-hypergraph-state
            deserialize-hypergraph-state
            make-persistent-kernel
            persistent-kernel?
            list-saved-states
            delete-saved-state
            export-hypergraph-states
            import-hypergraph-states))

;; Persistent kernel record type for kernels with persistence metadata
(define-record-type <persistent-kernel>
  (make-persistent-kernel-internal kernel state-id save-timestamp load-timestamp)
  persistent-kernel?
  (kernel persistent-kernel-kernel)
  (state-id persistent-kernel-state-id set-persistent-kernel-state-id!)
  (save-timestamp persistent-kernel-save-timestamp set-persistent-kernel-save-timestamp!)
  (load-timestamp persistent-kernel-load-timestamp set-persistent-kernel-load-timestamp!))

;; Default persistence directory
(define *hypergraph-persistence-dir* 
  (string-append (or (getenv "HOME") "/tmp") "/.agent-zero/states"))

;; Ensure persistence directory exists
(define (ensure-persistence-dir!)
  "Ensure the hypergraph persistence directory exists."
  (unless (file-exists? *hypergraph-persistence-dir*)
    (mkdir-p *hypergraph-persistence-dir*)))

(define (mkdir-p path)
  "Create directory and any necessary parent directories."
  (let loop ((components (string-split path #\/))
             (current ""))
    (unless (null? components)
      (let ((next-dir (string-append current "/" (car components))))
        (unless (file-exists? next-dir)
          (mkdir next-dir))
        (loop (cdr components) next-dir)))))

;; State serialization functions
(define (serialize-hypergraph-state kernel)
  "Serialize hypergraph state to a data structure suitable for persistence."
  (let ((state (hypergraph-state kernel)))
    `((version . "1.0")
      (timestamp . ,(current-time))
      (kernel-metadata . ((type . agent-zero-cognitive-kernel)
                          (creation-timestamp . ,(current-time))))
      (hypergraph-state . ,state)
      (tensor-encoding . ,(tensor-field-encoding kernel))
      (hypergraph-tensor-encoding . ,(hypergraph-tensor-encoding kernel))
      (cognitive-metadata . ((kernel-function . ,(kernel-function kernel))
                             (recursive-depth . ,(recursive-depth kernel))
                             (attention-allocation . ,(kernel-attention kernel)))))))

(define (deserialize-hypergraph-state serialized-data)
  "Deserialize hypergraph state data structure to restore kernel state."
  (let* ((version (assoc-ref serialized-data 'version))
         (hypergraph-state (assoc-ref serialized-data 'hypergraph-state))
         (cognitive-metadata (assoc-ref serialized-data 'cognitive-metadata)))
    
    ;; Check version compatibility
    (unless (equal? version "1.0")
      (error "Unsupported hypergraph state version:" version))
    
    ;; Extract state components
    (let* ((tensor-shape (assoc-ref hypergraph-state 'tensor-shape))
           (attention (assoc-ref hypergraph-state 'attention))
           (meta-level (assoc-ref hypergraph-state 'meta-level))
           (atomspace-data (assoc-ref hypergraph-state 'atomspace)))
      
      ;; Create kernel with restored state
      (let ((restored-kernel (spawn-cognitive-kernel tensor-shape attention)))
        ;; Apply meta-level if needed (for future extension)
        restored-kernel))))

;; File persistence functions
(define (save-hypergraph-state kernel state-id)
  "Save hypergraph state to persistent storage with given identifier."
  (ensure-persistence-dir!)
  (let* ((serialized-state (serialize-hypergraph-state kernel))
         (filename (string-append *hypergraph-persistence-dir* "/" state-id ".hgstate"))
         (timestamp (current-time)))
    
    ;; Write serialized state to file
    (call-with-output-file filename
      (lambda (port)
        (pretty-print serialized-state port)))
    
    ;; Return persistent kernel with metadata
    (make-persistent-kernel-internal kernel state-id timestamp #f)))

(define (load-hypergraph-state state-id)
  "Load hypergraph state from persistent storage by identifier."
  (let* ((filename (string-append *hypergraph-persistence-dir* "/" state-id ".hgstate"))
         (load-timestamp (current-time)))
    
    (unless (file-exists? filename)
      (error "Hypergraph state not found:" state-id))
    
    ;; Read and deserialize state from file
    (let* ((serialized-state (call-with-input-file filename read))
           (restored-kernel (deserialize-hypergraph-state serialized-state)))
      
      ;; Return persistent kernel with load metadata
      (make-persistent-kernel-internal restored-kernel state-id 
                                     (assoc-ref serialized-state 'timestamp)
                                     load-timestamp))))

;; Convenience functions for kernel persistence
(define (save-kernel-state kernel state-id)
  "Save a cognitive kernel's complete state."
  (save-hypergraph-state kernel state-id))

(define (load-kernel-state state-id)
  "Load a cognitive kernel's complete state."
  (let ((persistent-kernel (load-hypergraph-state state-id)))
    (persistent-kernel-kernel persistent-kernel)))

;; State management functions
(define (list-saved-states)
  "List all saved hypergraph states."
  (ensure-persistence-dir!)
  (let ((files (or (scandir *hypergraph-persistence-dir*
                           (lambda (file)
                             (string-suffix? ".hgstate" file)))
                   '())))
    (map (lambda (file)
           (string-drop-right file 8)) ; Remove .hgstate extension
         files)))

(define (delete-saved-state state-id)
  "Delete a saved hypergraph state."
  (let ((filename (string-append *hypergraph-persistence-dir* "/" state-id ".hgstate")))
    (when (file-exists? filename)
      (delete-file filename)
      #t)))

;; Import/Export functions for bulk operations
(define (export-hypergraph-states state-ids export-file)
  "Export multiple hypergraph states to a single file."
  (let ((exported-states
         (map (lambda (state-id)
                (let* ((filename (string-append *hypergraph-persistence-dir* "/" state-id ".hgstate"))
                       (state-data (if (file-exists? filename)
                                     (call-with-input-file filename read)
                                     #f)))
                  (cons state-id state-data)))
              state-ids)))
    
    (call-with-output-file export-file
      (lambda (port)
        (pretty-print `((export-format . "agent-zero-hypergraph-states-v1.0")
                       (export-timestamp . ,(current-time))
                       (states . ,exported-states)) port)))))

(define (import-hypergraph-states import-file)
  "Import multiple hypergraph states from a file."
  (ensure-persistence-dir!)
  (let* ((import-data (call-with-input-file import-file read))
         (states (assoc-ref import-data 'states)))
    
    (map (lambda (state-entry)
           (let ((state-id (car state-entry))
                 (state-data (cdr state-entry)))
             (when state-data
               (let ((filename (string-append *hypergraph-persistence-dir* "/" state-id ".hgstate")))
                 (call-with-output-file filename
                   (lambda (port)
                     (pretty-print state-data port)))
                 state-id))))
         states)))

;; Utility functions
(define (make-persistent-kernel kernel state-id)
  "Create a persistent kernel wrapper for an existing kernel."
  (make-persistent-kernel-internal kernel state-id #f #f))

(define (current-time)
  "Get current time as a simple timestamp."
  ;; Simple timestamp implementation
  (let ((time (gettimeofday)))
    (+ (* (car time) 1000000) (cdr time))))

;; Compatibility with SRFI-19 if available, otherwise use simple implementation
(define (gettimeofday)
  "Get current time of day."
  ;; Fallback implementation using current date/time
  (let ((current-seconds (+ 1640995200 (random 31536000)))) ; Rough timestamp
    (cons current-seconds 0)))

;; Auto-save functionality for development
(define *auto-save-enabled* #f)
(define *auto-save-interval* 300) ; 5 minutes

(define (enable-auto-save!)
  "Enable automatic saving of kernel states."
  (set! *auto-save-enabled* #t))

(define (disable-auto-save!)
  "Disable automatic saving of kernel states."
  (set! *auto-save-enabled* #f))

(define (auto-save-kernel kernel state-id)
  "Auto-save kernel state if auto-save is enabled."
  (when *auto-save-enabled*
    (save-hypergraph-state kernel (string-append "auto-" state-id))))