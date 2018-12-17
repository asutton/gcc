<<<<<<< HEAD:gcc/testsuite/g++.dg/concepts/pr68683.C
// { dg-do compile { target c++17 } }
// { dg-options "-fconcepts" }
=======
// { dg-options "-std=c++2a" }
>>>>>>> eda685858ca... move more ported tests:gcc/testsuite/g++.dg/cpp2a/concepts-pr68683.C

template <typename, typename>
struct is_same {
  static constexpr bool value = true;
};

template <typename T, typename U>
concept Same = is_same<T, U>::value;

template <typename T>
concept Integral = requires {
  { T () } -> Same<typename T::value_type>;
};

struct A {
  using value_type = bool;
};

int main () {
  Integral<A>;
  Integral<A>;
  return 0;
}
