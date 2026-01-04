#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <functional>

// --- 1. Типи та Енуми ---

enum TestFlag {
    SUCCESSED = 0,
    FAILED = 1
};

// Тип функції тесту
using TestFunc = void(*)();

// Структура, яка описує один тест
struct TestInfo {
    const char* suiteName;
    const char* testName;
    TestFunc testFunc;
    int flag;          // SUCCESSED або FAILED
    const char* file;
    int line;
};

// --- 2. TestingSystem (Збір статистики) ---

class TestingSystem {
public:
    static TestingSystem* instance() {
        static TestingSystem inst;
        return &inst;
    }

    void success() { passedCount++; }
    void fail() { failedCount++; }

    size_t GetFailedCount() const { return failedCount; }
    size_t GetPassedCount() const { return passedCount; }

private:
    size_t passedCount = 0;
    size_t failedCount = 0;
};

// --- 3. TestRegistry (Список всіх тестів) ---

class TestRegistry {
public:
    static TestRegistry& instance() {
        static TestRegistry inst;
        return inst;
    }

    // Повертає посилання на вектор тестів (для циклу в main)
    std::vector<TestInfo>& getRegistry() {
        return registry;
    }

    // Цей оператор викликається в main в кінці циклу: ++TestRegistry::instance();
    // Він перемикає індекс на наступний тест, щоб макроси знали, де ми зараз.
    void operator++() {
        currentTestIndex++;
    }

    // Допоміжний метод: позначає поточний тест як FAILED
    void markCurrentFailed() {
        if (currentTestIndex < registry.size()) {
            registry[currentTestIndex].flag |= FAILED;
        }
        TestingSystem::instance()->fail();
    }

private:
    std::vector<TestInfo> registry;
    size_t currentTestIndex = 0; // Індекс тесту, який зараз виконується
};

// --- 4. TestRegistrar (Реєстратор) ---
// Ця структура створюється статично перед main і додає тест у список.

struct TestRegistrar {
    TestRegistrar(const char* suite, const char* name, TestFunc func, const char* file, int line) {
        // Додаємо тест у реєстр зі статусом SUCCESSED за замовчуванням
        TestRegistry::instance().getRegistry().push_back({ suite, name, func, SUCCESSED, file, line });
    }
};

// --- 5. Макроси (Makros) ---

// Макрос для створення тесту
// 1. Оголошує функцію.
// 2. Створює статичний об'єкт Registrar, який додає цю функцію в список.
// 3. Починає тіло функції.
#define NOYX_TEST(SuiteName, TestName) \
    void TestFunc_##SuiteName##_##TestName(); \
    static TestRegistrar registrar_##SuiteName##_##TestName( \
        #SuiteName, #TestName, TestFunc_##SuiteName##_##TestName, __FILE__, __LINE__); \
    void TestFunc_##SuiteName##_##TestName()

// Макрос перевірки рівності (ASSERT_EQ)
#define NOYX_ASSERT_EQ(val1, val2) \
    if ((val1) != (val2)) { \
        std::cerr << "    [FAIL] Expected equality check failed:\n" \
                  << "      Expected: " << (val1) << "\n" \
                  << "      Actual:   " << (val2) << "\n" \
                  << "      File: " << __FILE__ << ":" << __LINE__ << "\n"; \
        TestRegistry::instance().markCurrentFailed(); \
        return; \
    }

// Макрос перевірки істини (ASSERT_TRUE)
#define NOYX_ASSERT_TRUE(condition) \
    if (!(condition)) { \
        std::cerr << "    [FAIL] Condition is false: " << #condition << "\n" \
                  << "      File: " << __FILE__ << ":" << __LINE__ << "\n"; \
        TestRegistry::instance().markCurrentFailed(); \
        return; \
    }

// Макрос для примусового провалу
#define NOYX_FAIL_MESSAGE(msg) \
    { \
        std::cerr << "    [FAIL] " << msg << "\n" \
                  << "      File: " << __FILE__ << ":" << __LINE__ << "\n"; \
        TestRegistry::instance().markCurrentFailed(); \
        return; \
    }