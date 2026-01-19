
#ifndef TEST_H
#define TEST_H
/* 
╰┭━╾┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅╼━┮╮
╭╯ Tests § datastructures → functional                                      ╭╯╿
╙╼━╾┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄━━╪*/
      
/* Checks that the deque is created in a valid initial state
   (non-NULL structure, coherent fields, empty but usable). */
bool is_initialized_properly(void *);

/* Verifies that the deque owns a mandatory internal buffer or storage area
   after initialization. */
bool has_mandatory_data_buffer(void *);

/* Checks that the deque supports dynamic resizing when capacity is exceeded. */
bool is_resizable(void *);

/* Verifies that all elements can be removed and the deque returns to an empty,
   consistent state. */
bool can_be_emptied(void *);

/* Checks that the deque can store a very large amount of data without
   corruption or undefined behavior. */
bool can_store_huge_data(void *);

/* Verifies that the deque properly refuses insertions when memory or capacity
   limits are reached, without breaking its invariants. */
bool properly_refuses_to_store_too_much(void *);

/* Checks that clearing or destroying the deque effectively erases its content
   and leaves no accessible stale data. */
bool gets_erased_properly(void *);

/* 
╰┭━╾┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅╼━┮╮
╭╯ Tests § datastructures → failures                                        ╭╯╿
╙╼━╾┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄━━╪*/
  
/* Verifies that all resources owned by the deque are released when the program
   exits or the deque is destroyed. */
bool is_freed_on_program_exit(void *);

/* Checks that the deque reports errors in a controlled and consistent way
   instead of continuing in an invalid state. */
bool properly_fails(void *);

/* Verifies that the deque remains usable after recoverable errors and does not
   enter a corrupted internal state. */
bool can_survive_errors(void *);

/* Ensures that normal or erroneous use of the deque never directly causes a
   crash (segfault, abort, UB). */
bool is_never_an_inherent_cause_of_crash(void *);

/* Checks that dangerous situations (near capacity, invalid operations, etc.)
   produce a warning or a detectable error signal for the user. */
bool warns_user_when_in_danger(void *);


/* 
╰┭━╾┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅╼━┮╮
╭╯ Tests § datastructures → benchmarks                                      ╭╯╿
╙╼━╾┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄━━╪*/
  

/* Measures the performance of random or sequential access operations on the
   deque. */
bool benchmark_access(void *, void *);

/* Measures the performance of element removal operations
   (pop front, pop back, bulk removals). */
bool benchmark_removal(void *, void *);

/* Measures the cost of resizing operations and capacity growth strategies. */
bool benchmark_resize(void *, void *);

/* Measures the performance of inserting and storing large amounts of data. */
bool benchmark_storage(void *, void *);

/* Runs randomized mixed operations (insert, remove, resize, access) to evaluate
   average performance and stress the deque under realistic workloads. */
bool benchmark_fuzzy_operations(void *, void *);

#endif
