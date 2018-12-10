// { dg-do compile }
// { dg-options "-std=c++2a" }

template<typename T>
concept Type = true;

template<typename T>
concept Class = __is_class(T);

template<typename T>
concept EmptyClass = Class<T> && __is_empty(T);

template<typename T, typename U>
concept Classes = __is_class(T) && __is_class (U);

struct empty { };

struct nonempty { int n; };

template<typename T>
struct S
{
  void f() requires Class<T> { }
  
  template<typename U>
  struct Inner
  {
    void g() requires Classes<T, U> { }
  };

  template<typename U> requires Classes<T, U> void h(U) { }
};

struct X { };

void driver_1()
{
  S<X> s1;
  s1.f(); // OK
  s1.h(s1); // OK
  s1.h(0); // { dg-error "no matching function" }

  S<int> s2;
  s2.f(); // { dg-error "no matching function" }

  S<X>::Inner<X> si1;
  si1.g();
  
  S<X>::Inner<int> si2;
  si2.g(); // { dg-error "no matching function" }
}

// Check constraints on non-dependent arguments, even when in a
// dependent context.

template<typename T> requires Class<T> void f1(T x) { }

// fn1-2.C -- Depenedent checks
template<typename T>
void caller_1(T x) 
{ 
  f1(x); // Unchecked dependent arg.
  f1(empty{}); // Checked non-dependent arg, but OK
  f1(0); // { dg-error "cannot call function" }
}

// fn3.c -- Ordering
template<typename T> 
  requires Class<T> 
constexpr int f2(T x) { return 1; }

template<typename T> 
  requires EmptyClass<T> 
constexpr int f2(T x) { return 2; }

template<typename T> 
constexpr int f3(T x) requires Class<T> { return 1; }

template<typename T> 
constexpr int f3(T x) requires EmptyClass<T> { return 2; }

void driver_2() 
{
  f2(0); // { dg-error "no matching function" }
  static_assert (f2(empty{}) == 2);
  static_assert (f2(nonempty{}) == 1);
  f3(0); // { dg-error "no matching function" }
  static_assert (f3(empty{}) == 2);
  static_assert (f3(nonempty{}) == 1);
}

// fn8.C -- Address of overloaded functions
template<typename T> requires Type<T> void ok(T) { }
template<typename T> requires Class<T> void err(T) { }

auto p1 = &ok<int>;
auto p2 = &err<int>; // { dg-error "no matches" }
void (*p3)(int) = &ok<int>;
void (*p4)(int) = &err<int>; // { dg-error "no matches" }
void (*p5)(int) = &ok;
void (*p6)(int) = &err; // { dg-error "no matches" }

template<typename T> void g(T x) { }

void driver_3 () 
{
  g(&ok<int>);
  g(&err<int>); // { dg-error "no matches" }
}


struct S2 {
  template<typename T> requires Type<T> int ok(T) { return 0; }
  template<typename T> requires Class<T> int err(T) { return 0; }
};

auto p7 = &S2::ok<int>;
auto p8 = &S2::err<int>; // { dg-error "no matches" }
int (S2::*p9)(int) = &S2::ok<int>;
int (S2::*p10)(int) = &S2::err<int>; // { dg-error "no matches" }
int (S2::*p11)(int) = &S2::ok;
int (S2::*p12)(int) = &S2::err; // { dg-error "no matches" }
