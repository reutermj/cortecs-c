# Generics

## Functions

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

Generics in Cortecs are designed to be easily and unambiguously parsed using LL parsing without type information. The syntax must also maintain the use of `[...]` as the accessor operator and allow operator overloading for `<` and `>`. Typically, these requirements are unsatisfiable, but Cortecs addresses them by enforcing strict naming conventions: variable and function names must start with a lowercase letter, whereas type names and namespaces must start with an uppercase letter.

To disambiguate the use of `<` in the syntax, the parser employs a simple algorithm. If the first non-whitespace token following the `<` is a type name or namespace, it is interpreted as a generic function application. Otherwise, it is treated as a binary operator. The following are examples illustrating how the parser distinguishes between various uses of `<`:

* `let x = f<T>(x)` is parsed as generic function application.
* `let x = f<t>(x)` is parsed with both `<` and `>` as binary operators, and `<` is applied first.
* `let x = f<>(x)` is parsed with `<>` as a binary operator.
* `let x = f< >(x)` is parsed with `<` as a binary operator and `>` as a unary operator.
* `let x = f(x)<y>(z)` is parsed with both `<` and `>` as binary operators, and `<` is applied first.

As well, the parser must gracefully handle error cases with clear error messages

* `let x = f <` produces the syntax error `expected type name or expression`
* `let x = f(x)<T>(y)` produces the syntax error `unexpected type name T`

### Overloading

Cortecs permits function overloading, and function overloads can conflict with generic functions. For example, both `foo<T>(x: T)` and `foo(x: I32)` can accept an `I32` argument. To resolve such ambiguities, Cortecs prioritizes more specific instantiations over generic ones. For example, when calling `foo(1)`, the function `foo(x: I32)` is chosen over the generic function `foo<T>(x: T)`. 

In cases where neither definition is more specific than the other, Cortecs resolves the ambiguity by prioritizing the arguments from left to right. For example, the function `foo<T>(x: I32, y: T)` is chosen over `foo<T>(x: T, y: I32)` when calling `foo(1, 2)`.

## Records

```bnf
<field_declaration> ::= <name> ':' <type>
<field_declarations> ::= <field_declaration> ('\n' <field_declaration>)*

<field_instantiation> ::= <name> ':' <expression>
<field_instantiations> ::= <field_instantiation> ('\n' <field_instantiation>)*

<record_declaration> ::= 'record' <type> ('<' <types> ('where' <constraints>)? '>')? '{' <field_declarations> '}'
<record_construction> ::= <type> ('<' <types> '>') '{' <field_instantiations> '}'
```

Records are analogous to C structs, providing a way to group related data together. When parsing the construction of a generic record, there is no need for disambiguation because binary operators are not valid immediately following a type name.

## Constraints

* used to constrain valid types for a given function/record generic type parameters
* defines a set of functions that must be defined for a given set of types
* unlike golang interfaces, constraints dont define a new type
* valid golang code where an interface is used to create a slice that stores two different structs
```golang
type Hashable interface {
	Hash() uint64
}

type Foo struct {
    hash uint64
}

func (this Foo) Hash() uint64 {
	return this.hash
}

type Bar struct {
    hash uint64
}

func (this Bar) Hash() uint64 {
	return this.hash
}

func main() {
	foo := Foo { 
		hash: 1,
	}
	bar := Bar { 
		hash: 2,
	}
	hashables := []Hashable { foo, bar }
	
	fmt.Printf("hello world %d %d\n", hashables[0].Hash(), hashables[1].Hash())
    // hello world 1 2
}
```

* Rust traits have a priviledged `self` type parameter.
* All type parameters of a Cortecs constraint are equal.
* The priviledged `self` type param in a rust trait does allow for definition of return value overloading
  * ex `String::from("a string")`
  * cortecs doesnt have a good way for encoding return value overloading
    * is this actually a problem?
    * probably not to be solved in MVP1 at the very least



## Monomorphization 

### Diamond Problem

```
Dependencies
    Foo
   /   \
Bar     Baz
   \   /
    Fiz
```

Take the following as an example: The module `Foo` defines a generic function `generic<T>(x: I32)` and does not reference `generic<I32>(x: I32)`. As a result, `Foo` does not need to instantiate `generic<I32>(x: I32)` in the binary, and it does not do so.

Both `Bar` and `Baz` reference `generic<I32>(x: I32)`. However, neither module depends on the other. This means that module `Bar` may not be linked to all the modules that module `Baz` is linked to, and vice versa. During the compilation of module `Bar`, there is no knowledge of the existence of module `Baz` in downstream dependencies, and the same is true for module `Baz` with respect to module `Bar`. Consequently, both modules `Bar` and `Baz` need to instantiate `generic<I32>(x: I32)` in their binaries. The monomorphization scheme will generate the same name for both instantiations of `generic<I32>(x: I32)`: `generic_1Cortecs_I32_2`.

The module `Fiz` depends on both `Bar` and `Baz`. Since both `Bar` and `Baz` instantiate `generic<I32>(...)`, a name conflict arises when statically linking `Bar` and `Baz`, resulting in the error: `Fiz.pic.o: multiple definition of 'generic_1Cortecs_I32_2'`. This name conflict does not occur when dynamically linking. Therefore, the likely course of action in MVP1 is to dynamically link everything and address the static linking issue at a later stage.
