#pragma once

#include <platform/typedef.hpp>
#include <utility/utility.hpp>
#include <type_traits>
#include <memory>
#include <cstring>
#include "iterators/iterators.hpp"

namespace noyx {
	namespace containers {
		template<typename T, size_t N>
		class StaticArray {
		public:
			T data[N];
			size_t size__ = N;

		public:
			StaticArray() = default;

			StaticArray(const StaticArray& other) {			//copy-constructor
				for (size_t i = 0; i < N; i++) {
					data[i] = other.data[i];
				}
			}

			StaticArray(StaticArray&& other) {				//move-constructor
				for (size_t i = 0; i < N; i++) {
					data[i] = std::move(other.data[i]);
				}
			}

			T& operator[](size_t i) { return data[i]; }
			const T& operator[](size_t i) const { return data[i]; }

		public:
			size_t size() { return size__; }
			bool empty() const { return size__ == 0; }

			T& at(size_t i) {								//returns index element
				if (i < N) {
					return data[i];
				}
				else throw std::out_of_range("Invalid index!");
			}
			const T& at(size_t i) const {
				if (i < N) {
					return data[i];
				}
				else throw std::out_of_range("Invalid index!");
			}

			T& front() {									//returns first element
				if (!empty()) return data[0];
				else throw std::out_of_range("Array is empty!");
			}
			const T& front() const {
				if (!empty()) return data[0];
				else throw std::out_of_range("Array is empty!");
			}

			T& back() {										//returns last element
				if (!empty()) return data[N - 1];
				else throw std::out_of_range("Array is empty!");
			}
			const T& back() const {
				if (!empty()) return data[N - 1];
				else throw std::out_of_range("Array is empty!");
			}
		};
	}
}