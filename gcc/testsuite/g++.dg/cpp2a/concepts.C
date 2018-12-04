// { dg-do compile }
// { dg-options "-std=c++2a" }

static_assert(__cpp_concepts > 201507);

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
