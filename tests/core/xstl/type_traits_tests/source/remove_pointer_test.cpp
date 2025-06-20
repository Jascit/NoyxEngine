#include <type_traits/remove_pointer.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>

template<typename T, typename Expected>
constexpr void tt_remove_pointer_test_type() {
  constexpr bool result = xstl::is_same_v<xstl::remove_pointer_t<T>, Expected>;
  NOYX_ASSERT_TRUE_MESSAGE(
    (result),
    "remove_pointer_t<T> returned wrong type"
  );
}

template<typename T>
constexpr void tt_remove_pointer_test_all() {
  tt_remove_pointer_test_type<T, T>();                    
  tt_remove_pointer_test_type<T*, T>();                        
  tt_remove_pointer_test_type<T**, T*>();                      
  tt_remove_pointer_test_type<T* const, T>();                  
  tt_remove_pointer_test_type<T* volatile, T>();               
  tt_remove_pointer_test_type<T* const volatile, T>();         
}

NOYX_TEST(RemovePointer, UnitTest) {
  tt_remove_pointer_test_all<int>();
  tt_remove_pointer_test_all<float>();
  tt_remove_pointer_test_all<char>();
  tt_remove_pointer_test_all<double>();
}
