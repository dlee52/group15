#ifndef VALIDATION_HELPER_H_
#define VALIDATION_HELPER_H_

#include "dberror.h"
#define RETURN_IF_NULL(ptr, err, message) \
    if (ptr == NULL)                      \
    {                                     \
        RC_message = message;             \
        return err;                       \
    }

#define RETURN_IF_NOT_EQ(expr, value, err, message) \
    if ((expr) != (value))                          \
    {                                               \
        RC_message = message;                       \
        return err;                                 \
    }

#define RETURN_OK(message)    \
    do                        \
    {                         \
        RC_message = message; \
        return RC_OK;         \
    } while (0);

#define RETURN_IF_OUTSIDE_RANGE(expr, low, high, err, message) \
    if (((expr) < (low)) || ((expr) >= high))                  \
    {                                                          \
        RC_message = message;                                  \
        return err;                                            \
    }

#define CHECK_OK(expr)   \
    if ((expr) != RC_OK) \
    {                    \
        return (expr);   \
    }

#define CHECK_LE(expr, value, err, message) \
    if ((expr) <= (value))                  \
    {                                       \
        RC_message = message;               \
        return err;                         \
    }

#endif // VALIDATION_HELPER_H_
