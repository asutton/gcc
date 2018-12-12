// { dg-do compile }
// { dg-options "-std=c++2a" }

// A poor mans Integral concept.
template<typename T>
concept Integral = __is_same_as(T, int);

template<typename... Args>
  requires Integral<Args...>
void f1();

void driver()
{
  f1<int, int>(); // { dg-error "cannot call function" }
}
