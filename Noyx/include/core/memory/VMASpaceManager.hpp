#pragma once

#include <Windows.h>
#include <memory>
#include <cstdint>
#include <cstddef>
#include <atomic>
#include <shared_mutex>
#include <functional>
#include "../platform/typedef.hpp"
#include "containers/XArray.hpp"

namespace noyx {
  namespace detail {
    class VMASpaceMG {
    public:
      // non-copyable, non-movable
      VMASpaceMG(const VMASpaceMG&) = delete;
      VMASpaceMG& operator=(const VMASpaceMG&) = delete;
      VMASpaceMG(VMASpaceMG&&) = delete;
      VMASpaceMG& operator=(VMASpaceMG&&) = delete;

      // Параметризований конструктор: можна передати бажаний розмір вірт. простору,
      // базову адресу (optional) і alignment для page granularity.
      VMASpaceMG(std::uint64_t virtualSpaceSize = 100ULL * 1024 * 1024 * 1024,
        void* preferredBase = nullptr,
        std::size_t pageGranularity = 4096);
      ~VMASpaceMG();

      // Ініціалізація/завершення (поки що optional — викликається в ctor/dtor).
      bool init();
      void shutdown();

      // Резервує адресний простір (MEM_RESERVE). Повертає початкову адресу або nullptr.
      void* reserveRange(std::uint64_t size, std::size_t alignment = 0);

      // Звільняє раніше зарезервований діапазон (MEM_RELEASE).
      bool releaseRange(void* base, std::uint64_t size);

      // Коммітне фізичну пам'ять в зарезервованому діапазоні (MEM_COMMIT).
      // Повертає true якщо успішно.
      bool commitPages(void* base, std::uint64_t size, DWORD protect = PAGE_READWRITE);

      // Декомміт фізичну пам'ять (MEM_DECOMMIT).
      bool decommitPages(void* base, std::uint64_t size);

      // Простий аллок в межах VMA: знайти вільний діапазон, commit, повернути ptr.
      // alignment за замовчуванням — page size
      void* alloc(std::size_t size, std::size_t alignment = 0);

      // Звільнити алокований блок: decommit або просто позначити в метаданих.
      bool free(void* ptr);

      // Realloc (на спрощеному рівні) — може робити new+copy+free
      void* realloc(void* ptr, std::size_t newSize, std::size_t alignment = 0);

      // Отримати статистику (reserved bytes, committed bytes, allocation count)
      struct Stats {
        std::uint64_t reservedBytes = 0;
        std::uint64_t committedBytes = 0;
        std::uint64_t allocationCount = 0;
      };
      Stats getStats() const;

      // Додаткові корисні фічі:
      // - enable/disable guard pages
      // - fault handler registration (custom structured exception handler)
      // - snapshot/dump метаданих для аналізу

    private:
      // Внутрішні допоміжні методи
      void* reserveInternal(std::uint64_t size, void* preferredBase, std::size_t alignment);
      bool releaseInternal(void* base, std::uint64_t size);

      // Методи для роботи з радікс-структурою (тут тільки API — реальний контейнер в .cpp)
      void lockForRead() const;
      void unlockForRead() const;
      void lockForWrite();
      void unlockForWrite();

      // Поля
      const uint64_t virtualSpaceSize_;
      void* virtualBase_ = nullptr;
      const noyx::size_t pageGranularity_ = 4*1024;

      // Внутрішнє сховище метаданих. Ти використовуєш XArray.hpp — заміни підходяще.
      // Можливі варіанти: radix tree, ordered map, interval tree.
      // Тут — загальна абстракція: XArray<PageMetadata> metaIndex_;
      containers::XArray<PageMetadata> metaIndex_;

      // Блокування для потокобезпечності (shared mutex для читачів/писачів)
      mutable std::shared_mutex metaMutex_;

      // Статистика (атоми — безпечні для concurrent reads)
      std::atomic<std::uint64_t> reservedBytes_{ 0 };
      std::atomic<std::uint64_t> committedBytes_{ 0 };
      std::atomic<std::uint64_t> allocationCount_{ 0 };

      // Допоміжні константи/мапи (наприклад, size class, alignment class) — реалізуй у .cpp
    };
  } // namespace detail
} // namespace noyx
