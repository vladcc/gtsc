#ifndef ARR_ALGO_DEFN_H
#define ARR_ALGO_DEFN_H

#include <stdbool.h>

// Module:      arr_algo
// Callback definitions.
typedef int (*arr_fcomp)(const void * arr_key, const void * your_key);
typedef bool (*arr_fpred)(const void * arr_key, void * arg);
typedef bool (*arr_fapply)(void * arr_key, void * arg);
#endif
