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
concept bool C3 = true; // { dg-error "expected identifier" }
template<typename T> 
bool concept C3 = true; // { dg-warning "deprecated as a decl-specifier" }

template<typename T>
concept C4 = true; // OK
template<typename T>
concept C4 = true; // { dg-error "redefinition" }
template<typename T, typename U>
concept C4 = true; // { dg-error "different template parameters" }
template<int>
concept C4 = true; // { dg-error "different template parameters" }
int C4 = 0; // { dg-error "different kind of symbol" }

int C5 = 0;
template<typename T>
concept C5 = true; // { dg-error "different kind of symbol" }

template<typename T>
concept True = true;

template<typename T>
concept False = false;

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
