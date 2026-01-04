#pragma once
#include <iostream>
#include <cstdlib>

#ifdef NDEBUG

#define ASSERT_ABORT(cond, msg) ((void)0)
#define NOYX_ASSERT(cond)       ((void)0)

#else

#define ASSERT_ABORT(cond, msg) \
        do { \
            if (!(cond)) { \
                std::cerr << "[NOYX ASSERT FAILED] " << (msg) << "\n" \
                          << "File: " << __FILE__ << ", Line: " << __LINE__ << "\n"; \
                std::abort(); \
            } \
        } while(0)

#define NOYX_ASSERT(cond) ASSERT_ABORT(cond, #cond)

#endif