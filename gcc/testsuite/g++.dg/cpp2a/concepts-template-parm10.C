<<<<<<< HEAD:gcc/testsuite/g++.dg/concepts/template-parm10.C
// { dg-do compile { target c++17 } }
// { dg-options "-fconcepts" }
=======
// { dg-options "-std=c++2a" }
>>>>>>> 594131fbad3... move ported tests; note more issues and needs port after fixes:gcc/testsuite/g++.dg/cpp2a/concepts-template-parm10.C

template<int N, class T>
  concept P = true;

template<template<typename> class X, class T>
  concept Q = true;

template<P<int> N> void f() { }
template<Q<int> X> void g() { }

template<typename> struct S { };

int main() {
  f<0>();
  g<S>();
}
