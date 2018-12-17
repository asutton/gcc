<<<<<<< HEAD:gcc/testsuite/g++.dg/concepts/pr71368.C
// { dg-do compile { target c++17 } }
// { dg-options "-fconcepts" }
=======
// { dg-options "-std=c++2a" }
>>>>>>> eda685858ca... move more ported tests:gcc/testsuite/g++.dg/cpp2a/concepts-pr71368.C

struct inner;

template<typename X> concept CompoundReq = requires {
    // fine with concrete type in trailing type, i.e. inner& instead of X&
    { X::inner_member() } -> X&;
};

template<typename X> concept Concept = requires {
    { X::outer_member() } -> CompoundReq;
};

struct inner { static inner& inner_member(); };
struct outer { static inner outer_member(); };

int main()
{
    // fine
    static_assert( CompoundReq<inner> );
    static_assert( CompoundReq<decltype( outer::outer_member() )> );

    // ICE
    static_assert( Concept<outer> );
}
