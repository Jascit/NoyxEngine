#include <tests_details.hpp>

int main() {
  auto& tests = TestRegistry::get();
	for (auto& test : tests)
	{
		test();
	}
}