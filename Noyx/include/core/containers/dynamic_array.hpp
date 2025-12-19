#pragma once
#include <platform/typedef.hpp>
#include <utility/utility.hpp>
#include <utility/macros.hpp>
#include <type_traits>
#include <memory>
#include <algorithm>
#include <iterator>
#include "memory/guards.hpp"
#include "memory/uninitialized.hpp"
#include <stdexcept>
#include <cstring>
#include "iterators/iterators.hpp"


namespace noyx {
	namespace containers {
		template<typename T>
		class DynamicArray
		{
			using _FirstZeroSecondArgs = noyx::utility::_FirstZeroSecondArgs;
			using _FirstOneSecondArgs = noyx::utility::_FirstOneSecondArgs;
		public:
			using allocator_type = std::allocator<T>;
			using traits = std::allocator_traits<allocator_type>;
			using pointer = typename traits::pointer;
			using const_pointer = typename traits::const_pointer;
			using value_type = T;
			using size_type = size_t;
			using iterator = MyIterator<T, false>;
			using const_iterator = MyIterator<T, true>;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;

			static constexpr bool kTrivialDestroy = std::is_trivially_destructible_v<T>;

			noyx::utility::TCompressedPair<allocator_type, T*> storage_;
			size_type size_;
			size_type capacity_;

		private:
			T* data_ptr() noexcept { return storage_.second(); }
			const T* data_ptr() const noexcept { return storage_.second(); }

			allocator_type& alloc() noexcept { return storage_.first(); }
			const allocator_type& alloc() const noexcept { return storage_.first(); }

			void deallocate_storage()
			{
				if (data_ptr() != nullptr)
				{
					traits::deallocate(alloc(), data_ptr(), capacity_);
				}
			}

		public:
			// --- Default Constructor ---
			explicit DynamicArray(const allocator_type& a = allocator_type()) noexcept : storage_(_FirstOneSecondArgs{}, a, nullptr), size_(0), capacity_(0) {}

			// --- Copy Constructor ---
			DynamicArray(const DynamicArray& other)
				: storage_(_FirstOneSecondArgs{}, traits::select_on_container_copy_construction(other.alloc()), nullptr)
				, size_(other.size_)
				, capacity_(other.size_)
			{
				if (size_ > 0)
				{
					T* new_ptr = traits::allocate(alloc(), capacity_);

					AllocationGuard<allocator_type> mem_guard(alloc(), new_ptr, capacity_);
					noyx::memory::uninitialized_copy_n(alloc(), new_ptr, size_, other.data_ptr());
					mem_guard.commit();
					storage_.second() = new_ptr;
				}
			}

			// --- Move Constructor ---
			DynamicArray(DynamicArray&& other) noexcept
				: storage_(_FirstOneSecondArgs{}, std::move(other.alloc()), other.data_ptr())
				, size_(other.size_)
				, capacity_(other.capacity_)
			{
				other.storage_.second() = nullptr;
				other.size_ = 0;
				other.capacity_ = 0;
			}


			// --- Destructor ---
			~DynamicArray()
			{
				T* ptr = data_ptr();
				if constexpr (!kTrivialDestroy)
				{
					for (size_t i = 0; i < size_; ++i)
					{
						traits::destroy(alloc(), ptr + (size_ - 1 - i));
					}
				}
				deallocate_storage();
			}


		public:
			using POCCA = typename traits::propagate_on_container_copy_assignment;
			using POCMA = typename traits::propagate_on_container_move_assignment;
			using POCS = typename traits::propagate_on_container_swap;

			// --- Copy Assignment ---
			DynamicArray& operator=(const DynamicArray& other)
			{
				if (this == &other) return *this;

				constexpr bool pocca = POCCA::value;
				bool reuse_buffer = (capacity_ >= other.size_);

				if constexpr (pocca)
				{
					if (alloc() != other.alloc())
					{
						reuse_buffer = false;
					}
				}

				if (reuse_buffer)
				{
					if (size_ >= other.size_)
					{
						for (size_type i = 0; i < other.size_; ++i)
						{
							data_ptr()[i] = other.data_ptr()[i];
						}
						noyx::memory::destroy_range(alloc(), data_ptr() + other.size_, data_ptr() + size_);
					}
					else
					{
						for (size_type i = 0; i < size_; ++i)
						{
							data_ptr()[i] = other.data_ptr()[i];
						}
						noyx::memory::uninitialized_copy_n(alloc(), data_ptr() + size_, (other.size_ - size_), other.data_ptr() + size_);
					}
					
					size_ = other.size_;

					if constexpr (pocca) {
						alloc() = other.alloc();
					}
				}

				else
				{
					DynamicArray temp(other);
					swap(temp);
				}
				return *this;
			}


			// --- Move Assignment
			DynamicArray& operator=(DynamicArray&& other) noexcept // додав noexcept (бажано)
			{
				if (this == &other) return *this;
				constexpr bool pocma = POCMA::value;
				if constexpr (pocma)
				{
					alloc() = std::move(other.alloc());
				}

				if (alloc() == other.alloc())
				{
					std::swap(storage_, other.storage_);
					std::swap(size_, other.size_);
					std::swap(capacity_, other.capacity_);
				}
				else
				{
					DynamicArray temp(std::move(other));
					swap(temp);
				}
				return *this;
			}

			// --- Equality Operators ---
			friend bool operator==(const DynamicArray& lhs, const DynamicArray& rhs)
			{
				if (lhs.size() != rhs.size()) return false;
				// Порівнюємо від begin() до begin() іншого масиву
				return std::equal(lhs.begin(), lhs.end(), rhs.begin());
			}

			friend bool operator!=(const DynamicArray& lhs, const DynamicArray& rhs)
			{
				return !(lhs == rhs);
			}

			// --- Swap ---
			void swap(DynamicArray& other) noexcept
			{
				using std::swap;
				swap(storage_, other.storage_);
				swap(size_, other.size_);
				swap(capacity_, other.capacity_);
				if constexpr (POCS::value)
				{
					swap(alloc(), other.alloc());
				}
			}


		public:
			// --- Getters ---
			size_type capacity() const noexcept { return capacity_; }
			size_type size() const noexcept { return size_; }
			[[nodiscard]] bool empty() const noexcept {
				return size_ == 0;
			}

			void reserve(size_type new_cap)
			{
				if (new_cap <= capacity_) return;

				T* new_ptr = traits::allocate(alloc(), new_cap);
				T* old_ptr = data_ptr();

				AllocationGuard<allocator_type> mem_guard(alloc(), new_ptr, new_cap);
				noyx::memory::uninitialized_move_n(alloc(), new_ptr, size_, old_ptr);

				if constexpr (!kTrivialDestroy)
				{
					for (size_type i = 0; i < size_; ++i)
					{
						traits::destroy(alloc(), old_ptr + (size_ - 1 - i));
					}
				}
				if (old_ptr != nullptr)
				{
					traits::deallocate(alloc(), old_ptr, capacity_);
				}
				mem_guard.commit();

				storage_.second() = new_ptr;
				capacity_ = new_cap;
			}


			void push_back(const T& value)
			{
				if (size_ == capacity_)
				{
					reserve(capacity_ == 0 ? 1 : capacity_ * 2);
				}
				traits::construct(alloc(), data_ptr() + size_, value);
				++size_;
			}
			void push_back(T&& value)
			{
				if (size_ == capacity_)
				{
					reserve(capacity_ == 0 ? 1 : capacity_ * 2);
				}
				traits::construct(alloc(), data_ptr() + size_, std::move(value));
				++size_;
			}


		public:
			// --- Element Access ---
			constexpr T& operator[](size_t i) noexcept { return data_ptr()[i]; }
			constexpr const T& operator[](size_t i) const noexcept { return data_ptr()[i]; }

			constexpr T& at(size_t i) { return (i < size_) ? data_ptr()[i] : throw std::out_of_range("StaticArray::at: index out of bounds"); }
			constexpr const T& at(size_t i) const { return (i < size_) ? data_ptr()[i] : throw std::out_of_range("StaticArray::at: index out of bounds"); }

			constexpr T& front() noexcept { return data_ptr()[0]; }
			constexpr const T& front() const noexcept { return data_ptr()[0]; }

			constexpr T& back() noexcept { return data_ptr()[size_ - 1]; }
			constexpr const T& back() const noexcept { return data_ptr()[size_ - 1]; }

			constexpr T* data() noexcept { return data_ptr(); }
			constexpr const T* data() const noexcept { return data_ptr(); }


			// --- Iterators ---
			constexpr iterator begin() noexcept { return iterator(data_ptr()); }
			constexpr const_iterator begin() const noexcept { return const_iterator(data_ptr()); }
			constexpr const_iterator cbegin() const noexcept { return const_iterator(data_ptr()); }

			constexpr iterator end() noexcept { return iterator(data_ptr() + size_); }
			constexpr const_iterator end() const noexcept { return const_iterator(data_ptr() + size_); }
			constexpr const_iterator cend() const noexcept { return const_iterator(data_ptr() + size_); }


			// --- Reverse Iterators ---
			constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
			constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
			constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
			constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
			constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
			constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }


		public:
			// --- Methods ---
			void pop_back() noexcept
			{
				if (size_ > 0)
				{
					traits::destroy(alloc(), data_ptr() + (size_ - 1));
					--size_;
				}
			}

			void clear() noexcept
			{
				if (size_ > 0)
				{
					noyx::memory::destroy_range(alloc(), data_ptr(), data_ptr() + size_);
					size_ = 0;
				}
			}

			template<typename... Args>
			T& emplace_back(Args&&... args)
			{
				if (size_ == capacity_)
				{
					reserve(capacity_ == 0 ? 1 : capacity_ * 2);
				}
				traits::construct(alloc(), data_ptr() + size_, std::forward<Args>(args)...);
				return data_ptr()[size_++];
			}

			void resize(size_type new_size, const T& value = T())
			{
				if (new_size > size_)
				{
					if (new_size > capacity_)
					{
						reserve(new_size);
					}
					noyx::memory::uninitialized_fill_n(alloc(), data_ptr() + size_, new_size - size_, value);
				}

				else if (new_size < size_)
				{
					noyx::memory::destroy_range(alloc(), data_ptr() + new_size, data_ptr() + size_);
				}
				size_ = new_size;
			}
		};
	}
}