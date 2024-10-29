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

Function overloads can conflict with generic functions. To resolve such ambiguities, Cortecs prioritizes more specific instantiations over generic ones.

```cortecs
function foo<T>(x: T) {
    unused(x)
    println("Calling generic foo")
}

function foo(x: I32) {
    println("Calling foo specialized for I32: {x}")
}

test overloadingResolution {
    foo(1)
    foo(1.2)
}

// stdout
// Calling foo specialized for I32: 1
// Calling generic foo
```

In cases where no definition is more specific than another, Cortecs resolves the ambiguity by prioritizing the arguments from left to right.

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

## Parsing

Generics in Cortecs are designed to be:
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

```cortecs
// Example 1: Generic function application
function foo<T>(x: T): T {
    return x
}
let x = f(10)


// Example 3: Binary operator <>
func f(x: Int) -> Int {
    return x
}
let x = f<>(10) // x is 10 (assuming <> is a no-op)

// Example 4: Unary and binary operators
func f(x: Int) -> Int {
    return x
}
let x = f< >(10) // x is 10 (assuming < is a no-op and > is a unary operator)

// Example 5: Mixed operators
func f(x: Int) -> Int {
    return x
}
let x = f(10)<5>(20) // x is 15 (assuming < and > are overloaded for addition)

// Error handling examples
// let x = f < // Syntax error: expected type name or expression
// let x = f(10)<Int>(20) // Syntax error: unexpected type name Int
```

As well, the parser must gracefully handle error cases with clear error messages

* `let x = f <` produces the syntax error `expected type name or expression`
* `let x = f(x)<T>(y)` produces the syntax error `unexpected type name T`