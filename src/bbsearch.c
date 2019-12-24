#include "bbsearch.h"

static int32_t EMPTY_INT32[1] = {INT32_MAX};

__attribute__((always_inline)) static inline void update_search_window(const int32_t *array, ptrdiff_t *start, ptrdiff_t *end, const int32_t key) {
    ptrdiff_t median = (*start + *end) / 2;
    int32_t value = array[median];

    uintptr_t start_mask = -(key < value);
    uintptr_t end_mask = -(key > value);

    *start = (median & ~start_mask) | (*start & start_mask);
    *end = ((median & ~end_mask) | (*end & end_mask)) +
            (~start_mask & ~end_mask & 1);
}

ptrdiff_t bbsearch(int32_t *array, size_t size, int32_t key) {
    unsigned int bits = 64 - __builtin_clzl(size);
    int is_non_empty_mask = -(size > 0);
    int is_empty_mask = ~is_non_empty_mask;

    // __builtin_clzl has undefined behavior for 0, so set bits to 0 in that case
    bits = (unsigned int) is_non_empty_mask & bits;

    // Clear most bits to make a jump table more likely
    // The switch below has extra cases to avoid checking if bits is handled by
    // a case
    bits = bits % 64;

    ptrdiff_t start = 0;

    // use a dummy array if size == 0
    array = (int32_t *) (((uintptr_t) is_empty_mask & (uintptr_t) EMPTY_INT32) |
            ((uintptr_t) is_non_empty_mask & (uintptr_t) array));

    ptrdiff_t end = is_non_empty_mask & size | is_empty_mask & 1;

    switch (bits) { // NOLINT(hicpp-multiway-paths-covered)
        case 63: update_search_window(array, &start, &end, key);
        case 62: update_search_window(array, &start, &end, key);
        case 61: update_search_window(array, &start, &end, key);
        case 60: update_search_window(array, &start, &end, key);
        case 59: update_search_window(array, &start, &end, key);
        case 58: update_search_window(array, &start, &end, key);
        case 57: update_search_window(array, &start, &end, key);
        case 56: update_search_window(array, &start, &end, key);
        case 55: update_search_window(array, &start, &end, key);
        case 54: update_search_window(array, &start, &end, key);
        case 53: update_search_window(array, &start, &end, key);
        case 52: update_search_window(array, &start, &end, key);
        case 51: update_search_window(array, &start, &end, key);
        case 50: update_search_window(array, &start, &end, key);
        case 49: update_search_window(array, &start, &end, key);
        case 48: update_search_window(array, &start, &end, key);
        case 47: update_search_window(array, &start, &end, key);
        case 46: update_search_window(array, &start, &end, key);
        case 45: update_search_window(array, &start, &end, key);
        case 44: update_search_window(array, &start, &end, key);
        case 43: update_search_window(array, &start, &end, key);
        case 42: update_search_window(array, &start, &end, key);
        case 41: update_search_window(array, &start, &end, key);
        case 40: update_search_window(array, &start, &end, key);
        case 39: update_search_window(array, &start, &end, key);
        case 38: update_search_window(array, &start, &end, key);
        case 37: update_search_window(array, &start, &end, key);
        case 36: update_search_window(array, &start, &end, key);
        case 35: update_search_window(array, &start, &end, key);
        case 34: update_search_window(array, &start, &end, key);
        case 33: update_search_window(array, &start, &end, key);
        case 32: update_search_window(array, &start, &end, key);
        case 31: update_search_window(array, &start, &end, key);
        case 30: update_search_window(array, &start, &end, key);
        case 29: update_search_window(array, &start, &end, key);
        case 28: update_search_window(array, &start, &end, key);
        case 27: update_search_window(array, &start, &end, key);
        case 26: update_search_window(array, &start, &end, key);
        case 25: update_search_window(array, &start, &end, key);
        case 24: update_search_window(array, &start, &end, key);
        case 23: update_search_window(array, &start, &end, key);
        case 22: update_search_window(array, &start, &end, key);
        case 21: update_search_window(array, &start, &end, key);
        case 20: update_search_window(array, &start, &end, key);
        case 19: update_search_window(array, &start, &end, key);
        case 18: update_search_window(array, &start, &end, key);
        case 17: update_search_window(array, &start, &end, key);
        case 16: update_search_window(array, &start, &end, key);
        case 15: update_search_window(array, &start, &end, key);
        case 14: update_search_window(array, &start, &end, key);
        case 13: update_search_window(array, &start, &end, key);
        case 12: update_search_window(array, &start, &end, key);
        case 11: update_search_window(array, &start, &end, key);
        case 10: update_search_window(array, &start, &end, key);
        case 9: update_search_window(array, &start, &end, key);
        case 8: update_search_window(array, &start, &end, key);
        case 7: update_search_window(array, &start, &end, key);
        case 6: update_search_window(array, &start, &end, key);
        case 5: update_search_window(array, &start, &end, key);
        case 4: update_search_window(array, &start, &end, key);
        case 3: update_search_window(array, &start, &end, key);
        case 2: update_search_window(array, &start, &end, key);
        case 1: update_search_window(array, &start, &end, key);
        case 0: {}
    }

    /* this is equivalent to the bitwise operations below
   if (is_empty_mask) {
        return -1;
    } else if (array[start] == key) {
        return start;
    } else if (array[start] < key) {
        return -end - 1;
    } else if (array[end] > key) {
        return -start - 1;
    } else {
        return 0;
    }
     */
    int is_match_mask = -(array[start] == key);
    int is_after_start = -(array[start] < key);
    int is_before_end = -(array[end] > key);

    return (is_empty_mask & -1) | (is_non_empty_mask & (
            (is_match_mask & start) | (~is_match_mask & (
                    (is_after_start & (-end - 1)) |
                    (~is_after_start & (is_before_end & (-start - 1)))))));
}
