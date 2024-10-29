# Generics



## Records

```bnf
<field_declaration> ::= <name> ':' <type>
<field_declarations> ::= <field_declaration> ('\n' <field_declaration>)*

<field_instantiation> ::= <name> ':' <expression>
<field_instantiations> ::= <field_instantiation> ('\n' <field_instantiation>)*

<record_declaration> ::= 'record' <type> ('<' <types> ('where' <constraints>)? '>')? '{' <field_declarations> '}'
<record_construction> ::= <type> ('<' <types> '>') '{' <field_instantiations> '}'
```

Records are analogous to C structs, providing a way to group related data together. 

```cortecs
record RecordsName {
    field1: Type1
    field2: Type2
    ...
    fieldN: TypeN
}
```

When parsing the construction of a generic record, there is no need for disambiguation because binary operators are not valid immediately following a type name.

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

* MVP1 will only support generic compiliation in the form of monomorphization

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
