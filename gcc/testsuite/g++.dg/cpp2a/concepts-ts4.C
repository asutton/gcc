// { dg-do compile }
// { dg-options "-std=c++2a -fconcepts-ts" }

// Basic tests for introduction syntax.

template<typename T>
concept True = true;

template<typename T>
concept False = false;

template<typename T, typename U>
concept Same = __is_same_as(T, U);

template<int N>
concept Pos = N >= 0;

True{T} void f1(T) { }
False{T} void f2(T) { }
Same{X, Y} void same();
Pos{N} void fn();

void driver()
{
  f1(0);
  f2(0); // { dg-error "cannot call function" }

  same<int, int>();
  same<int, char>(); // { dg-error "cannot call function" }

  fn<0>(); // OK
  fn<-1>(); // { dg-error "cannot call function" }
  fn<int>(); // { dg-error "no matching function" }
}
