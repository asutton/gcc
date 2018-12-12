<<<<<<< HEAD:gcc/testsuite/g++.dg/concepts/member-concept.C
// { dg-do compile { target c++17 } }
// { dg-options "-fconcepts" }
=======
// { dg-options "-std=c++2a" }
>>>>>>> 594131fbad3... move ported tests; note more issues and needs port after fixes:gcc/testsuite/g++.dg/cpp2a/concepts-member-concept.C

struct Base {
  template<typename T>
    static concept bool D() { return __is_same_as(T, int); } // { dg-error "a concept cannot be a member function" }

  template<typename T, typename U>
    static concept bool E() { return __is_same_as(T, U); } // { dg-error "a concept cannot be a member function" }
};
