<<<<<<< HEAD:gcc/testsuite/g++.dg/concepts/inherit-ctor4.C
// { dg-do compile { target c++17 } }
// { dg-options "-fconcepts" }
=======
// { dg-options "-std=c++2a" }
>>>>>>> 594131fbad3... move ported tests; note more issues and needs port after fixes:gcc/testsuite/g++.dg/cpp2a/concepts-inherit-ctor4.C

template<typename T>
  concept C = __is_class(T);

template<typename T>
  struct S1 {
    template<C U> S1(U x) { }
  };

template<typename T>
  struct S2 : S1<T> {
    using S1<T>::S1;
  };

int main() {
  S2<int> s(0); // { dg-error "no matching function" }
}
