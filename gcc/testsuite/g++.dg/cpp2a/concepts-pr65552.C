<<<<<<< HEAD:gcc/testsuite/g++.dg/concepts/pr65552.C
// { dg-do compile { target c++17 } }
// { dg-options "-fconcepts" }
=======
// { dg-options "-std=c++2a" }
>>>>>>> eda685858ca... move more ported tests:gcc/testsuite/g++.dg/cpp2a/concepts-pr65552.C

template<typename T>
concept Concept =
  requires () {
    typename T::member_type1;
    typename T::member_type2;
  };

struct model {
  using member_type1 = int;
  using member_type2 = int;
};

template<Concept C>
struct S {};

S<model> s;
