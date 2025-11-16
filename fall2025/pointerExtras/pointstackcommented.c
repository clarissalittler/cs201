// PEDAGOGICAL PURPOSE:
// This program demonstrates pointer-to-pointer usage in the context of a linked list stack.
// Key learning objectives:
// 1. Understanding why we need pointer-to-pointer (struct Node**) for stack operations
// 2. Learning how to modify a pointer itself (not just what it points to)
// 3. Understanding the difference between passing a pointer vs pointer-to-pointer
// 4. Seeing memory reuse behavior with malloc/free
// 5. Understanding struct pointers and the arrow operator (->)
// 6. Learning linked list fundamentals (nodes with next pointers)
// 7. Understanding how to traverse linked structures recursively
// 8. Seeing the relationship between stack operations and memory addresses

#include "stdio.h"
#include "stdlib.h"

// WARNING FROM ORIGINAL CODE:
// "This is the WORST possible stack you could implement."
// "This is only here as a demonstration of pointers to pointers"
//
// WHY IS IT "THE WORST"?
// - No error handling for malloc failures
// - popStack has a bug (see explanation below)
// - No memory cleanup in main
// - Not production-ready code
//
// BUT: It's excellent for learning about pointer-to-pointer!

// STRUCT DEFINITION:
// A Node represents one element in our linked list stack
// Each Node contains:
// 1. v: the integer value stored in this node
// 2. next: a pointer to the next Node in the stack (or NULL if this is the bottom)
struct Node {
  int v;
  struct Node* next;
};

// PUSH OPERATION: Adding to the top of the stack
//
// FUNCTION SIGNATURE ANALYSIS:
// void pushStack(struct Node** s, int v)
//
// WHY struct Node** instead of struct Node*?
// Because we need to MODIFY the pointer itself (s), not just what it points to!
//
// THINK ABOUT IT:
// - If we used struct Node* s, we'd get a COPY of the pointer
// - We could modify what s points to (*s)
// - But we couldn't change where the original pointer points!
// - With struct Node** s, we can change *s (the original pointer)
//
// PARAMETERS:
// - s: pointer to the stack top pointer (allows us to modify stackTop in main)
// - v: the integer value to push onto the stack
void pushStack(struct Node** s, int v){

  // STEP 1: ALLOCATE A NEW NODE
  // malloc allocates heap memory for one struct Node
  // sizeof(struct Node) ensures we get exactly the right amount of memory
  // (typically 16 bytes: 4 for int v + 8 for pointer next + 4 padding on 64-bit)
  //
  // WHY USE malloc?
  // - Stack memory would disappear when function returns
  // - Heap memory persists until we explicitly free() it
  struct Node* h = malloc(sizeof(struct Node));

  // STEP 2: SET THE VALUE
  // h->v means (*h).v (dereference h, then access the v field)
  // The arrow operator (->) is syntactic sugar for pointer dereferencing
  // We store the value v in the new node's v field
  h->v = v;

  // STEP 3: LINK THE NEW NODE TO THE CURRENT STACK
  // *s is the CURRENT top of the stack (could be NULL if stack is empty)
  // We set the new node's next pointer to point to the current top
  // This creates the link: new_node -> old_top -> rest of stack
  h->next = *s;

  // STEP 4: UPDATE THE STACK TOP
  // THIS IS WHY WE NEED struct Node**!
  // *s = h changes what the original pointer (in main) points to
  // Now the stack top points to our new node
  //
  // VISUAL REPRESENTATION:
  // Before: stackTop -> [old top node] -> [other nodes] -> NULL
  // After:  stackTop -> [new node h] -> [old top node] -> [other nodes] -> NULL
  //
  // If we had used struct Node* s instead:
  // - s would be a local copy
  // - s = h would only change the local copy
  // - The original stackTop in main wouldn't change!
  *s = h;
}

// POP OPERATION: Removing from the top of the stack
//
// FUNCTION SIGNATURE ANALYSIS:
// void popStack(struct Node** s, int* r)
//
// WHY struct Node** s?
// - Same reason as push: we need to modify the stack top pointer
// - After popping, stackTop should point to the next node
//
// WHY int* r?
// - We need to return the popped value to the caller
// - C doesn't have pass-by-reference, so we use a pointer
// - We can modify *r to store the returned value
//
// PARAMETERS:
// - s: pointer to the stack top pointer
// - r: pointer to store the return value
void popStack(struct Node** s, int* r){

  // CHECK IF STACK IS EMPTY
  // *s is the current stack top
  // If *s == NULL, the stack is empty (nothing to pop)
  if(*s == NULL){

    // BUG IN THIS CODE!
    // Setting r = NULL changes the LOCAL POINTER r
    // It does NOT set *r (the value the caller can see)
    // This should probably be something like:
    //   *r = -1; or return an error code
    // But since this is a demo, the bug is left in
    //
    // WHAT REALLY HAPPENS:
    // r becomes NULL (locally), then function returns
    // The caller's variable is unchanged (contains garbage or old value)
    r = NULL;
  }
  else{

    // STEP 1: SAVE A POINTER TO THE SECOND NODE
    // (*s)->next means: dereference s to get the top node, then get its next pointer
    // We save this because we're about to free the top node
    // temp will become the new stack top
    //
    // OPERATOR PRECEDENCE:
    // -> has higher precedence than *
    // So (*s)->next needs parentheses around *s
    struct Node* temp = (*s)->next;

    // STEP 2: EXTRACT THE VALUE FROM THE TOP NODE
    // *r = (*s)->v stores the top node's value in the caller's variable
    // This is how we "return" the value (since function is void)
    *r = (*s)->v;

    // STEP 3: FREE THE TOP NODE
    // free(*s) returns the memory to the heap
    // After this, the memory could be reused by future malloc calls
    // This is why we saved temp earlier!
    free(*s);

    // STEP 4: UPDATE THE STACK TOP
    // *s = temp makes the stack top point to what was the second node
    // The stack is now one node shorter
    //
    // VISUAL REPRESENTATION:
    // Before: stackTop -> [top: v=3] -> [temp: v=2] -> [v=1] -> NULL
    // After:  stackTop -> [temp: v=2] -> [v=1] -> NULL
    //         [freed memory]
    *s = temp;
  }
}

// TRAVERSE AND PRINT: Display all nodes and their addresses
//
// FUNCTION SIGNATURE:
// void addressTraverse(struct Node* s)
//
// WHY NOT struct Node** here?
// - We're only READING the stack, not modifying the pointers
// - We don't need to change where s points
// - A simple struct Node* is sufficient
//
// This function demonstrates:
// 1. Recursive traversal of a linked structure
// 2. Viewing memory addresses of nodes
// 3. How nodes are chained together
void addressTraverse(struct Node* s){

  // BASE CASE: if s is NULL, we've reached the end of the list
  if(s != NULL){

    // PRINT NODE INFORMATION:
    // %p prints the address of the node (the pointer value)
    // s->v prints the value stored in the node
    //
    // WHY PRINT ADDRESSES?
    // It helps us see memory reuse when we pop and push
    // Notice: after popping node with value 3, then pushing 4,
    // the same address might be reused!
    printf("Address of node %p with value %d \n",s,s->v);

    // RECURSIVE CALL:
    // s->next is the pointer to the next node
    // Recursively traverse the rest of the list
    // This continues until we hit NULL (end of list)
    addressTraverse(s->next);
  }
}

int main(){

  // STEP 1: INITIALIZE EMPTY STACK
  // stackTop is a pointer to the top node of our stack
  // Initially NULL because the stack is empty
  // Type: struct Node* (pointer to Node)
  struct Node* stackTop = NULL;

  // STEP 2: DECLARE A VARIABLE TO RECEIVE POPPED VALUES
  // This will be passed to popStack to receive the returned value
  int ret;

  // STEP 3: PUSH THREE VALUES ONTO THE STACK
  //
  // IMPORTANT: We pass &stackTop (address of stackTop)
  // This gives pushStack a pointer-to-pointer (struct Node**)
  // This allows pushStack to modify stackTop itself
  //
  // After these calls, stack looks like:
  // stackTop -> [v=3] -> [v=2] -> [v=1] -> NULL
  // (Most recently pushed is on top)
  pushStack(&stackTop,1);
  pushStack(&stackTop,2);
  pushStack(&stackTop,3);

  // STEP 4: TRAVERSE AND PRINT THE STACK
  // Shows all three nodes with their addresses and values
  // Expected output (addresses will vary):
  //   Address of node 0x... with value 3
  //   Address of node 0x... with value 2
  //   Address of node 0x... with value 1
  addressTraverse(stackTop);

  // STEP 5: POP ONE VALUE
  // We pass &stackTop so popStack can modify stackTop
  // We pass &ret so popStack can store the returned value in ret
  //
  // After this:
  // - ret contains 3 (the popped value)
  // - stackTop now points to the node with value 2
  // - The node with value 3 has been freed
  popStack(&stackTop,&ret);

  // STEP 6: PRINT THE POPPED VALUE
  // Should print: Return value: 3
  printf("Return value: %d\n", ret);

  // STEP 7: TRAVERSE AGAIN
  // Now the stack only has two nodes (values 2 and 1)
  // Notice: the node with value 3 is gone!
  addressTraverse(stackTop);

  // STEP 8: DEMONSTRATE MEMORY REUSE
  printf("And now!\n");

  // Push a new value (4) onto the stack
  pushStack(&stackTop,4);

  // STEP 9: TRAVERSE ONE MORE TIME
  // INTERESTING OBSERVATION:
  // The address of the new node (value 4) might be the SAME
  // as the address of the old node (value 3) that we freed!
  //
  // WHY?
  // - malloc often reuses recently freed memory
  // - This demonstrates that free() returns memory to the pool
  // - The next malloc might get the same memory block
  //
  // This is a great visualization of heap memory reuse!
  addressTraverse(stackTop);

  // MEMORY LEAK WARNING:
  // We never free the remaining nodes (stackTop and its next nodes)
  // In a proper program, you'd pop all remaining nodes and free them

  return 0;
}

// EXPECTED OUTPUT (addresses will vary):
// Address of node 0x55a1b2c3d2a0 with value 3
// Address of node 0x55a1b2c3d2c0 with value 2
// Address of node 0x55a1b2c3d2e0 with value 1
// Return value: 3
// Address of node 0x55a1b2c3d2c0 with value 2
// Address of node 0x55a1b2c3d2e0 with value 1
// And now!
// Address of node 0x55a1b2c3d2a0 with value 4   <- Note: same address as old value 3!
// Address of node 0x55a1b2c3d2c0 with value 2
// Address of node 0x55a1b2c3d2e0 with value 1

// CONCEPTUAL UNDERSTANDING:
//
// WHY POINTER-TO-POINTER?
//
// Consider what happens if pushStack used struct Node* instead:
//
// void pushStack_WRONG(struct Node* s, int v) {
//     struct Node* h = malloc(sizeof(struct Node));
//     h->v = v;
//     h->next = s;
//     s = h;  // This only changes the LOCAL COPY of s!
// }
//
// In main:
// pushStack_WRONG(stackTop, 1);  // stackTop is COPIED to s
// // After function returns, stackTop is still NULL!
// // The local variable s was changed, but stackTop wasn't!
//
// With struct Node**:
// void pushStack(struct Node** s, int v) {
//     ...
//     *s = h;  // This changes what s POINTS TO (stackTop in main!)
// }
//
// In main:
// pushStack(&stackTop, 1);  // Pass ADDRESS of stackTop
// // After function returns, stackTop points to the new node!
//
// VISUAL REPRESENTATION OF MEMORY:
//
// After pushing 1, 2, 3:
//
// Stack (local variables):
// main's stackTop: [address 0x1000]
//
// Heap:
// 0x1000: [v=3, next=0x2000]  <- Top of stack
// 0x2000: [v=2, next=0x3000]
// 0x3000: [v=1, next=NULL]    <- Bottom of stack
//
// After popping (value 3):
//
// Stack (local variables):
// main's stackTop: [address 0x2000]  <- Now points to second node
// main's ret: [3]
//
// Heap:
// 0x1000: [FREED MEMORY]
// 0x2000: [v=2, next=0x3000]  <- New top
// 0x3000: [v=1, next=NULL]
//
// After pushing 4:
//
// Stack (local variables):
// main's stackTop: [address 0x1000]  <- Might reuse old address!
//
// Heap:
// 0x1000: [v=4, next=0x2000]  <- Same address, new data!
// 0x2000: [v=2, next=0x3000]
// 0x3000: [v=1, next=NULL]
//
// THE FUNDAMENTAL PRINCIPLE:
//
// In C, function parameters are ALWAYS passed by value (copied).
// To modify a variable in the caller:
// - If you want to modify an int: pass int*
// - If you want to modify a pointer: pass pointer-to-pointer (T**)
//
// EXAMPLES:
// void modify_int(int* x) { *x = 10; }     // Can change caller's int
// void modify_ptr(int** p) { *p = NULL; }  // Can change caller's pointer
//
// STACK OPERATIONS SUMMARY:
//
// Push: O(1) - constant time
// - Allocate new node
// - Set its next to current top
// - Update top to new node
//
// Pop: O(1) - constant time
// - Save reference to second node
// - Extract value from top node
// - Free top node
// - Update top to second node
//
// MEMORY REUSE DEMONSTRATION:
// This program beautifully shows how malloc/free work:
// - free() doesn't "erase" memory, it returns it to the allocator
// - The next malloc() might reuse that exact same memory
// - This is why using freed memory (use-after-free) is dangerous!
//
// COMMON MISTAKES:
// 1. Forgetting & when calling pushStack (won't compile)
// 2. Using struct Node* instead of struct Node** (won't modify caller's pointer)
// 3. Forgetting to save temp before free() in pop (lose rest of list!)
// 4. Not checking for NULL before dereferencing (crash!)
// 5. Not freeing memory (memory leak)
// 6. Using freed memory (use-after-free = undefined behavior)
//
// TRY IT:
// 1. Compile: gcc pointstack.c -o pointstack
// 2. Run: ./pointstack
// 3. Observe the addresses - is 0x...2a0 reused for value 4?
// 4. Try pushing and popping different sequences
// 5. Add code to properly free all nodes at the end
// 6. Use valgrind to check for memory leaks: valgrind ./pointstack
// 7. Try implementing peek, size, or isEmpty functions
// 8. What happens if you pop from an empty stack?
