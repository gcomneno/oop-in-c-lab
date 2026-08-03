# OOP in C Lab

Codename: **Canis Polymorphicus Manualis** 🐕⚙️

A small educational lab that shows how to simulate object-oriented programming concepts in plain C.

The goal is to make visible what usually hides behind concepts such as:

- classes;
- objects;
- inheritance;
- virtual methods;
- vtables;
- polymorphism;
- `self` / `this`.

The example is intentionally simple:

```text
Animal
├── Dog
└── Cat
```

Italian version: [README.it.md](README.it.md)

## Project layout

```text
.
├── animal.h   # base class / common interface
├── animal.c   # public virtual dispatch functions
├── dog.h      # concrete Dog type
├── dog.c      # Dog implementation + dog_vtable
├── cat.h      # concrete Cat type
├── cat.c      # Cat implementation + cat_vtable
├── main.c     # polymorphic usage through Animal*
├── docs/      # standalone lessons
├── examples/  # separate experiments
└── Makefile   # minimal build helper
```

## Concept 1: a struct as an object

In C, a `struct` is just a data container.

It does not have:

- real methods;
- constructors;
- destructors;
- inheritance;
- `private`;
- `public`;
- `protected`.

However, C gives us enough low-level tools to manually build some object-oriented mechanisms.

In this lab, `Animal` contains:

```c
struct Animal {
    const char *name;
    const AnimalVTable *vtable;
};
```

`name` is shared state.

`vtable` is a pointer to the table of virtual methods.

## Concept 2: simulated inheritance

`Dog` and `Cat` both contain `Animal base` as their first field:

```c
typedef struct {
    Animal base;
    const char *breed;
} Dog;
```

```c
typedef struct {
    Animal base;
    int lives;
} Cat;
```

This simulates the idea:

```text
Dog is an Animal
Cat is an Animal
```

## Concept 3: the first-field trick

Fundamental rule:

```text
The address of a struct is the same as the address of its first member.
```

So, if `Animal base` is the first field of `Dog`:

```c
&dog == &dog.base
```

This lets us treat a `Dog` as an `Animal`:

```c
Animal *animal = &dog.base;
```

And, carefully, go back:

```c
Dog *dog = (Dog *)animal;
```

This downcast is safe only if that `Animal *` really points to the base part of a `Dog`.

If it actually points to a `Cat`, C will not save you: that is undefined behavior. Trapdoor open.

## Memory layout demo

The demo prints the addresses of each concrete object and its embedded `base` field:

```text
Memory layout demo:
  dog object address      = 0x...
  dog.base address        = 0x...
  same address?           = yes

  cat object address      = 0x...
  cat.base address        = 0x...
  same address?           = yes
```

The exact addresses change on every run.

The important part is that each object address matches the address of its first field.

That is the practical reason why this lab can safely pass `&dog.base` or `&cat.base` around as `Animal *`.

## container_of-style experiment

The main demo relies on the first-field trick: `Animal base` is the first member of `Dog` and `Cat`, so the concrete object address and the embedded base address are the same.

The separate `container_of` experiment shows a more advanced case:

```c
typedef struct {
    const char *label;
    Animal base;
    int marker;
} WeirdDog;
```

Here, `Animal base` is not the first field.

That means:

```text
&weird_dog != &weird_dog.base
```

To recover the outer `WeirdDog *` from an inner `Animal *`, the demo uses `offsetof` and a small `container_of`-like macro:

```c
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))
```

The idea is:

1. `offsetof(WeirdDog, base)` tells us how many bytes separate the start of `WeirdDog` from its `base` member.
2. `char *` pointer arithmetic lets us move byte by byte.
3. subtracting that offset from the member pointer gives us the address of the outer struct.

This is a powerful low-level C technique, often associated with systems code and the Linux kernel style.

Use it carefully. It is only valid when the pointer really points to that exact member inside an object of the requested type.

Run the separate experiment with:

```bash
make run-container-of
```

## Opaque pointer experiment

The main demo keeps struct layouts public on purpose. You can see every field, every vtable, and every upcast trick.

The opaque pointer experiment shows the opposite style: a professional C API that hides implementation details behind handles.

Start with the lesson:

- English: [docs/opaque-pointers-lesson.md](docs/opaque-pointers-lesson.md)
- Italian: [docs/opaque-pointers-lesson.it.md](docs/opaque-pointers-lesson.it.md)

Then read the source files in:

```text
examples/opaque/
```

Run the separate experiment with:

```bash
make run-opaque
```

Quick comparison:

| Topic | Main demo | Opaque demo |
|-------|-----------|-------------|
| Struct layout | visible in `.h` | hidden in `.c` |
| Allocation | stack | heap |
| Field access | direct | API only |
| Upcast | `&dog.base` | `dog_as_animal(dog)` |
| Cleanup | automatic | `animal_destroy(...)` |

The OOP mechanism is the same. What changes is the boundary between public API and private implementation.

## Concept 4: upcast and downcast

Upcast:

```text
Dog* -> Animal*
```

This means looking at a concrete object through its base type.

Example:

```c
Animal *animal = &dog.base;
```

Downcast:

```text
Animal* -> Dog*
```

This means going back from the base type to the concrete type.

Example:

```c
Dog *self = (Dog *)animal;
```

In this lab, the downcast happens inside `dog_speak` and `dog_describe`.

## Concept 5: virtual methods

In an object-oriented language, we might write something like:

```cpp
animal->speak();
```

C has no such syntax.

Instead, we expose a public function:

```c
void animal_speak(Animal *self)
{
    self->vtable->speak(self);
}
```

This function:

1. receives the object;
2. reads its vtable;
3. finds the concrete method;
4. calls that method while passing `self`.

## Concept 6: vtable

A vtable is a table of function pointers.

In this lab:

```c
typedef struct {
    void (*speak)(Animal *self);
    void (*describe)(Animal *self);
} AnimalVTable;
```

`Dog` has its own table:

```c
static const AnimalVTable dog_vtable = {
    .speak = dog_speak,
    .describe = dog_describe,
};
```

`Cat` has its own table:

```c
static const AnimalVTable cat_vtable = {
    .speak = cat_speak,
    .describe = cat_describe,
};
```

All `Dog` instances share the same `dog_vtable`.

All `Cat` instances share the same `cat_vtable`.

## Concept 7: explicit self

In C++ / C# / Java, the current object reference is passed implicitly.

In C, we pass it manually.

That is why virtual functions look like this:

```c
void dog_speak(Animal *animal)
{
    Dog *self = (Dog *)animal;

    printf("%s the %s says: Woof!\n", self->base.name, self->breed);
}
```

`self` is our manual version of `this`.

## Run the demo

Build and run:

```bash
make run
```

Clean the binary:

```bash
make clean
```

## Expected output

Memory addresses will change between runs.

The important part is that:

- both `Dog` instances point to the same vtable;
- both `Cat` instances point to the same vtable;
- Dog and Cat use different vtables.

Example:

```text
Dog vtables:
  dog1.base.vtable = 0x...
  dog2.base.vtable = 0x...

Cat vtables:
  cat1.base.vtable = 0x...
  cat2.base.vtable = 0x...

Polymorphic calls:
Fido is a dog of breed Labrador.
Fido the Labrador says: Woof!
Micia is a cat with 9 lives left.
Micia has 9 lives and says: Meow!
Rex is a dog of breed German Shepherd.
Rex the German Shepherd says: Woof!
Nebbia is a cat with 7 lives left.
Nebbia has 7 lives and says: Meow!
```

## Core idea

C does not provide native OOP.

But it lets you manually build many OOP mechanisms.

The price is that the compiler will not protect you very much.

Golden rule:

```text
C lets you do it.
Then it sends the invoice.
```
