// { dg-do compile }
// { dg-options "-std=c++2a" }

// Change in grammar for the expression trailing `requires`.
template<typename T>
  requires true != false // { dg-error "expected unqualified-id" }
void f1(T)
{ }

template<typename T>
void f3(T) requires true != false // { dg-error "expected initializer" }
{ }

template<typename T>
  requires requires {
    requires true == false; // OK: logical-or-expressions allowed here.
  }
void f3(T)
{ }

template<typename T> 
concept bool C1 = true; // { dg-warning "deprecated as a decl-specifier" }
template<typename T> 
bool concept C1 = true; // { dg-warning "deprecated as a decl-specifier" }

template<typename T>
concept C2 = true; // OK
template<typename T>
concept C2 = true; // { dg-error "redefinition" }
template<typename T, typename U>
concept C2 = true; // { dg-error "different template parameters" }
template<int>
concept C2 = true; // { dg-error "different template parameters" }
int C2 = 0; // { dg-error "different kind of symbol" }

int C3 = 0;
template<typename T>
concept C3 = true; // { dg-error "different kind of symbol" }

// Checking

template<typename T>
concept True = true;

// FIXME: The error comes from calling f2 in driver. This should be a
// warning not an error.
template<typename T>
concept False = false; // { dg-error "never satisfied" }

static_assert(True<int>);
static_assert(False<int>); // { dg-error "static assertion failed" }

template<typename T>
  requires True<T>
int f1(T t) { return 0; }

template<typename T>
  requires False<T>
int f2(T t) { return 0; }

void driver()
{
  f1(0);
  f2(0); // { dg-error "cannot call function" }
}

// Ordering

template<typename T>
concept C4 = requires (T t) { t.f(); };

template<typename T>
concept C5 = C4<T> && requires (T t) { t.g(); };

template<typename T>
concept C6 = requires (T t) { t.f(); };

template<typename T> requires C4<T>
constexpr int algo(T t) { return 1; }

template<typename T> requires C5<T>
constexpr int algo(T t) { return 2; }

template<typename T> requires C4<T>
constexpr int ambig(T t) { return 1; }

template<typename T> requires C6<T>
constexpr int ambig(T t) { return 1; }

struct S1 {
  void f() { }
};

struct S2 : S1 {
  void g() { }
};

int driver_2()
{
  S1 x;
  S2 y;
  static_assert(algo(x) == 1);
  static_assert(algo(y) == 2);
  ambig(x); // { dg-error "call of overload | is ambiguous" }
}

