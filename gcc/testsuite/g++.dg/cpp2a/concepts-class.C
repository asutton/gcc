// { dg-do compile }
// { dg-options "-std=c++2a" }

template<typename T>
concept Class = __is_class(T);

template<typename T>
concept Union = __is_union(T);

template<typename T>
concept One = sizeof(T) >= 4;

template<typename T>
concept Two = One<T> && sizeof(T) >= 8;

// Basic checks
template<typename T>
  requires true
struct ok { };

// FIXME: This error should be a note. That note is also on the wrong line.
template<typename T>
  requires false 
struct err { }; // { dg-error "never satisfied" }

ok<int> ok1;
err<int> err1; // { dg-error "invalid class name" }

// Redeclarations
template<typename T> 
  requires Class<T>
struct S1;

template<Class T> // { dg-error "template parameter | different constraints" }
struct S1 { };

template<typename T>
  requires Class<T>
struct S2;

template<typename T>
  requires Union<T> 
struct S2; // { dg-error "redeclaration | different constraints" }


// Check non-overlapping specializations
template<typename T>
struct S3 { static const int value = 0; };

template<typename T>
  requires Class<T>
struct S3<T> { static const int value = 1; };

template<typename T>
  requires Union<T>
struct S3<T> { static const int value = 2; };

struct S { };
union U { };

static_assert(S3<int>::value == 0, "");
static_assert(S3<S>::value == 1, "");
static_assert(S3<U>::value == 2, "");

// Check ordering of partial specializations
template<typename T>
struct S4 { static const int value = 0;  };

template<typename T>
  requires One<T>
struct S4<T> { static const int value = 1; };

template<typename T>
  requires Two<T>
struct S4<T> { static const int value = 2; };

struct one_type { char x[4]; };
struct two_type { char x[8]; };

// static_assert(S4<char>::value == 0, "");
static_assert(S4<one_type>::value == 1, "");
static_assert(S4<two_type>::value == 2, "");

// Specializations are more specialized.
template<typename T> requires Two<T> struct S5 { };
template<typename T> requires One<T> struct S5<T> { }; // { dg-error "does not specialize" }
