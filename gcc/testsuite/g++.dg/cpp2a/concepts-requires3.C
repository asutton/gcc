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

int driver_1()
{
  fn(S2{});
  return 0;
}

// req10.C
// Test implicit conversion requirements

template<typename T, typename U>
concept ConvertibleTo = requires(T& t) { {t} -> U&; };

struct B { };
class D : /*private*/ B { };

void driver_2()
{
  static_assert(ConvertibleTo<D, B>()); // { dg-error "not a function" }
  static_assert(ConvertibleTo<D, B>); // { dg-error "static assertion failed" }
}

// req11.C
template<typename T>
concept Streamable = requires (T t) { t; };

template<typename T>
concept Range = requires (T t) { t; };

// FIXME: There are two syntax errors here when there should be
// just one.
template<class T>
  requires Streamable<T> && !Range<T> // { dg-error "expected primary-expression|expected unqualified-id" }
void print1(const T& x) { }

template<class T>
  requires Streamable<T> && (!Range<T>)
void print2(const T& x) { }

void driver_3()
{
  print2("hello"); // { dg-error "cannot call" }
}


// req12.C

template <typename T, typename U>
concept SameAs = __is_same_as(T, U);

template <typename T>
concept C1 = requires(T t) {
  { t } -> SameAs<T>;
};

template <typename T>
  requires C1<T>
constexpr bool f1() { return true; }

static_assert(f1<char>());
static_assert(f1<int>());
static_assert(f1<double>());


template <typename T>
concept C2 = requires(T t) {
  { t } -> SameAs<T&>;
};

template <typename T>
  requires C2<T>
constexpr bool f2() { return true; }

static_assert(f2<int>()); // { dg-error "cannot call|does not satisfy placeholder constraints" }
