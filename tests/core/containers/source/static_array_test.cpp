#include <tests_details.hpp> // Твій фреймворк
#include "containers/static_array.hpp" // Твій хедер масиву

#include <string>
#include <memory>
#include <stdexcept>

// Щоб код скомпілювався, якщо ASSERT_ABORT не визначено в headers
#ifndef ASSERT_ABORT
#include <cassert>
#define ASSERT_ABORT(cond, msg) assert((cond) && (msg))
#endif

using noyx::containers::StaticArray;

// --- ТЕСТ 1: Базова поведінка (int) ---
NOYX_TEST(StaticArrayTest, BasicIntBehavior) {
    constexpr size_t N = 8;
    StaticArray<int, N> a;

    // Перевірка size()
    NOYX_ASSERT_EQ(a.size(), N);

    // Заповнення через operator[]
    for (size_t i = 0; i < N; ++i) {
        a[i] = (int)i * 10;
    }

    // Перевірка читання
    for (size_t i = 0; i < N; ++i) {
        NOYX_ASSERT_EQ(a[i], (int)i * 10);

        // У твоїй реалізації at() повертає вказівник T*, тому розіменовуємо
        NOYX_ASSERT_EQ(a.at(i), (int)i * 10);
    }

    // Перевірка front/back
    NOYX_ASSERT_EQ(a.front(), 0);
    NOYX_ASSERT_EQ(a.back(), 70);

    // Перевірка data()
    NOYX_ASSERT_TRUE(a.data() != nullptr);
    NOYX_ASSERT_EQ(*a.data(), 0);
}

// --- ТЕСТ 2: Методи fill та assign ---
NOYX_TEST(StaticArrayTest, FillAndAssign) {
    constexpr size_t N = 5;
    StaticArray<int, N> a;

    // Test fill
    a.fill(a.begin(), a.end(), 123);
    for (auto val : a) {
        NOYX_ASSERT_EQ(val, 123);
    }

    // Test assign (часткове перезаписування)
    // assign(count, value)
    a.assign(3, 777);

    NOYX_ASSERT_EQ(a[0], 777);
    NOYX_ASSERT_EQ(a[1], 777);
    NOYX_ASSERT_EQ(a[2], 777);
    NOYX_ASSERT_EQ(a[3], 123); // Старе значення
}

// --- ТЕСТ 3: Глибоке копіювання ---
NOYX_TEST(StaticArrayTest, CopySemantics) {
    constexpr size_t N = 3;
    StaticArray<std::string, N> a;
    a.replace_at(0, "Original");
    a.replace_at(1, "Data");
    a.replace_at(2, "Here");

    // Copy Constructor
    StaticArray<std::string, N> b(a);

    // Модифікуємо копію
    b.replace_at(0, "Changed");

    // Оригінал має залишитись старим
    NOYX_ASSERT_TRUE(a[0] == "Original");
    NOYX_ASSERT_TRUE(b[0] == "Changed");

    // Copy Assignment
    StaticArray<std::string, N> c;
    c = b;
    NOYX_ASSERT_TRUE(c[0] == "Changed");
}

// --- ТЕСТ 4: Move семантика (std::unique_ptr) ---
NOYX_TEST(StaticArrayTest, MoveOnlyType) {
    using U = std::unique_ptr<int>;
    constexpr size_t N = 3;
    StaticArray<U, N> arr;

    // Ініціалізація через replace_at (in-place construction)
    arr.replace_at(0, std::make_unique<int>(10));
    arr.replace_at(1, std::make_unique<int>(20));
    arr.replace_at(2, std::make_unique<int>(30));

    // Move Constructor
    StaticArray<U, N> moved(std::move(arr));

    // Перевіряємо, що moved забрав ресурси
    NOYX_ASSERT_TRUE(moved[0] != nullptr);
    NOYX_ASSERT_EQ(*moved[0], 10);
    NOYX_ASSERT_EQ(*moved[2], 30);

    // Перевіряємо, що старий масив пустий (moved-from unique_ptr == nullptr)
    NOYX_ASSERT_TRUE(arr[0] == nullptr);
}

// --- ТЕСТ 5: replace_at ---
NOYX_TEST(StaticArrayTest, ReplaceAt) {
    constexpr size_t N = 2;
    StaticArray<std::string, N> a;

    // replace_at викликає деструктор старого і конструктор нового
    a.replace_at(0, "Hello");
    NOYX_ASSERT_TRUE(a[0] == "Hello");

    // Замінюємо "Hello" на довгий рядок (перевірка роботи з пам'яттю std::string)
    a.replace_at(0, "Hello World, this is a longer string to verify allocation");
    NOYX_ASSERT_TRUE(a[0].length() > 20);
}

// --- ТЕСТ 6: Вихід за межі (Exceptions) ---
NOYX_TEST(StaticArrayTest, OutOfRange) {
    constexpr size_t N = 5;
    StaticArray<int, N> a;

    bool caught = false;
    try {
        // Індекс N є першим неправильним (0..N-1)
        a.at(N);
    }
    catch (const std::out_of_range&) {
        caught = true;
    }
    catch (...) {
        NOYX_FAIL_MESSAGE("Wrong exception type caught!");
    }

    if (!caught) {
        NOYX_FAIL_MESSAGE("at() did not throw std::out_of_range for invalid index");
    }
}

// --- ТЕСТ 7: SBO Check (Stack vs Heap) ---
NOYX_TEST(StaticArrayTest, SBOCheck) {
    // 1. STACK: 10 int * 4 bytes = 40 bytes. Поріг 1024.
    // Масив має бути "жирним" (містити буфер всередині)
    using StackArr = StaticArray<int, 10>;
    // sizeof класу > розміру даних, бо там буфер
    NOYX_ASSERT_TRUE(sizeof(StackArr) >= 40);

    // 2. HEAP: 1000 int * 4 bytes = 4000 bytes. Поріг 1024.
    // Масив має бути "худим" (тільки вказівник + алокатор)
    using HeapArr = StaticArray<int, 1000>;

    // На 64-біт: вказівник (8) + алокатор (1->pad to 8) + discriminant (якщо є)
    // Зазвичай це 16-24 байти, але точно менше ніж 4000!
    NOYX_ASSERT_TRUE(sizeof(HeapArr) < 100);
}

// --- ТЕСТ 8: Swap ---
NOYX_TEST(StaticArrayTest, Swap) {
    constexpr size_t N = 2;
    StaticArray<int, N> a;
    a[0] = 1; a[1] = 2;

    StaticArray<int, N> b;
    b[0] = 10; b[1] = 20;

    // ADL swap
    using std::swap;
    swap(a, b);

    NOYX_ASSERT_EQ(a[0], 10);
    NOYX_ASSERT_EQ(b[0], 1);
}


// --- ТЕСТ 9: Heap Mode & Pointer Stealing ---
NOYX_TEST(StaticArrayTest, HeapModeBehavior) {
    // 1. Налаштування
    // int = 4 байти. 1000 елементів = 4000 байт.
    // Це більше за kStackThresholdBytes (1024), тому масив піде в Heap.
    constexpr size_t N = 1000;
    using HeapArray = StaticArray<int, N>;

    // Перевірка розміру самого класу: він має бути малим (тільки вказівник), 
    // а не гігантським (якщо б буфер був всередині).
    // sizeof(HeapArray) ~ 8-16 байт << 4000 байт.
    NOYX_ASSERT_TRUE(sizeof(HeapArray) < 100);

    // 2. Створення та заповнення
    HeapArray a;
    for (size_t i = 0; i < N; ++i) a[i] = (int)i;

    // Запам'ятовуємо адресу виділеної пам'яті
    int* original_ptr = a.data();
    NOYX_ASSERT_TRUE(original_ptr != nullptr);

    // 3. Тест Move Constructor (Pointer Stealing)
    HeapArray b(std::move(a));

    // ПЕРЕВІРКА 1: Адреса даних у 'b' має бути ТОЮ САМОЮ, що була у 'a'.
    // Це означає, що копіювання не відбулося, ми просто вкрали вказівник.
    NOYX_ASSERT_EQ(b.data(), original_ptr);

    // ПЕРЕВІРКА 2: Старий об'єкт 'a' має бути пустим (nullptr у Heap режимі).
    NOYX_ASSERT_TRUE(a.data() == nullptr);

    // ПЕРЕВІРКА 3: Дані на місці
    NOYX_ASSERT_EQ(b[0], 0);
    NOYX_ASSERT_EQ(b[N - 1], 999);

    // 4. Тест Swap
    HeapArray c;
    c.fill(c.begin(), c.end(), 777);
    int* c_ptr = c.data();

    // Свапаємо b (старий 'a') і c
    using std::swap;
    swap(b, c);

    // Перевіряємо, що вказівники помінялися місцями
    NOYX_ASSERT_EQ(b.data(), c_ptr);       // b тепер вказує на масив 777
    NOYX_ASSERT_EQ(c.data(), original_ptr); // c тепер вказує на масив 0..999

    NOYX_ASSERT_EQ(b[0], 777);
    NOYX_ASSERT_EQ(c[0], 0);
}