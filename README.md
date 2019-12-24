# Branchless Binary Search
[![CircleCI](https://circleci.com/gh/SiftScience/sift-java.svg?style=svg)](https://circleci.com/gh/SiftScience/sift-java)

This is a proof-of-concept binary search implementation that entirely avoids branching,
i.e. `if-then-else`. It accomplishes this with a mix of bit twiddling,
jump tables, and access to the x86 `BSR` instruction through a compiler builtin.

## Background
* [Binary search](https://en.wikipedia.org/wiki/Binary_search_algorithm)
* [Branch prediction](https://en.wikipedia.org/wiki/Branch_predictor)

## Motivation
Branch mispredictions can be expensive, especially on architectures with long
pipelines. This approach avoids branching entirely, at the expense of code size
and more overall instructions.

Additionally, because the same code is run regardless of input, this approach can be
extended to have SIMD/vector support, parallelizing searches across the same
(or different) data.

## Implementation
At a high level, the search works as follows:

1. compute `log2(length)`
2. `switch` on the result
3. Taking advantage of case fall-through, run a modified binary search update
`log2(length)` times.
4. Interpret the values of the search window to get the result

### `log2(length)` call
This is an optimization. The can work correctly by running `log2(sizeof(length))`
(e.g. 32, 64) times.

A standard binary search implicitly computes this empirically, essentially
dividing `length` by 2 until it's 0. Because this implementation uses a jump
table to avoid a loop condition, it needs to be known before the search.

```c
unsigned int bits = 64 - __builtin_clzl(size);
```

### Jump table
This is functionally the same as the loop in an iterative binary search
implementation⁠—it runs `update_search_window` `log2(length)` times.

Without `bits % 64`, the compiler adds a check to make sure there's a case
for `bits`.

```c
switch (bits % 64) {
    case 63: update_search_window(array, &start, &end, key);
    case 62: update_search_window(array, &start, &end, key);
    case 61: update_search_window(array, &start, &end, key);
    ...
```

### Window update function
For the most part, the update function looks similar to a standard binary
search, but instead of returning when the result is found, the window remains
the same allowing the function to be run again.

The second difference is more in implementation than function. Rather than
using an `if-then-else` construct to updated `start` or `end`, both are
updated regardless, with bit masks used to select the next value.

```c
__attribute__((always_inline)) static inline void update_search_window(
    const int32_t *array, ptrdiff_t *start, ptrdiff_t *end, const int32_t key) {

    ptrdiff_t median = (*start + *end) / 2;
    int32_t value = array[median];

    uintptr_t start_mask = -(key < value);
    uintptr_t end_mask = -(key > value);

    *start = (median & ~start_mask) | (*start & start_mask);
    *end = ((median & ~end_mask) | (*end & end_mask)) +
            (~start_mask & ~end_mask & 1);
}
```

### Computing the result
Like a recursive binary search's base case, the result is determined by
comparing the `key` being looked up the current window. Like in the window
update function, bitmasks are used to avoid branching.

Unlike `bsearch()`, but like Java's <a href="https://docs.oracle.com/javase/7/docs/api/java/util/Arrays.html#binarySearch(int[], int)">`Arrays.binarySearch()`</a>,
this returns `-insertion_point - 1` when `key` isn't found.

```c
int is_match_mask = -(array[start] == key);
int is_after_start = -(array[start] < key);
int is_before_end = -(array[end] > key);

return (is_empty_mask & -1) | (is_non_empty_mask & (
        (is_match_mask & start) | (~is_match_mask & (
                (is_after_start & (-end - 1)) |
                (~is_after_start & (is_before_end & (-start - 1)))))));
```
