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
