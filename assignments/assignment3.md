# Assignment 3: Threads and Arrays

Okay folks, let's give several threads some work to share! Your program will divide an array into chunks. Each worker will claim the next available chunk, process it, and come back for another until there is no work left.

The interesting part is deciding what needs to be protected by a mutex and what the threads can safely do on their own.

## Your program

Start with an array of `int` values allocated using `malloc`. Initialize every element to zero. For an initial version, use 1000 elements, chunks of 100 elements, and four worker threads. You can keep these sizes as constants, and you may assume the chunk size evenly divides the array size.

Your program should:

- Create a shared counter that identifies the next unclaimed chunk, and protect that counter with a mutex.
- Have each worker lock the mutex, check whether work remains, claim a chunk by advancing the counter, and unlock the mutex. The check and claim need to happen together while the mutex is locked.
- Have the worker add exactly one to each element of its claimed chunk **after releasing the mutex**. Then it should try to claim another chunk.
- End each worker when there are no chunks left. Be sure to unlock the mutex on that path, too!
- Use `pthread_join` to wait for all the workers before checking the results.
- Check allocation and thread-creation results, and clean up the resources you created. Don't free the array while a worker could still be using it. If thread creation fails, report the failure and join the workers that did start before cleaning up.

Every chunk should be claimed exactly once. Don't assign a fixed part of the array to each worker in advance; the shared counter is how the workers find their next job. It's fine if one worker finishes more chunks than another.

Incrementing each element by one is enough for full credit. Once that version works, you're welcome to experiment with other mapping operations.

## Show that it works

After joining the workers, check **every element** of the array. Each should be `1`. Count how many elements have an unexpected value and print a summary, such as:

```text
Checked 1000 elements: 0 incorrect values.
```

If the check fails, print the index and value of at least the first incorrect element. Printing a few sample values can help you debug, but the full-array check is required.

Run at least these three configurations, starting from a newly initialized array each time:

| Array size | Chunk size | Worker threads |
| --- | --- | --- |
| 1000 | 100 | 1 |
| 1000 | 100 | 4 |
| 1000 | 500 | 4 |

The last case has more workers than chunks. Workers that find no work should exit normally. Run the configurations several times and include the results. Passing these checks is useful evidence; your explanation of the synchronization matters too.

## Explain one piece

In a short paragraph or a diagram, explain what the mutex protects. Walk through how two workers could otherwise claim the same chunk. Then explain why the workers can modify their separately claimed chunks without holding the mutex, and why the main thread waits before checking the array.

Also explain what would happen to the amount of parallel work if a worker kept the mutex locked while processing its entire chunk.

## Resources and submission

The [mutex example](https://github.com/clarissalittler/cs201/blob/main/winter2025/lecture10/mutexreview.c) shows how to package a mutex with the data it protects. The [concurrency tutorial](https://github.com/clarissalittler/cs201/tree/main/concurrency-tutorial) is another place to look. Lectures 9 and 10 from the winter term, in the Videos tab, cover this material as well.

Submit your C source files, build and run instructions, test results, and explanation. For a single file named `chunks.c`:

```sh
cc -std=c11 -Wall -Wextra -Wpedantic -g -pthread chunks.c -o chunks
./chunks
```

## If you want to explore performance

Try comparing the same workload with different thread counts or chunk sizes. Keep printing and result checking outside the timed work, repeat your measurements, and report what you observed. More threads don't have to be faster! This is an optional extension; the required task is to share the work correctly and explain why it is safe.
