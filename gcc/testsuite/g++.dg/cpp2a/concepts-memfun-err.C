<<<<<<< HEAD:gcc/testsuite/g++.dg/concepts/memfun-err.C
<<<<<<< HEAD
// { dg-do compile { target c++17 } }
// { dg-options "-fconcepts" }
=======
// needs port, fix function concepts
// { dg-do run}
// { dg-options "-std=c++17 -fconcepts" }
>>>>>>> 594131fbad3... move ported tests; note more issues and needs port after fixes

=======
// { dg-options "-std=c++2a" }
>>>>>>> eda685858ca... move more ported tests:gcc/testsuite/g++.dg/cpp2a/concepts-memfun-err.C

template<typename T>
  concept C = __is_class(T);

template<typename T>
  concept D = __is_empty(T);

struct X { } x;
struct Y { int n; } y;

int called = 0;

// Test constrained member definitions
template<typename T>
  struct S1 { // { dg-message "defined here" }
    void f1() requires C<T> { }
    void g1() requires C<T> and true;
    template<C U> void h1(U u) { called = 1; }

    void g2() requires C<T>; // { dg-message "candidate" }
  };

template<typename T>
  void S1<T>::g2() requires D<T> { } // { dg-error "no declaration matches" }

int main() {
  S1<X> sx;
  S1<Y> sy;
  S1<int> si;

  si.f1(); // { dg-error "matching" }
  si.g1(); // { dg-error "matching" }
  si.h1(0); // { dg-error "matching" }
}
