# Lesson: Type Identity and Checked Downcasting in C

Italian version: [type-identity-and-checked-downcasting.it.md](type-identity-and-checked-downcasting.it.md)

This lesson belongs to the **Canis Polymorphicus Manualis** lab.

It explains the runtime type identity and checked-downcast mechanism implemented in:

```text
animal.h
animal.c
dog.h
dog.c
cat.h
cat.c
examples/checked_downcast_demo.c
```

Run the companion demo:

```bash
make run-checked-downcast
```

---

## 1. The problem

The static type of a pointer is the type known by the compiler.

For example:

```c
Animal *animal = &dog.base;
```

The compiler sees an `Animal *`.

The real object in memory is still a `Dog`.

That concrete runtime identity is also called the **dynamic type**.

A downcast tries to recover the concrete pointer:

```text
Animal * -> Dog *
```

A raw C cast can express that conversion:

```c
Dog *dog = (Dog *)animal;
```

But the cast performs no runtime validation.

It changes how the compiler interprets the address. It does not prove that a `Dog` is actually there.

---

## 2. Why a wrong cast is dangerous

Suppose `animal` actually points to the base part of a `Cat`:

```c
Animal *animal = &cat.base;
Dog *dog = (Dog *)animal;
```

The compiler accepts this.

The resulting pointer still refers to the `Cat` object, but the program now interprets its bytes using the layout of `Dog`.

Reading:

```c
dog->breed
```

would interpret unrelated memory as a pointer to a string.

This is undefined behavior.

C does not automatically carry enough runtime type information to reject the mistake.

---

## 3. Explicit runtime type identity

The lab adds an explicit type tag:

```c
typedef enum {
    ANIMAL_TYPE_UNKNOWN = 0,
    ANIMAL_TYPE_DOG,
    ANIMAL_TYPE_CAT,
} AnimalType;
```

Every `Animal` stores its concrete identity:

```c
struct Animal {
    const char *name;
    const AnimalVTable *vtable;
    AnimalType type;
};
```

`dog_init()` installs `ANIMAL_TYPE_DOG`.

`cat_init()` installs `ANIMAL_TYPE_CAT`.

The value is maintained manually because plain C provides no automatic class metadata.

---

## 4. Why UNKNOWN is zero

The first enum value is:

```c
ANIMAL_TYPE_UNKNOWN = 0
```

This is deliberate.

A zero-initialized or uninitialized-as-zero `Animal` must not accidentally look like a valid `Dog` or `Cat`.

The separate `container_of` experiment embeds an `Animal` only to demonstrate member recovery. It has no virtual behavior and is not a concrete Dog/Cat object, so its tag is assigned explicitly as `ANIMAL_TYPE_UNKNOWN`.

It also gives `animal_get_type(NULL)` a safe result:

```c
AnimalType animal_get_type(const Animal *self)
{
    if (self == NULL) {
        return ANIMAL_TYPE_UNKNOWN;
    }

    return self->type;
}
```

---

## 5. Public type queries

The base API exposes simple predicates:

```c
bool animal_is_dog(const Animal *self);
bool animal_is_cat(const Animal *self);
```

They make the runtime question explicit:

```c
if (animal_is_dog(animal)) {
    /* the dynamic type is Dog */
}
```

The functions also return `false` for `NULL`.

---

## 6. Checked downcasting

`Dog` exposes:

```c
Dog *dog_from_animal(Animal *animal);
const Dog *dog_from_animal_const(const Animal *animal);
```

The implementation validates the tag before casting:

```c
Dog *dog_from_animal(Animal *animal)
{
    if (!animal_is_dog(animal)) {
        return NULL;
    }

    return (Dog *)animal;
}
```

The cast still exists.

The difference is that it happens only after the runtime identity has been checked.

The corresponding `Cat` helpers apply the same rule.

---

## 7. Failure is represented by NULL

A rejected downcast returns `NULL`:

```c
Dog *dog = dog_from_animal(cat_as_animal);

if (dog == NULL) {
    /* rejected safely */
}
```

This prevents the caller from dereferencing the object as the wrong concrete type.

The caller must check the result before using concrete fields.

---

## 8. Const-correct downcasting

The lab provides both mutable and read-only variants:

```c
Dog *dog_from_animal(Animal *animal);
const Dog *dog_from_animal_const(const Animal *animal);
```

The const helper preserves the original promise:

```c
const Animal *animal = &dog.base;
const Dog *dog = dog_from_animal_const(animal);
```

A `const Animal *` must not silently become a mutable `Dog *`.

---

## 9. Layout, identity and behavior are different things

Three mechanisms work together.

| Mechanism | Question answered |
|-----------|-------------------|
| First-field layout | Can this address physically represent the base part? |
| Type tag | Which concrete type is stored here? |
| Vtable | Which implementation should a virtual call execute? |

The first-field trick makes this address relationship possible:

```text
&dog == &dog.base
```

The type tag validates that the object is really a `Dog`.

The vtable selects `dog_speak()` or `cat_speak()` during virtual dispatch.

None of the three mechanisms replaces the others.

---

## 10. Checked casts inside virtual methods

A Dog virtual function receives `Animal *` because every vtable entry shares the base signature.

The implementation now performs a checked conversion:

```c
static void dog_speak(Animal *animal)
{
    Dog *self = dog_from_animal(animal);

    if (self == NULL) {
        return;
    }

    /* safe concrete access */
}
```

In a correctly initialized object, the check succeeds.

It also makes the hidden precondition visible and guards against inconsistent manually assembled objects.

---

## 11. Companion demo

The demo shows:

1. runtime type queries;
2. successful Dog and Cat downcasts;
3. rejected Cat-to-Dog and Dog-to-Cat casts;
4. const-preserving downcasting;
5. the difference between a C cast and a runtime proof.

Run it with:

```bash
make run-checked-downcast
```

---

## 12. Comparison with dynamic_cast

Languages with runtime type information can provide a built-in checked conversion.

A C++ example might use:

```cpp
Dog *dog = dynamic_cast<Dog *>(animal);
```

Plain C has no equivalent built into the language.

This lab manually supplies a tiny subset of that behavior through:

- an enum type tag;
- query helpers;
- concrete checked-cast functions;
- `NULL` on failure.

This is educational runtime type information, not a complete reflection system.

---

## 13. Limitations

The enum approach is intentionally small and explicit.

As the hierarchy grows:

- every concrete type must receive a unique enum value;
- constructors must install the correct tag;
- query and cast helpers multiply;
- plugins cannot easily add types without changing the shared enum;
- inheritance deeper than one level requires a more expressive relationship model;
- a corrupted or manually forged tag can still lie.

Production systems may use richer descriptors, class records, unique addresses, interface tables or code generation.

The lab avoids those mechanisms so the essential idea remains visible.

---

## 14. Core lesson

A C cast answers:

> How should the compiler interpret this address?

A checked downcast also asks:

> Is the object at this address really the requested runtime type?

The cast changes the view.

The type tag provides the missing proof.
