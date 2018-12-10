// { dg-do compile }
// { dg-options "-std=c++2a" }

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
  void f() requires Class<T>
  { }
  
  template<typename U>
  struct Inner
  {
    void g() requires Classes<T, U>
    { }
  };

  template<typename U>
    requires Classes<T, U>
   void h(U) { }
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

template<typename T>
  requires Class<T>
void f1(T x) { }

// fn1.C
template<typename T>
void caller_1(T x) { 
  f1(x); // Unchecked dependent arg.
}

template<typename T>
void caller_2(T x) { 
  f1(empty{}); // Checked non-dependent arg, but OK
}

// fn2.C
template<typename T>
void caller_3() {
  f1(0); // { dg-error "cannot call function" }
}

// fn3.c

int called = 0;

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

void driver_2() {
  f2(0); // { dg-error "no matching function" }
  static_assert (f2(empty{}) == 2);
  static_assert (f2(nonempty{}) == 1);
  f3(0); // { dg-error "no matching function" }
  static_assert (f3(empty{}) == 2);
  static_assert (f3(nonempty{}) == 1);
}
