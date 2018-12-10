// { dg-do compile }
// { dg-options "-std=c++2a" }

template<typename T>
concept Class = __is_class(T);

// Allow a requires-expression with no parms.
template<typename T>
concept C = requires { typename T::type; };

void f1(int a) requires true;         // OK
auto f2(int a) -> bool requires true; // OK
auto f3(int a) requires true -> bool; // { dg-error "" } requires-clause precedes trailing-return-type
typedef void fn_t() requires true;    // { dg-error "typedef" }
void (*pf)() requires true;           // { dg-error "non-function" }
void (*fn(int))() requires false;     // { dg-error "return type" }
void g(int (*)() requires true);      // { dg-error "parameter|non-function" }
auto* p = new (void(*)(char) requires true); // { dg-error "type-id" }
void f4(auto a) requires Class<decltype(a)> { }
void f5(auto a) requires requires (decltype(a) x) { -x; } { } 

struct Test {
  void f(auto a) requires Class<decltype(a)>;
} test;

void driver_1() {
  struct S { } s;
  f4(s);
  f5(0);
  f5((void*)0); // { dg-error "cannot call" }
  test.f(s);
}

void Test::f(auto a) requires Class<decltype(a)> { }

template<bool B> requires B struct S0; // OK

// Diagnostics are required only during satisfaction. We could test earlier,
// but that causes multiple errors during parsing.
template<int N> requires N struct S1 { };
S1<1> x0; // { dg-error "invalid use of class template|does not have type" }

// We want to diagnose the syntax error (N == 0 must be in parens). Don't
// diagnose constraint errors during parsing.
template<int N> requires N == 0 struct S2 { }; // { dg-error "expected unqualified-id" }

template<int N> requires (N == 0) struct S3 { }; // OK

template<typename T, T X> requires X struct S4 { }; // OK
S4<int, 0> x1;      // { dg-error "invalid use of class template|does not have type" }
S4<bool, true> x2; // OK
S4<bool, false> x3; // { dg-error "invalid use of class template" }

struct fool {
  constexpr fool operator&&(fool) const { return {}; }
  constexpr fool operator||(fool) const { return {}; }
};

template<typename T> constexpr fool p1() { return {}; }
template<typename T> constexpr fool p2() { return {}; }

template<typename T>
concept Bad = p1<T>() && p2<T>();

template<typename T> requires Bad<T> void bad(T x) { }

int main() {
  bad(0); // { dg-error "cannot call" }
}
