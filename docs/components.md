# Components

Components are user defined data types that allow the grouping of other data called fields.

```cortecs
component Position2 {
    x: F32
    y: F32
}

component Veclocity2 {
    x: F32
    y: F32
}
```

## Record Construction

Components are constructed with the following notation

```cortecs
component Position2 {
    x: F32
    y: F32
}

function main() {
    let pos = Position2 {
        x: 1.1,
        y: 2.2,
    }
}
```

## Accessing Fields

Component fields are accessed with dot notation.

```cortecs
component Position2 {
    x: F32
    y: F32
}

function main() {
    let pos = Position2 {
        x: 1.1,
        y: 2.2,
    }

    println("Position is ({pos.x}, {pos.y})")
}
```

## Generics

Cortecs allows for generic components that define type parameters and can group together many different types passed to it.

```cortecs
component Pair<S, T> {
    first: S
    second: T
}

function main() {
    let i32I32 = Pair {
        first: 1,
        second: 2,
    }

    let f32I32 = Pair {
        first: 1.1,
        second, 2,
    }
}
```
