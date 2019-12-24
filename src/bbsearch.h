#ifndef BBSEARCH_H
#define BBSEARCH_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern ptrdiff_t bbsearch(int32_t *array, size_t size, int32_t key);

#ifdef __cplusplus
}
#endif

#endif //BBSEARCH_H
