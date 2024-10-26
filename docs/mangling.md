# Mangling

Cortecs employs name mangling to convert namespaces, function/record names, and generics into valid C names. This process is facilitated through the use of C preprocessor macros. This document serves multiple purposes: it explains the use of name mangling macros in the C Intermediate Represetation (CIR), provides a details of the mapping from Cortecs names to the CIR, and outlines the reverse mapping from the CIR back to Cortecs names.

# Cortecs to CIR

## Functions

```
foo.cortecs

namespace Cortecs

function foo() { ... }
function bar(x: I32) { ... }
function baz(x: I32): I64 { ... }
function biz(x: I32, y: F32): I64 { ... }
```

```
foo.h

void CN(Cortecs, foo)();
void CN(Cortecs, bar)(CN(Cortecs, I32));
CN(Cortecs, I64) CN(Cortecs, baz)(CN(Cortecs, I32));

foo.c

void CN(Cortecs, foo)() { ... }
void CN(Cortecs, bar)(CN(Cortecs, I32) x) { ... }
CN(Cortecs, I64) CN(Cortecs, baz)(CN(Cortecs, I32) x) { ... }
CN(Cortecs, I64) CN(Cortecs, biz)(CN(Cortecs, I32) x, CN(Cortecs, F32) y) { ... }
```

### Overloading

Cortecs allows function overloading

```
foo.cortecs

namespace Cortecs

function foo() { ... }
function foo(x: I32, y: F32): F32 { ... }
function foo(y: F32): F32 { ... }
```

```
foo.h

void CN(Cortecs, foo)();
void CN(Cortecs, foo)();
```

### Generics

```
foo.cortecs:

namespace Cortecs

function foo[T](x: T) { ... }

function bar() {
    foo(1)
    foo(1.1)
}
```

```
foo.template.h:

void CN(Cortecs, foo)(TYPE_PARAM_T x);

foo.template.c:

void CN(Cortecs, foo)(TYPE_PARAM_T x) { ... }

foo.h:

#define TYPE_PARAM_T CN(Cortecs, I32)
#include "foo.template.h"

#define TYPE_PARAM_T CN(Cortecs, F32)
#include "foo.template.h"

void CN(Cortecs, bar)();

foo.c:

#define TYPE_PARAM_T CN(Cortecs, I32)
#include "foo.template.c"

#define TYPE_PARAM_T CN(Cortecs, F32)
#include "foo.template.c"

void CN(Cortecs, bar)() {
    CN(Cortecs, foo, CTP(CN(Cortecs, I32)))(1)
    CN(Cortecs, foo, CTP(CN(Cortecs, F32)))(1.1)
}
```

## Records

```
namespace Cortecs

record Foo {
    ...
}

function bar() {
    Foo foo = ...
}
```

Translates into the CIR as

```
typedef struct {
    ...
} cortecs_name(Cortecs, Foo);

void cortecs_name(Cortecs, bar)() {
    cortecs_name(Cortecs, Foo) foo = ...
}
```

### Generics

```
foo.cortecs

namespace Cortecs

record Foo[S, T] {
    ...
}

function bar() {
    let baz: Foo[I32, I64] = ...
    let biz: Foo[F32, F64] = ...
}
```

translates into the CIR as

```
foo.template.h:

typedef struct {
    ...
} CN(Cortecs, Foo, CTP(TYPE_PARAM_T, TYPE_PARAM_S));

foo.h:

#define TYPE_PARAM_T CN(Cortecs, I32)
#define TYPE_PARAM_S CN(Cortecs, I64)
#include "foo.template.h"

#define TYPE_PARAM_T CN(Cortecs, F32)
#define TYPE_PARAM_S CN(Cortecs, F64)
#include "foo.template.h"

void CN(Cortecs, bar)();

foo.c:

void CN(Cortecs, bar)() {
    CN(Cortecs, Foo, CTP(CN(Cortecs, I32), CN(Cortecs, I64))) baz = ...
    CN(Cortecs, Foo, CTP(CN(Cortecs, F32), CN(Cortecs, F64))) biz = ...
}
```

