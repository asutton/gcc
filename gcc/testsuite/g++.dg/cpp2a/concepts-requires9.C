// PR c++/66832
<<<<<<< HEAD:gcc/testsuite/g++.dg/concepts/req15.C
// { dg-do compile { target c++17 } }
// { dg-options "-fconcepts" }
=======
// { dg-do compile }
// { dg-options "-std=c++2a" }
>>>>>>> c81d7292415... Migrating a test.:gcc/testsuite/g++.dg/cpp2a/concepts-requires9.C

template <class T, class U, unsigned N>
  requires requires (T& t, U &u) { t.foo(); u.foo(); }
void foo_all( T (&t)[N], U (&u)[N] ) {
  for(auto& x : t)
      x.foo();
  for(auto& x : u)
      x.foo();
}

struct S {
  void foo() {}
};

int main() {
  S rg[4] {};
  foo_all(rg, rg);
}

