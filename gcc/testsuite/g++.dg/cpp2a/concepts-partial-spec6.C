// PR c++/67152
<<<<<<< HEAD:gcc/testsuite/g++.dg/concepts/partial-spec6.C
// { dg-do compile { target c++17 } }
// { dg-options "-fconcepts" }
=======
// { dg-options "-std=c++2a" }
>>>>>>> eda685858ca... move more ported tests:gcc/testsuite/g++.dg/cpp2a/concepts-partial-spec6.C

template <class T>
concept HasType = requires { typename T::type; };

template<class T>
struct trait {
  using type = void;
};

struct has_type { using type = void; };

// Instantiation here
trait<has_type>::type foo() {}

// constrained version here. Type "has_type" would fail this
// constraint so this partial specialization would not have been
// selected.
template<class T>
  requires (!HasType<T>)
struct trait<T> {
  using type = void;
};
