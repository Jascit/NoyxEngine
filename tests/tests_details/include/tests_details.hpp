#pragma once
#include <vector>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <cstring>

using TestFunc = std::function<void()>;

struct TestInfo {
  const char* suiteName;
  const char* testName;
  TestFunc testFunc;
  const char* file;
  int line;
};

inline std::vector<TestInfo>& getTestRegistry() {
  static std::vector<TestInfo> registry;
  return registry;
}

struct TestRegistrar {
  TestRegistrar(const char* suite, const char* name, TestFunc func, const char* file, int line) {
    getTestRegistry().push_back({ suite, name, func, file, line });
  }
};

#define NOYX_CONCAT_INTERNAL(a, b) a##b
#define NOYX_CONCAT(a, b) NOYX_CONCAT_INTERNAL(a, b)

#define NOYX_EVAL(expr) expr

#define NOYX_TEST(SuiteName, TestName)                                                      \
    static void NOYX_CONCAT(SuiteName, _##TestName)();                                      \
    static TestRegistrar NOYX_CONCAT(registrar_, NOYX_CONCAT(SuiteName, _##TestName)) (     \
        #SuiteName, #TestName, &NOYX_CONCAT(SuiteName, _##TestName), __FILE__, __LINE__);   \
    static void NOYX_CONCAT(SuiteName, _##TestName)()

#define NOYX_FAIL()                                                                          \
    do {                                                                                     \
        std::ostringstream oss;                                                              \
        oss << "Test failed at " << __FILE__ << ":" << __LINE__ << "\n";                     \
        throw std::runtime_error(oss.str());                                                 \
    } while (0)

#define NOYX_FAIL_MESSAGE(message)                                                           \
    do {                                                                                     \
        std::ostringstream oss;                                                              \
        oss << "Test failed at " << __FILE__ << ":" << __LINE__ << "\n";                     \
        oss << "Message: " << message << "\n";                                               \
        throw std::runtime_error(oss.str());                                                 \
    } while (0)

#define NOYX_ASSERT_TRUE(expr)                                                               \
    do {                                                                                     \
        if (!(expr)) {                                                                       \
            std::ostringstream oss;                                                          \
            oss << "Expected true but was false: " << #expr                                  \
                << " (" << __FILE__ << ":" << __LINE__ << ")";                               \
            throw std::runtime_error(oss.str());                                             \
        }                                                                                    \
    } while (0)

#define NOYX_ASSERT_TRUE_MESSAGE(expr, message)                                              \
    do {                                                                                     \
        if (!(expr)) {                                                                       \
            std::ostringstream oss;                                                          \
            oss << "Expected true but was false: " << #expr                                  \
            << "\nMessage: " << message                                                  \
                << " (" << __FILE__ << ":" << __LINE__ << ")";                               \
            throw std::runtime_error(oss.str());                                             \
        }                                                                                    \
    } while (0)

#define NOYX_ASSERT_FALSE(expr)                                                              \
    do {                                                                                     \
        if (expr) {                                                                          \
            std::ostringstream oss;                                                          \
            oss << "Expected false but was true: " << #expr                                  \
                << " (" << __FILE__ << ":" << __LINE__ << ")";                               \
            throw std::runtime_error(oss.str());                                             \
        }                                                                                    \
    } while (0)

#define NOYX_ASSERT_FALSE_MESSAGE(expr, message)                                             \
    do {                                                                                     \
        if (expr) {                                                                          \
            std::ostringstream oss;                                                          \
            oss << "Expected false but was true: " << #expr                                  \
                << "\nMessage: " << message                                                  \
                << " (" << __FILE__ << ":" << __LINE__ << ")";                               \
            throw std::runtime_error(oss.str());                                             \
        }                                                                                    \
    } while (0)

#define NOYX_ASSERT_EQ(expected, actual)                                                     \
    do {                                                                                     \
        auto _exp_val = (expected);                                                          \
        auto _act_val = (actual);                                                            \
        if (!(_exp_val == _act_val)) {                                                       \
            std::ostringstream oss;                                                          \
            oss << "Expected equality: " << #expected << " == " << #actual                   \
                << "\n   Expected: " << _exp_val                                             \
                << "\n   Actual:   " << _act_val                                             \
                << " (" << __FILE__ << ":" << __LINE__ << ")";                               \
            throw std::runtime_error(oss.str());                                             \
        }                                                                                    \
    } while (0)

#define NOYX_ASSERT_LT(a, b)                                                                 \
    do {                                                                                     \
        auto _a = (a); auto _b = (b);                                                        \
        if (!(_a < _b)) {                                                                    \
            std::ostringstream oss; oss << "Expected " << #a << " < " << #b                  \
                                        << "\n   " << _a << " !< " << _b                     \
                                        << " (" << __FILE__ << ":" << __LINE__ << ")";       \
            throw std::runtime_error(oss.str());                                             \
        }                                                                                    \
    } while (0)

#define NOYX_ASSERT_LE(a, b)                                                                 \
    do {                                                                                     \
        auto _a = (a); auto _b = (b);                                                        \
        if (!(_a <= _b)) {                                                                   \
            std::ostringstream oss; oss << "Expected " << #a << " <= " << #b                 \
                                        << "\n   " << _a << " !<= " << _b                    \
                                        << " (" << __FILE__ << ":" << __LINE__ << ")";       \
            throw std::runtime_error(oss.str());                                             \
        }                                                                                    \
    } while (0)

#define NOYX_ASSERT_GT(a, b)                                                                 \
    do {                                                                                     \
        auto _a = (a); auto _b = (b);                                                        \
        if (!(_a > _b)) {                                                                    \
            std::ostringstream oss; oss << "Expected " << #a << " > " << #b                  \
                                        << "\n   " << _a << " !> " << _b                     \
                                        << " (" << __FILE__ << ":" << __LINE__ << ")";       \
            throw std::runtime_error(oss.str());                                             \
        }                                                                                    \
    } while (0)

#define NOYX_ASSERT_GE(a, b)                                                                 \
    do {                                                                                     \
        auto _a = (a); auto _b = (b);                                                        \
        if (!(_a >= _b)) {                                                                   \
            std::ostringstream oss; oss << "Expected " << #a << " >= " << #b                 \
                                        << "\n   " << _a << " !>= " << _b                    \
                                        << " (" << __FILE__ << ":" << __LINE__ << ")";       \
            throw std::runtime_error(oss.str());                                             \
        }                                                                                    \
    } while (0)

#define NOYX_ASSERT_STREQ(a, b)                                                              \
    do {                                                                                     \
        const char* _a = (a);                                                                \
        const char* _b = (b);                                                                \
        if (std::strcmp(_a, _b) != 0) {                                                      \
            std::ostringstream oss; oss << "Expected C-string equality: "                    \
                                        << #a << " == " << #b                                \
                                        << "\n   \"" << _a << "\" != \"" << _b << "\""       \
                                        << " (" << __FILE__ << ":" << __LINE__ << ")";       \
            throw std::runtime_error(oss.str());                                             \
        }                                                                                    \
    } while (0)
