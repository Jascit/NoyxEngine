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

      // ���������������� �����������: ����� �������� ������� ����� ���. ��������,
      // ������ ������ (optional) � alignment ��� page granularity.
      VMASpaceMG(std::uint64_t virtualSpaceSize = 100ULL * 1024 * 1024 * 1024,
        void* preferredBase = nullptr,
        std::size_t pageGranularity = 4096);
      ~VMASpaceMG();

      // �����������/���������� (���� �� optional � ����������� � ctor/dtor).
      bool init();
      void shutdown();

      // ������� �������� ������ (MEM_RESERVE). ������� ��������� ������ ��� nullptr.
      void* reserveRange(std::uint64_t size, std::size_t alignment = 0);

      // ������� ����� �������������� ������� (MEM_RELEASE).
      bool releaseRange(void* base, std::uint64_t size);

      // ������� ������� ���'��� � ��������������� ������� (MEM_COMMIT).
      // ������� true ���� ������.
      bool commitPages(void* base, std::uint64_t size, DWORD protect = PAGE_READWRITE);

      // ������� ������� ���'��� (MEM_DECOMMIT).
      bool decommitPages(void* base, std::uint64_t size);

      // ������� ����� � ����� VMA: ������ ������ �������, commit, ��������� ptr.
      // alignment �� ������������� � page size
      void* alloc(std::size_t size, std::size_t alignment = 0);

      // �������� ���������� ����: decommit ��� ������ ��������� � ���������.
      bool free(void* ptr);

      // Realloc (�� ���������� ���) � ���� ������ new+copy+free
      void* realloc(void* ptr, std::size_t newSize, std::size_t alignment = 0);

      // �������� ���������� (reserved bytes, committed bytes, allocation count)
      struct Stats {
        std::uint64_t reservedBytes = 0;
        std::uint64_t committedBytes = 0;
        std::uint64_t allocationCount = 0;
      };
      Stats getStats() const;

      // �������� ������ ����:
      // - enable/disable guard pages
      // - fault handler registration (custom structured exception handler)
      // - snapshot/dump ��������� ��� ������

    private:
      // ������� ������� ������
      void* reserveInternal(std::uint64_t size, void* preferredBase, std::size_t alignment);
      bool releaseInternal(void* base, std::uint64_t size);

      // ������ ��� ������ � �����-���������� (��� ����� API � �������� ��������� � .cpp)
      void lockForRead() const;
      void unlockForRead() const;
      void lockForWrite();
      void unlockForWrite();

      // ����
      const uint64_t virtualSpaceSize_;
      void* virtualBase_ = nullptr;
      const noyx::size_t pageGranularity_ = 4*1024;

      // ������� ������� ���������. �� ������������ XArray.hpp � ����� ��������.
      // ������ �������: radix tree, ordered map, interval tree.
      // ��� � �������� ����������: XArray<PageMetadata> metaIndex_;
      containers::XArray<PageMetadata> metaIndex_;

      // ���������� ��� ���������������� (shared mutex ��� �������/�������)
      mutable std::shared_mutex metaMutex_;

      // ���������� (����� � ������� ��� concurrent reads)
      std::atomic<std::uint64_t> reservedBytes_{ 0 };
      std::atomic<std::uint64_t> committedBytes_{ 0 };
      std::atomic<std::uint64_t> allocationCount_{ 0 };

      // ������� ���������/���� (���������, size class, alignment class) � ������� � .cpp
    };
  } // namespace detail
} // namespace noyx
