// PR c++/67240
<<<<<<< HEAD:gcc/testsuite/g++.dg/concepts/iconv1.C
// { dg-do compile { target c++17 } }
// { dg-options "-fconcepts" }
=======
// { dg-options "-std=c++2a" }
>>>>>>> eda685858ca... move more ported tests:gcc/testsuite/g++.dg/cpp2a/concepts-iconv1.C

int foo(int x)
{
    return x;
}
 
template <typename T>
concept C1 = requires (T x) {
    {foo(x)} -> int&;
};

template <typename T>
concept C2 = requires (T x) {
    {foo(x)} -> void;
};
 
static_assert( C1<int> );	// { dg-error "assert" }
static_assert( C2<int> );	// { dg-error "assert" }
