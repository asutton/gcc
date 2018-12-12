<<<<<<< HEAD:gcc/testsuite/g++.dg/concepts/template-parm5.C
// { dg-do compile { target c++17 } }
// { dg-options "-fconcepts" }
=======
// { dg-options "-std=c++2a" }
>>>>>>> 594131fbad3... move ported tests; note more issues and needs port after fixes:gcc/testsuite/g++.dg/cpp2a/concepts-template-parm5.C

template<typename T>
  concept C1 = __is_same_as(T, int);

template<int N>
  concept C2 = N == 0;

template<template<typename> class X>
  concept C3 = true;

template<typename> struct Foo;

template<C1... Ts = int> struct S1; // { dg-error "default argument" }
template<C1... = int> struct S2; // { dg-error "default argument" }
template<C2... Ns = 0> struct S3; // { dg-error "default argument" }
template<C2... = 0> struct S4; // { dg-error "default argument" }
template<C3... Ts = Foo> struct S5; // { dg-error "default argument" }
template<C3... = Foo> struct S6; // { dg-error "default argument" }
