#ifndef THREADS_FIXED_POINT_H
#define THREADS_FIXED_POINT_H
#include <stdint.h>
#define F (1 << 14)
#define INT_TO_FP(n) ((n) * F)
#define FP_TO_INT_TRUNCATE(n) ((n) / F)
#define FP_TO_INT_ROUND(n) ((n) >= 0 ? (((n) + (F / 2)) / F) : (((n) - (F / 2)) / F))
#define ADD(x, y) ((x) + (y))
#define SUBTRACT(x, y) ((x) - (y))
#define ADD_FP_INT(x, n) ((x) + (n)*F)
#define SUBTRACT_FP_INT(x, n) ((x) - (n)*F)
#define MULTIPLY(x, y) (((int64_t) (x)) * (y) / F)
#define MULTIPLY_FP_INT(x, n) ((x) * (n))
#define DIVIDE(x, y) (((int64_t) (x)) * F / (y))
#define DIVIDE_FP_INT(x, n) ((x) / (n))
#endif