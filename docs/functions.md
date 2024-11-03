# Functions

```bnf
<name> ::= [a-z][a-zA-Z]*
<type_name> ::= [A-Z][a-zA-Z]*
<namespace> ::= [A-Z][a-zA-Z]*
<namespaces> ::= <namespace> (':' <namespace>)*
<type> ::= <type_name> | <type_name> '<' <types> '>'
<types> ::= <type> (',' <type>)*
<constraint> ::= <type> '<' <types> '>'
<constraints> ::= <constraints> (',' <constraints>)*
<param> ::= <name> ':' <type>
<params> ::= <empty> | <param> (',' <param>)*
<args> ::= <empty> | <expression> (',' <expression>)*
<block> ::= '{' ... '}'

<function_declaration> ::= 'function' <name>('<' <types> ('where' <constraints>)? '>')? '(' <params> ')' (':' <type>)? <block>
<function_call> ::= (<namespaces> ':')? <name> ('<' <types> '>')? '(' <args> ')'
```

Functions are callable chunks of code.

```cortecs
function greet() {
    println("Hello!")
}

test greeting {
    greet()
}

// stdout
// Hello!
```

## Parameters

Functions can define parameters to accept input values. These parameters can be of various types, and the function can use them within its body.

```cortecs
function greetPerson(name: String) {
    println("Hello, {name}!")
}

function greetPeople(name1: String, name2: String) {
    println("Hello, {name1} and {name2}!")
}

test greeting {
    greetPerson("Alice")
    greetPerson("Bob")
    greetPeople("Alice", "Bob")
}

// stdout
// Hello, Alice!
// Hello, Bob!
// Hello, Alice and Bob!
```

## Return Values

Functions in Cortecs can also return values to the caller.

```cortecs
function add(x: I32, y: I32): I32 {
    return x + y
}

test addition {
    let result = add(2, 3)
    println("Result of addition: {result}")
}

// stdout
// Result of addition: 5
```

## Recursive Functions

Cortecs supports recursive functions, which are functions that call themselves. 

```cortecs
function factorial(n: I32): I32 {
    if n <= 1 {
        return 1
    } else {
        return n * factorial(n - 1)
    }
}

test recursion {
    let result = factorial(5)
    println("Factorial of 5: {result}")
}

// stdout
// Factorial of 5: 120
```

## Generic Functions

Cortecs allows for generic functions that define type parameters and can operate on many different types passed to it.

```
function greet(name: String) {
    println("Hello, {name}!")
}

function add(x: I32, y: I32): I32 {
    return x + y
}

function identity<T>(t: T): T {
    return t
}

test generic {
    greet(identity("Alice"))
    let sum = add(identity(2), 3)
    println("Result of addition: {result}")
}

// stdout
// Hello, Alice!
// Result of addition: 5
```

## Constrained Generic Functions

Cortecs allows for generic functions to be constrained by constraints, which declare a set of functions defined for the generic type parameters of the function.

```
constraint Addable[S, T, R] {
    add(x: S, y: T): R
}

function addTogether<S, T where Addable[S, T, T]>(x: S, y: T): T {
    return add(x, y)
}

function add(x: I32, y: I32): I32 {
    return x + y
}

function add(x: I32, y: F32): I32 {
    return toF32(x) + y
}

test constrainedGenerics {
    val i32Result = addTogether(2, 3)
    println("Result of I32 addition: {i32Result}")
    val f32Result = addTogether(3, 4.1)
    println("Result of F32 addition: {f32Result}")
}

// stdout
// Result of I32 addition: 5
// Result of F32 addition: 7.1
```

## Overloading

Cortecs permits function overloading, allowing multiple functions with the same name but different parameter types.

```cortecs
function foo(x: I32) {
    println("Calling foo specialized for I32: {x}")
}

function foo(x: F32) {
    println("Calling foo specialized for F32: {x}")
}

test overloading {
    foo(1)
    foo(1.1)
}

// stdout
// Calling foo specialized for I32: 1
// Calling foo specialized for F32: 1.1
```

Function overloads can conflict with generic functions. Cortecs resolves these ambiguities by prioritizing more specific overloads over generic ones. Specifying the type arguments overrides this default.

```cortecs
function foo<T>(x: T) {
    unused(x)
    println("Calling generic foo")
}

function foo(x: I32) {
    println("Calling foo specialized for I32: {x}")
}

function foo<T>(x: I32, y: T) {...}
function foo<T>(x: T, y: I32) {...}
function foo(x: I32, y: I32) {...}

test overloadingResolution {
    foo(1)
    foo(1.2)
    foo<I32>(1)
}

// stdout
// Calling foo specialized for I32: 1
// Calling generic foo
// Calling generic foo
```

In cases where no definition is more specific than another, Cortecs resolves the ambiguity by prioritizing the arguments from left to right. TODO: Figure if there are ways to override this disambiguation default.

```cortecs
function foo<T>(x: I32, y: T) {
    unused(y)
    println("Calling leftmost specialized foo: {x}")
}

function foo<T>(x: T, y: I32) {
    unused(x)
    println("Calling rightmost specialized foo: {y}")
}

test overloadingResolution {
    foo(1, 2.2)
    foo(1.1, 2)
    foo(1, 2)
}

// stdout
// Calling leftmost specialized foo: 1
// Calling rightmost specialized foo: 2
// Calling leftmost specialized foo: 1
```

# Design Considerations

## Parsing

The syntax of generics in Cortecs is designed to be:
* easily and unambiguously parsed using LL parsing without type information,
* maintain the use of `[...]` as the accessor operator, and
* allow operator overloading for `<` and `>`. 

Cortecs addresses these requirements by enforcing strict naming conventions: variable and function names must start with a lowercase letter, whereas type names and namespaces must start with an uppercase letter.

To disambiguate the use of `<` in the syntax, the parser employs a simple algorithm. If the first non-whitespace token following the `<` is a type name or namespace, it is interpreted as a generic function application. Otherwise, it is treated as a binary operator. The following are examples illustrating how the parser distinguishes between various uses of `<`:

* `let x = foo<T>(y)` is parsed as generic function application.
* `let x = foo<t>(y)` is parsed with both `<` and `>` as binary operators, and `<` is applied first.
* `let x = foo<>(y)` is parsed with `<>` as a binary operator.
* `let x = foo< >(y)` is parsed with `<` as a binary operator and `>` as a unary operator.
* `let x = foo(y)<z>(w)` is parsed with both `<` and `>` as binary operators, and `<` is applied first.

As well, the parser must gracefully handle error cases with clear error messages

* `let x = f <` produces the syntax error `expected type name or expression`
* `let x = f(x)<T>(y)` produces the syntax error `unexpected type name T`

## Representation in CIR

The Cortecs compiler must lower high level concepts of function overloading and generics into valid C Intermediate Represetion (CIR). In MVP1, monomorphization is the method for representing overloading and generics. The monomorphization must be able to represent:

* instantiations of generic functions,
* overloaded functions, and
* the ability to override default disambiguation of generic and overloaded function.

Name mangling must also have a direct reverse mapping back to the original Cortecs function.

Key:
* `_` -> separator
* `_0` -> open generic type argument list
* `_1` -> close generic type argument list
* `_2` -> open/close generic type parameter list

```
namespace Cortecs

function foo() -> 
    CN(Cortecs, foo) -> 
    Cortecs_foo

function foo(x: I32) -> 
    CN(Cortecs, foo, CT(I32)) -> 
    Cortecs_foo_1I32_2

function foo(x: Foo<Bar<I32>>, y: Baz<Biz<F32, I32>>) -> 
    CN(Cortecs, foo, CT(I32)) -> 
    Cortecs_foo_0Foo_0Bar_0I32_1_1_Baz_0Biz_0F32_I32_1_1_1

function foo<T>(x: T) instantiated for I32 -> 
    CN(Cortecs, foo, CG(I32), CT(T)) -> 
    Cortecs_foo_2I32_2_0T_1

function foo<T>(x: T) instantiated for F32 -> 
    CN(Cortecs, foo, CG(F32), CT(T)) -> 
    Cortecs_foo_2F32_2_0T_1

function foo(x: I32, y: F32) ->
    CN(Cortecs, foo, CT(I32, F32)) ->
    Cortecs_foo_0I32_F32_1
```