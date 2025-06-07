#pragma once
#include <testings_data.hpp>

class TestingSystem {
public:
    constexpr void fail(TestInfo& info, const char* message) {
        m_failedFunctionsNames.push_back(std::forward<const char*>(info.testName));
        m_failedFunctionsMessages.push_back(message);
        m_failed++;
    }

    constexpr void success() {
        m_successed++;
    }

    ~TestingSystem() {
        report();
    }
    constexpr inline size_t GetFailedCount() {
      return m_failed;
    }
    constexpr inline size_t GetSuccessedCount() {
      return m_successed;
    }
private:
    void report() {
        std::cout << "\nSummary: " << m_successed << " passed, " << m_failed << " failed.\n";
    }

private:
    size_t m_successed;
    size_t m_failed;

    std::vector<const char*> m_failedFunctionsNames;
    std::vector<const char*> m_failedFunctionsMessages;
public:
    TestingSystem(const TestingSystem&) = delete;
    TestingSystem operator=(const TestingSystem&) = delete;
    static inline TestingSystem* instance() {
        static TestingSystem systemInstance;
        return &systemInstance;
    }
private:
  TestingSystem() = default;
};