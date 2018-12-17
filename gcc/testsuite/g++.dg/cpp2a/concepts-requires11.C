// PR c++/67018
<<<<<<< HEAD:gcc/testsuite/g++.dg/concepts/req17.C
// { dg-do compile { target c++17 } }
// { dg-options "-fconcepts" }
=======
// { dg-do compile }
// { dg-options "-std=c++2a" }
>>>>>>> 1358ae9e3b2... Finish migrating all of the existing requires tests.:gcc/testsuite/g++.dg/cpp2a/concepts-requires11.C

template <typename T>
constexpr bool Val = true;

template <class I>
concept InputIterator = requires (I i) {
  requires Val<decltype(i++)>;
};

template <class I>
concept ForwardIterator = InputIterator<I> && true;

template<InputIterator>
constexpr bool f() { return false; }

template<ForwardIterator>
constexpr bool f() { return true; }

static_assert(f<int*>());
