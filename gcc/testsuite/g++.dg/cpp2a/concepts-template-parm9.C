<<<<<<< HEAD:gcc/testsuite/g++.dg/concepts/template-parm9.C
// { dg-do compile { target c++17 } }
// { dg-options "-fconcepts" }
=======
// { dg-options "-std=c++2a" }
>>>>>>> 594131fbad3... move ported tests; note more issues and needs port after fixes:gcc/testsuite/g++.dg/cpp2a/concepts-template-parm9.C

template<typename T>
  concept C = __is_class(T);

template<typename T>
  concept D = C<T> and __is_empty(T);

template<template<typename Q> requires C<Q> class X>
  struct S { };

template<typename A> requires true struct T0 { };
template<typename A> requires D<A> struct T1 { };

S<T0> x3; // { dg-error "constraint mismatch|invalid type" }
S<T1> x4; // { dg-error "constraint mismatch|invalid type" }

int main() { }
