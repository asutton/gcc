// { dg-do compile }
// { dg-options "-std=c++2a" }


template<typename T>
concept Class = __is_class(T);

template<Class T>
void f1(T t) { }

struct empty { };

void driver_1() 
{
  f1(0); // { dg-error "cannot call function" }
  f1(empty{});
}
