#include <tests_details.hpp> // Твій фреймворк
#include "containers/dynamic_array.hpp" // Хедер твого DynamicArray

#include <string>
#include <memory>
#include <vector>

#ifndef NOYX_ASSERT_FALSE
#define NOYX_ASSERT_FALSE(cond) NOYX_ASSERT_TRUE(!(cond))
#endif

using noyx::containers::DynamicArray;

// --- ТЕСТ 1: Базова поведінка та Push Back ---
NOYX_TEST(DynamicArrayTest, BasicPushAndAccess) {
    DynamicArray<int> arr;

    // Спочатку масив пустий
    NOYX_ASSERT_EQ(arr.size(), 0);
    NOYX_ASSERT_EQ(arr.capacity(), 0);
    NOYX_ASSERT_TRUE(arr.empty());

    // Додаємо елементи (trigger allocation)
    arr.push_back(10);
    arr.push_back(20);
    arr.push_back(30);

    NOYX_ASSERT_EQ(arr.size(), 3);
    NOYX_ASSERT_TRUE(arr.capacity() >= 3); // Capacity може бути 4 або більше (стратегія x2)
    NOYX_ASSERT_FALSE(arr.empty());

    // Перевірка доступу
    NOYX_ASSERT_EQ(arr[0], 10);
    NOYX_ASSERT_EQ(arr[1], 20);
    NOYX_ASSERT_EQ(arr.back(), 30);
    NOYX_ASSERT_EQ(arr.front(), 10);
}

// --- ТЕСТ 2: Reserve (Уникнення переалокацій) ---
NOYX_TEST(DynamicArrayTest, Reserve) {
    DynamicArray<int> arr;

    // Виділяємо пам'ять наперед
    arr.reserve(100);

    NOYX_ASSERT_EQ(arr.size(), 0);
    NOYX_ASSERT_EQ(arr.capacity(), 100);

    int* ptr_before = arr.data();

    // Заповнюємо до 100 елементів
    for (int i = 0; i < 100; ++i) {
        arr.push_back(i);
    }

    // Перевіряємо, що вказівник не змінився (переалокації не було)
    NOYX_ASSERT_EQ(arr.data(), ptr_before);
    NOYX_ASSERT_EQ(arr.size(), 100);
}

// --- ТЕСТ 3: Copy Semantics (Глибока копія) ---
NOYX_TEST(DynamicArrayTest, DeepCopy) {
    DynamicArray<std::string> original;
    original.push_back("Hello");
    original.push_back("World");

    // Copy Constructor
    DynamicArray<std::string> copy(original);

    // Перевірка незалежності
    NOYX_ASSERT_EQ(copy.size(), original.size());
    NOYX_ASSERT_TRUE(copy[0] == "Hello");

    // Змінюємо копію
    copy[0] = "Changed";
    copy.push_back("New");

    // Оригінал має залишитись незмінним
    NOYX_ASSERT_TRUE(original[0] == "Hello");
    NOYX_ASSERT_EQ(original.size(), 2);
    NOYX_ASSERT_EQ(copy.size(), 3);

    // Copy Assignment
    DynamicArray<std::string> assigned;
    assigned = copy;
    NOYX_ASSERT_TRUE(assigned[0] == "Changed");
}

// --- ТЕСТ 4: Move Semantics (Крадіжка ресурсів) ---
NOYX_TEST(DynamicArrayTest, MoveSemantics) {
    DynamicArray<int> source;
    source.reserve(1000);
    source.push_back(42);

    int* original_ptr = source.data();
    size_t original_cap = source.capacity();

    // Move Constructor
    DynamicArray<int> dest(std::move(source));

    // Dest має отримати ресурси Source
    NOYX_ASSERT_EQ(dest.data(), original_ptr);
    NOYX_ASSERT_EQ(dest.size(), 1);
    NOYX_ASSERT_EQ(dest.capacity(), original_cap);
    NOYX_ASSERT_EQ(dest[0], 42);

    // Source має стати пустим (valid state)
    NOYX_ASSERT_EQ(source.size(), 0);
    NOYX_ASSERT_EQ(source.capacity(), 0);
    NOYX_ASSERT_TRUE(source.data() == nullptr);

    // Move Assignment
    DynamicArray<int> dest2;
    dest2 = std::move(dest);
    NOYX_ASSERT_EQ(dest2.data(), original_ptr);
}

// --- ТЕСТ 5: Emplace Back (Пряме створення) ---
struct ComplexObj {
    int x, y;
    ComplexObj(int a, int b) : x(a), y(b) {}
    bool operator==(const ComplexObj& other) const { return x == other.x && y == other.y; }
};

NOYX_TEST(DynamicArrayTest, EmplaceBack) {
    DynamicArray<ComplexObj> arr;

    // Передаємо аргументи конструктора, а не сам об'єкт
    ComplexObj& ref = arr.emplace_back(10, 20);

    NOYX_ASSERT_EQ(arr.size(), 1);
    NOYX_ASSERT_EQ(arr[0].x, 10);
    NOYX_ASSERT_EQ(arr[0].y, 20);

    // Перевірка, що функція повертає посилання на створений елемент
    NOYX_ASSERT_EQ(&ref, &arr[0]);
}

// --- ТЕСТ 6: Resize (Зростання та Зменшення) ---
NOYX_TEST(DynamicArrayTest, Resize) {
    DynamicArray<int> arr;

    // 1. Resize GROW (з дефолтним значенням)
    arr.resize(5, 7); // 5 елементів по 7
    NOYX_ASSERT_EQ(arr.size(), 5);
    for (size_t i = 0; i < 5; ++i) NOYX_ASSERT_EQ(arr[i], 7);

    // 2. Resize GROW (з доповненням існуючих)
    arr.resize(8, 9); // Старі (5) лишаються, нові (3) будуть 9
    NOYX_ASSERT_EQ(arr.size(), 8);
    NOYX_ASSERT_EQ(arr[4], 7); // Старий
    NOYX_ASSERT_EQ(arr[5], 9); // Новий
    NOYX_ASSERT_EQ(arr[7], 9); // Новий

    // 3. Resize SHRINK
    arr.resize(2);
    NOYX_ASSERT_EQ(arr.size(), 2);
    NOYX_ASSERT_EQ(arr[0], 7);
    // Capacity зазвичай не зменшується при resize down
    NOYX_ASSERT_TRUE(arr.capacity() >= 8);
}

// --- ТЕСТ 7: Pop Back & Clear ---
NOYX_TEST(DynamicArrayTest, PopAndClear) {
    DynamicArray<std::string> arr;
    arr.push_back("One");
    arr.push_back("Two");
    arr.push_back("Three");

    // Pop Back
    arr.pop_back();
    NOYX_ASSERT_EQ(arr.size(), 2);
    NOYX_ASSERT_TRUE(arr.back() == "Two");

    // Clear
    size_t old_cap = arr.capacity();
    arr.clear();

    NOYX_ASSERT_EQ(arr.size(), 0);
    NOYX_ASSERT_EQ(arr.capacity(), old_cap); // Пам'ять не звільняється
    NOYX_ASSERT_TRUE(arr.empty());
}

// --- ТЕСТ 8: Exception Safety (At) ---
NOYX_TEST(DynamicArrayTest, OutOfRange) {
    DynamicArray<int> arr;
    arr.push_back(1);

    bool caught = false;
    try {
        arr.at(5); // Індекс за межами
    }
    catch (const std::out_of_range&) {
        caught = true;
    }

    if (!caught) {
        NOYX_FAIL_MESSAGE("at() did not throw out_of_range");
    }
}

// --- ТЕСТ 9: Порівняння (Operators == / !=) ---
NOYX_TEST(DynamicArrayTest, Equality) {
    DynamicArray<int> a;
    a.push_back(1); a.push_back(2);

    DynamicArray<int> b;
    b.push_back(1); b.push_back(2);

    DynamicArray<int> c;
    c.push_back(1); c.push_back(99);

    NOYX_ASSERT_TRUE(a == b);
    NOYX_ASSERT_FALSE(a == c);
    NOYX_ASSERT_TRUE(a != c);

    b.push_back(3);
    NOYX_ASSERT_FALSE(a == b); // Різний розмір
}

// --- ТЕСТ 10: Робота з об'єктами (RAII Check) ---
// Цей тест перевіряє, чи правильно викликаються деструктори при видаленні/ресайзі
static int g_destructor_count = 0;
struct Tracker {
    ~Tracker() { g_destructor_count++; }
};

NOYX_TEST(DynamicArrayTest, DestructorCalls) {
    g_destructor_count = 0;
    {
        DynamicArray<Tracker> arr;
        arr.resize(5);
        arr.pop_back();
    }

    NOYX_ASSERT_EQ(g_destructor_count, 5);
}