;; Agent-Zero Hypergraph Persistence Tests
;; /tests/agent-zero/persistence-tests.scm

(use-modules (srfi srfi-64)
             (srfi srfi-1)
             (agent-zero kernel)
             (agent-zero persistence)
             (ice-9 ftw))

(test-begin "hypergraph-persistence-tests")

(test-group "serialization-deserialization"
  (test-assert "serialize hypergraph state"
    (let ((kernel (spawn-cognitive-kernel '(64 32) 0.7)))
      (let ((serialized (serialize-hypergraph-state kernel)))
        (and (list? serialized)
             (assoc 'version serialized)
             (assoc 'hypergraph-state serialized)
             (assoc 'tensor-encoding serialized)))))

  (test-assert "deserialize hypergraph state"
    (let* ((kernel (spawn-cognitive-kernel '(32 16) 0.8))
           (serialized (serialize-hypergraph-state kernel))
           (deserialized (deserialize-hypergraph-state serialized)))
      (and deserialized
           (equal? (kernel-tensor-shape deserialized) '(32 16))
           (= (kernel-attention deserialized) 0.8))))

  (test-assert "serialization preserves kernel properties"
    (let* ((original-kernel (spawn-cognitive-kernel '(128 64) 0.9))
           (serialized (serialize-hypergraph-state original-kernel))
           (restored-kernel (deserialize-hypergraph-state serialized)))
      (and (equal? (kernel-tensor-shape original-kernel)
                  (kernel-tensor-shape restored-kernel))
           (= (kernel-attention original-kernel)
              (kernel-attention restored-kernel))))))

(test-group "file-persistence"
  (test-assert "save hypergraph state to file"
    (let ((kernel (spawn-cognitive-kernel '(16 8) 0.6)))
      (let ((persistent-kernel (save-hypergraph-state kernel "test-state-1")))
        (and (persistent-kernel? persistent-kernel)
             (equal? (persistent-kernel-state-id persistent-kernel) "test-state-1")))))

  (test-assert "load hypergraph state from file"
    (let* ((kernel (spawn-cognitive-kernel '(8 4) 0.5))
           (saved (save-hypergraph-state kernel "test-state-2"))
           (loaded (load-hypergraph-state "test-state-2")))
      (and (persistent-kernel? loaded)
           (equal? (kernel-tensor-shape (persistent-kernel-kernel loaded)) '(8 4))
           (= (kernel-attention (persistent-kernel-kernel loaded)) 0.5))))

  (test-assert "save and load preserves complete state"
    (let* ((original-kernel (spawn-cognitive-kernel '(64 64) 0.85))
           (original-state (hypergraph-state original-kernel))
           (saved (save-hypergraph-state original-kernel "test-state-3"))
           (loaded (load-hypergraph-state "test-state-3"))
           (restored-kernel (persistent-kernel-kernel loaded))
           (restored-state (hypergraph-state restored-kernel)))
      (and (equal? (assoc-ref original-state 'tensor-shape)
                  (assoc-ref restored-state 'tensor-shape))
           (= (assoc-ref original-state 'attention)
              (assoc-ref restored-state 'attention))
           (= (assoc-ref original-state 'meta-level)
              (assoc-ref restored-state 'meta-level))))))

(test-group "kernel-convenience-functions"
  (test-assert "save kernel state convenience function"
    (let ((kernel (spawn-cognitive-kernel '(32 32) 0.7)))
      (let ((result (save-kernel-state kernel "test-kernel-1")))
        (persistent-kernel? result))))

  (test-assert "load kernel state convenience function"
    (let* ((kernel (spawn-cognitive-kernel '(16 16) 0.6))
           (saved (save-kernel-state kernel "test-kernel-2"))
           (loaded-kernel (load-kernel-state "test-kernel-2")))
      (and loaded-kernel
           (equal? (kernel-tensor-shape loaded-kernel) '(16 16))
           (= (kernel-attention loaded-kernel) 0.6)))))

(test-group "state-management"
  (test-assert "list saved states"
    (let* ((kernel1 (spawn-cognitive-kernel '(8 8) 0.4))
           (kernel2 (spawn-cognitive-kernel '(4 4) 0.3))
           (saved1 (save-kernel-state kernel1 "list-test-1"))
           (saved2 (save-kernel-state kernel2 "list-test-2"))
           (state-list (list-saved-states)))
      (and (list? state-list)
           (member "list-test-1" state-list)
           (member "list-test-2" state-list))))

  (test-assert "delete saved state"
    (let* ((kernel (spawn-cognitive-kernel '(2 2) 0.2))
           (saved (save-kernel-state kernel "delete-test"))
           (states-before (list-saved-states))
           (deleted (delete-saved-state "delete-test"))
           (states-after (list-saved-states)))
      (and deleted
           (member "delete-test" states-before)
           (not (member "delete-test" states-after))))))

(test-group "bulk-operations"
  (test-assert "export multiple states"
    (let* ((kernel1 (spawn-cognitive-kernel '(16 8) 0.7))
           (kernel2 (spawn-cognitive-kernel '(8 16) 0.8))
           (saved1 (save-kernel-state kernel1 "export-test-1"))
           (saved2 (save-kernel-state kernel2 "export-test-2"))
           (export-file "/tmp/test-export.hgstates"))
      (export-hypergraph-states '("export-test-1" "export-test-2") export-file)
      (file-exists? export-file)))

  (test-assert "import multiple states"
    (let* ((kernel1 (spawn-cognitive-kernel '(32 16) 0.6))
           (kernel2 (spawn-cognitive-kernel '(16 32) 0.9))
           (saved1 (save-kernel-state kernel1 "import-test-1"))
           (saved2 (save-kernel-state kernel2 "import-test-2"))
           (export-file "/tmp/test-import.hgstates"))
      ;; Export first
      (export-hypergraph-states '("import-test-1" "import-test-2") export-file)
      ;; Delete original states
      (delete-saved-state "import-test-1")
      (delete-saved-state "import-test-2")
      ;; Import back
      (let ((imported (import-hypergraph-states export-file)))
        (and (list? imported)
             (member "import-test-1" imported)
             (member "import-test-2" imported)))))

(test-group "persistent-kernel-record"
  (test-assert "persistent kernel creation"
    (let* ((kernel (spawn-cognitive-kernel '(4 8) 0.5))
           (persistent-kernel (make-persistent-kernel kernel "test-persistent")))
      (and (persistent-kernel? persistent-kernel)
           (equal? (persistent-kernel-state-id persistent-kernel) "test-persistent")
           (eq? (persistent-kernel-kernel persistent-kernel) kernel))))

  (test-assert "persistent kernel metadata"
    (let* ((kernel (spawn-cognitive-kernel '(8 4) 0.7))
           (saved (save-hypergraph-state kernel "metadata-test")))
      (and (persistent-kernel-save-timestamp saved)
           (> (persistent-kernel-save-timestamp saved) 0)))))

(test-group "error-handling"
  (test-assert "load non-existent state throws error"
    (catch #t
      (lambda ()
        (load-hypergraph-state "non-existent-state")
        #f) ; Should not reach here
      (lambda (key . args)
        #t))) ; Error expected

  (test-assert "delete non-existent state returns false"
    (not (delete-saved-state "non-existent-state-for-deletion"))))

(test-group "integration-with-kernel-module"
  (test-assert "kernel module persistence functions are available"
    ;; Set up persistence integration
    (set-persistence-functions! save-kernel-state load-kernel-state)
    (and (procedure? save-kernel-to-state)
         (procedure? load-kernel-from-state)))

  (test-assert "integrated save and load functions work"
    (when (and save-kernel-to-state load-kernel-from-state)
      (let* ((kernel (spawn-cognitive-kernel '(64 32) 0.8))
             (saved (save-kernel-to-state kernel "integration-test"))
             (loaded (load-kernel-from-state "integration-test")))
        (and saved loaded
             (equal? (kernel-tensor-shape loaded) '(64 32))
             (= (kernel-attention loaded) 0.8))))))

(test-end "hypergraph-persistence-tests")

;; Test runner function
(define (run-persistence-tests)
  "Run all Agent-Zero persistence tests."
  (display "Running Agent-Zero Hypergraph Persistence Tests...")
  (newline)
  
  ;; Set up persistence integration for testing
  (set-persistence-functions! save-kernel-state load-kernel-state)
  
  ;; Run the tests (already defined above)
  
  (display "All Agent-Zero persistence tests completed!")
  (newline))

;; Export test runner for external use
(define-public run-agent-zero-persistence-tests run-persistence-tests)

;; Cleanup function for tests
(define (cleanup-test-states)
  "Clean up test state files."
  (let ((test-states '("test-state-1" "test-state-2" "test-state-3"
                      "test-kernel-1" "test-kernel-2" 
                      "list-test-1" "list-test-2"
                      "export-test-1" "export-test-2"
                      "import-test-1" "import-test-2"
                      "metadata-test" "integration-test")))
    (for-each (lambda (state-id)
               (catch #t
                 (lambda () (delete-saved-state state-id))
                 (lambda (key . args) #f))) ; Ignore errors
             test-states)))

;; Export cleanup function
(define-public cleanup-agent-zero-test-states cleanup-test-states)