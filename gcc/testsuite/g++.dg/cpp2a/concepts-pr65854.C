<<<<<<< HEAD:gcc/testsuite/g++.dg/concepts/pr65854.C
// { dg-do compile { target c++17 } }
// { dg-options "-fconcepts" }
=======
// { dg-options "-std=c++2a" }
>>>>>>> eda685858ca... move more ported tests:gcc/testsuite/g++.dg/cpp2a/concepts-pr65854.C

// Handle alias templates in type requirements.

template<typename T1, typename T2>
struct BTT { };

template<typename T>
struct BTT<T,T> { using type = int; };

template<typename T1, typename T2>
using Alias1 = typename BTT<T1, T2>::type;

template<typename T1, typename T2>
concept C = requires() { typename Alias1<T1, T2>; };

template<typename T1, typename T2>
  requires C<T1, T2>
int f();

auto i = f<char, int>(); // { dg-error "cannot call function" }
