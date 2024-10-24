# Monomorphization

## Diamond Problem

```
Dependencies
    Foo
   /   \
Bar     Baz
   \   /
    Fiz
```

Take the following as an example: The module `Foo` defines a generic function `generic[T](x: I32)` and does not reference `generic[I32](x: I32)`. As a result, `Foo` does not need to instantiate `generic[I32](x: I32)` in the binary, and it does not do so.

Both `Bar` and `Baz` reference `generic[I32](x: I32)`. However, neither module depends on the other. This means that module `Bar` may not be linked to all the modules that module `Baz` is linked to, and vice versa. During the compilation of module `Bar`, there is no knowledge of the existence of module `Baz` in downstream dependencies, and the same is true for module `Baz` with respect to module `Bar`. Consequently, both modules `Bar` and `Baz` need to instantiate `generic[I32](x: I32)` in their binaries. The monomorphization scheme will generate the same name for both instantiations of `generic[I32](x: I32)`: `generic_1Cortecs_I32_2`.

The module `Fiz` depends on both `Bar` and `Baz`. Since both `Bar` and `Baz` instantiate `generic[I32](...)`, a name conflict arises when statically linking `Bar` and `Baz`, resulting in the error: `Fiz.pic.o: multiple definition of 'generic_1Cortecs_I32_2'`. This name conflict does not occur when dynamically linking. Therefore, the likely course of action in MVP1 is to dynamically link everything and address the static linking issue at a later stage.
