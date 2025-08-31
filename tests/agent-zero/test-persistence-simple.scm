#!/usr/bin/env guile
!#
;; Simple test script for Agent-Zero hypergraph persistence
;; /tests/agent-zero/test-persistence-simple.scm

(add-to-load-path "../../modules")

(use-modules (agent-zero kernel)
             (agent-zero persistence)
             (ice-9 format))

(define (simple-test-persistence)
  "Run simple hypergraph persistence tests."
  (format #t "=== Agent-Zero Hypergraph Persistence Simple Test ===~%")
  
  ;; Test 1: Create a kernel
  (format #t "1. Creating cognitive kernel...~%")
  (define test-kernel (spawn-cognitive-kernel '(64 32) 0.75))
  (format #t "   Kernel created with shape: ~a, attention: ~a~%" 
          (kernel-tensor-shape test-kernel)
          (kernel-attention test-kernel))
  
  ;; Test 2: Get hypergraph state
  (format #t "2. Getting hypergraph state...~%")
  (define state (hypergraph-state test-kernel))
  (format #t "   State keys: ~a~%" (map car state))
  
  ;; Test 3: Serialize state
  (format #t "3. Serializing hypergraph state...~%")
  (define serialized (serialize-hypergraph-state test-kernel))
  (format #t "   Serialized state keys: ~a~%" (map car serialized))
  
  ;; Test 4: Save to file
  (format #t "4. Saving state to file...~%")
  (define persistent-kernel (save-hypergraph-state test-kernel "simple-test"))
  (format #t "   Saved state ID: ~a~%" 
          (persistent-kernel-state-id persistent-kernel))
  
  ;; Test 5: Load from file
  (format #t "5. Loading state from file...~%")
  (define loaded-persistent (load-hypergraph-state "simple-test"))
  (define loaded-kernel (persistent-kernel-kernel loaded-persistent))
  (format #t "   Loaded kernel shape: ~a, attention: ~a~%"
          (kernel-tensor-shape loaded-kernel)
          (kernel-attention loaded-kernel))
  
  ;; Test 6: Verify data integrity
  (format #t "6. Verifying data integrity...~%")
  (define original-shape (kernel-tensor-shape test-kernel))
  (define loaded-shape (kernel-tensor-shape loaded-kernel))
  (define original-attention (kernel-attention test-kernel))
  (define loaded-attention (kernel-attention loaded-kernel))
  
  (if (and (equal? original-shape loaded-shape)
           (= original-attention loaded-attention))
      (format #t "   ✓ Data integrity verified!~%")
      (format #t "   ✗ Data integrity check failed!~%"))
  
  ;; Test 7: List saved states
  (format #t "7. Listing saved states...~%")
  (define saved-states (list-saved-states))
  (format #t "   Found ~a saved states: ~a~%" 
          (length saved-states) saved-states)
  
  ;; Test 8: Clean up
  (format #t "8. Cleaning up test state...~%")
  (define deleted (delete-saved-state "simple-test"))
  (format #t "   Cleanup ~a~%" (if deleted "successful" "failed"))
  
  (format #t "=== Simple persistence test completed ===~%")
  #t)

;; Run the test if this script is executed directly
(when (string=? (car (command-line)) 
                (string-append (getcwd) "/test-persistence-simple.scm"))
  (simple-test-persistence))