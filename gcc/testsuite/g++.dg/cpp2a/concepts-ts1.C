// { dg-do compile }
// { dg-options "-std=c++2a -fconcepts-ts" }

// This tests the terse notation.

template<typename T>
concept True = true;

template<typename T>
concept False = false;

template<typename T, typename U>
concept SameAs = __is_same_as(T, U);

True x1 = 0;
False x2 = 0; // { dg-error "deduced initializer does not satisfy" }

void f1(True t) { }
void f2(False t) { }
void f3(SameAs<int> q) { }
void f4(True a, SameAs<decltype(a)> b) { }

True f5() { return 0; }
False f6() { return 0; } // { dg-error "placeholder constraints not satisfied" }
SameAs<int> f7() { return 0; }
SameAs<int> f8() { return 'a'; } // { dg-error "placeholder constraints not satisfied" }
auto f9() -> True { return 0; }
auto f10() -> False { return 0; } // { dg-error "placeholder constraints not satisfied" }
auto f11() -> SameAs<int> { return 0; }
auto f12() -> SameAs<char> { return 0; } // { dg-error "placeholder constraints not satisfied" }
auto f13(int n) -> SameAs<decltype(n)> { return n; }
auto f14(int n) -> SameAs<decltype(n)> { return 'a'; } // { dg-error "placeholder constraints not satisfied" }
auto f15(auto x) -> SameAs<decltype(x)> { return 0; } // { dg-error "placeholder constraints not satisfied" }

void driver()
{
  f1(0);
  f2(0); // { dg-error "cannot call" }
  f3(0);
  f3('a'); // { dg-error "cannot call" }
  f4(0, 0);
  f4(0, 'a'); // { dg-error "cannot call" }
  f15(0);
  f15('a');
}
