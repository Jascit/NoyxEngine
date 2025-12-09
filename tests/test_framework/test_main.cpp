#include <iostream>
#include <chrono>

// Підключаємо твій хедер з реалізацією Registry та System
#include <tests_details.hpp> 

int main() {
    auto& registry = TestRegistry::instance().getRegistry();

    std::cout << "=== NOYX UNIT TESTS ===\n";
    std::cout << "Running " << registry.size() << " tests:\n";

    for (auto& info : registry) {
        std::cout << "[RUN] " << info.suiteName << "." << info.testName << " ... ";

        auto start = std::chrono::steady_clock::now();

        // Виклик тестової функції
        info.testFunc();

        auto end = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        // Перевірка результату через бітову маску (як у твоєму коді)
        if (!(info.flag & FAILED)) {
            std::cout << "OK (" << ms << " ms)\n";
            TestingSystem::instance()->success();
        }
        else {
            std::cout << "FAILED\n";
            // Тут можна вивести info.file і info.line, якщо хочеш
        }

        // Перемикаємо глобальний індекс на наступний тест
        ++TestRegistry::instance();
    }

    std::cout << "--------------------------------------------------\n";
    std::cout << "Tests Passed: " << TestingSystem::instance()->GetPassedCount() << "\n";
    std::cout << "Tests Failed: " << TestingSystem::instance()->GetFailedCount() << "\n";

    return (TestingSystem::instance()->GetFailedCount() == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}