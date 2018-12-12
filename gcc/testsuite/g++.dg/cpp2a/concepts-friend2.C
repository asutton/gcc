<<<<<<< HEAD:gcc/testsuite/g++.dg/concepts/friend2.C
// { dg-do compile { target c++17 } }
// { dg-options "-fconcepts" }
=======
// { dg-options "-std=c++2a" }
>>>>>>> 594131fbad3... move ported tests; note more issues and needs port after fixes:gcc/testsuite/g++.dg/cpp2a/concepts-friend2.C

template<typename T>
  concept Eq = requires(T t) { t == t; };

template<Eq T> struct Foo { };

template<typename T>
  struct S { // { dg-error "invalid" }
    template<Eq U> friend class Bar;

    friend class Foo<T>;
  };

struct X { };

int main() {
  S<int> si; // OK
  S<X> sx;
}
