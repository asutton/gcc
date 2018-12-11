// PR c++/66758
<<<<<<< HEAD:gcc/testsuite/g++.dg/concepts/req14.C
// { dg-do compile { target c++17 } }
// { dg-options "-fconcepts" }
=======
// { dg-do compile }
// { dg-options "-std=c++2a" }
>>>>>>> e21ff18e8cd... Restructuring requirement tests.:gcc/testsuite/g++.dg/cpp2a/concepts-requires7.C

template <class T, class U>
concept C = requires (T t, U u) { t + u; };

template <class T, class U>
  requires C<T,U>
void f(T t, U u) { t + u; }

struct non_addable { };

int main()
{
  using T = decltype(f(42, 24));
}
