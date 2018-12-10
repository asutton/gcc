// { dg-do compile }
// { dg-options "-std=c++2a" }

// req8.C
// This is just a sanity test for type requirements.

template<typename T>
concept Has_member_type = requires { typename T::type; };

template<typename T>
concept Concept = true && Has_member_type<T>;

template<typename T>
  requires Concept<T>
void foo(T t) { }

// req9.C

template<typename T>
struct S1 { };

template<typename T>
concept C = requires(T x) { { x.fn() } -> S1<T>; };

template<typename U>
  requires C<U>
void fn(U x)
{
  x.fn();
}

struct S2
{
  auto fn() const { return S1<S2>(); }
};

int main()
{
  fn(S2{});
  return 0;
}
