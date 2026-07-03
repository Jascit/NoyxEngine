/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     testing_system.hpp
 * @brief
 *
 * @author   Jascit (https://github.com/Jascit)
 * @date     04.01.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#pragma once
#include <iostream>
#include <string>
#include <test_registry.hpp>
#include <testings_data.hpp>
#include <vector>

class TestingSystem
{
public:
    void fail(std::string message)
    {
        auto &info = TestRegistry::instance().get_current_test_info();
        if (!(info.flag & FAILED))
        {
            info.flag = FAILED;
            m_failed_functions_messages.push_back(std::vector<std::string> {});
            m_failed_makros_count.push_back(0);
            m_failed_functions_names.push_back(info.suite_name);
            ++m_failed;
        }
        m_failed_functions_messages[m_failed - 1].push_back(message);
        m_failed_makros_count[m_failed - 1]++;
    }

    void success() { ++m_passed; }

    ~TestingSystem() { report(); }

    size_t get_failed_count() const { return m_failed; }

    size_t get_passed_count() const { return m_passed; }

    TestingSystem(const TestingSystem &)            = delete;
    TestingSystem &operator=(const TestingSystem &) = delete;

    static TestingSystem *instance()
    {
        static TestingSystem system_instance;
        return &system_instance;
    }

private:
    TestingSystem()
        : m_passed(0),
          m_failed(0)
    {
    }

    void report()
    {
        std::cout << "\n========== Test Summary ==========\n";
        std::cout << "Passed: " << m_passed << "\n";
        std::cout << "Failed: " << m_failed << "\n";

        if (!m_failed_functions_names.empty())
        {
            std::cout << "\nFailed Tests:\n";
            for (int i = 0; i < m_failed_functions_messages.size(); i++)
            {
                std::cout << "-----" << m_failed_functions_names[i] << "-----" << "\n";
                std::cout << "Failed NOYX_MAKROS: " << std::to_string(m_failed_makros_count[i])
                          << "\n";
                for (auto &message : m_failed_functions_messages[i])
                {
                    std::cout << "  - " << m_failed_functions_names[i] << ": " << message << "\n";
                }
            }
        }

        std::cout << "==================================\n";
    }

private:
    size_t m_passed;
    size_t m_failed;
    std::vector<std::string> m_failed_functions_names;
    std::vector<std::vector<std::string>> m_failed_functions_messages;
    std::vector<size_t> m_failed_makros_count;
};
