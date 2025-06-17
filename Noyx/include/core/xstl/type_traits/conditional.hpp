#pragma once
namespace xstl {
  template<bool cond, typename Tt, typename Tf>
  struct conditional{ //true
    using type = Tt;
  };

  template<typename Tt, typename Tf>
  struct conditional<false, Tt, Tf> { //false
    using type = Tf;
  };

  template<bool cond, typename Tt, typename Tf>
  using conditional_t = conditional<cond, Tt, Tf>::type;

} //xstl