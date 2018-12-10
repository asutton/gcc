// { dg-do compile }
// { dg-options "-std=c++2a" }

template<typename T>
concept Class = __is_class(T);

template<typename T, typename U>
concept Classes = __is_class(T) && __is_class (U);

struct empty { };

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

int main()
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
void constrained_f(T x) { }

template<typename T>
void caller_1(T x) { 
  constrained_f(x); // Unchecked dependent arg.
}

template<typename T>
void caller_2(T x) { 
  constrained_f(empty{}); // Checked non-dependent arg, but OK
}

template<typename T>
void caller_3() {
  constrained_f(0); // { dg-error "cannot call function" }
}
