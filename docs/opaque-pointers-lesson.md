# Lesson: Opaque Pointers in C

Italian version: [opaque-pointers-lesson.it.md](opaque-pointers-lesson.it.md)

This lesson belongs to the **Canis Polymorphicus Manualis** lab.

It explains the opaque pointer pattern implemented in:

```text
examples/opaque/
```

Run the companion demo:

```bash
make run-opaque
```

Read the source files in this order:

1. `opaque_animal.h` — public API, incomplete type
2. `opaque_animal_internal.h` — internal details for concrete types only
3. `opaque_animal.c` — real struct definition (hidden)
4. `opaque_dog.h` / `opaque_dog.c` — concrete Dog behind a handle
5. `opaque_cat.h` / `opaque_cat.c` — concrete Cat behind a handle
6. `opaque_demo.c` — client code that uses only the public API

Compare with the main demo at the repository root, which uses **open structs**.

---

## 1. What you already know

The main lab shows manual OOP in C with **open structs**:

```c
struct Animal {
    const char *name;
    const AnimalVTable *vtable;
};
```

Anyone who includes `animal.h` can:

- see every field;
- allocate `Dog dog1;` on the stack;
- write `dog1.breed` directly;
- upcast manually with `&dog1.base`.

That transparency is excellent for learning. You see every gear.

But real C libraries often do the opposite: they hide the struct layout completely.

---

## 2. The core idea

An **opaque pointer** is a pointer to a type whose definition is hidden from the client.

Public header:

```c
typedef struct Animal Animal;
```

Implementation file:

```c
struct Animal {
    const char *name;
    const AnimalVTable *vtable;
};
```

The client knows that `Animal` exists.

The client does **not** know:

- how many fields it has;
- their order;
- its size;
- how to reach internal data.

So the client can only interact through functions:

```c
Animal *animal = dog_as_animal(dog);
animal_speak(animal);
animal_destroy(animal);
```

This is C's version of **information hiding** and **encapsulation**.

---

## 3. Incomplete types in C

This line:

```c
typedef struct Animal Animal;
```

creates an **incomplete type**.

The compiler knows the name `Animal`, but not its layout.

| Operation | Allowed? | Why |
|-----------|----------|-----|
| `Animal *p;` | yes | pointer size is always known |
| `Animal a;` | no | compiler needs full layout for stack storage |
| `p->name` | no | client does not know the fields |
| `sizeof(Animal)` | no | size is unknown |
| `void animal_speak(Animal *self);` | yes | function declaration only needs a pointer |

This is not a limitation. It is the mechanism that **forces** API usage.

---

## 4. The three layers of the pattern

### Layer A — public API (`.h` seen by clients)

Example: `opaque_animal.h`

- forward declaration only;
- function prototypes;
- no struct body;
- no vtable exposure.

### Layer B — hidden implementation (`.c`)

Example: `opaque_animal.c`

- full `struct Animal { ... }` definition;
- virtual dispatch;
- destroy logic.

### Layer C — internal header (optional, for concrete types)

Example: `opaque_animal_internal.h`

- shared between `opaque_dog.c` and `opaque_cat.c`;
- exposes vtable and init helpers;
- **must not** be included by client code.

Many real libraries have exactly this split:

```text
public API header
internal implementation header
.c files with the real structs
```

---

## 5. Create and destroy

Open struct version:

```c
Dog dog1;
dog_init(&dog1, "Fido", "Labrador");
/* memory freed automatically when dog1 goes out of scope */
```

Opaque version:

```c
Dog *dog = dog_create("Fido", "Labrador");
Animal *animal = dog_as_animal(dog);
animal_speak(animal);
animal_destroy(animal);
```

Important consequences:

1. objects usually live on the **heap** (`malloc`);
2. the client receives a **handle** (`Dog *`);
3. the client must call **`animal_destroy`**;
4. forgetting destroy = memory leak.

There are no real constructors or destructors in C.

`create` and `destroy` are the manual replacement.

---

## 6. Polymorphism stays the same

This is easy to misunderstand.

Opaque pointers change **visibility**, not the OOP mechanism.

Inside `opaque_dog.c`:

- `Dog` still embeds `Animal base` as first field;
- `dog_vtable` still exists;
- `dog_speak()` still receives `Animal *` and downcasts to `Dog *`;
- `animal_speak()` still dispatches through the vtable.

So:

```text
same vtable
same virtual dispatch
same first-field inheritance trick
different boundary between public API and private implementation
```

---

## 7. Upcast through an explicit API

Main demo:

```c
Animal *animal = &dog1.base;
```

Opaque demo:

```c
Animal *animal = dog_as_animal(dog);
```

Both do the same thing internally.

The difference is who is allowed to know about `base`:

- open struct: everyone;
- opaque struct: only implementation code.

That is why `dog_as_animal()` exists.

It documents the upcast and keeps layout knowledge inside the library.

---

## 8. Side-by-side comparison

| Topic | Main demo (open structs) | Opaque demo |
|-------|--------------------------|-------------|
| Struct layout | visible in `.h` | hidden in `.c` |
| Typical allocation | stack | heap |
| Object creation | `dog_init(&dog, ...)` | `dog = dog_create(...)` |
| Cleanup | automatic at scope end | `animal_destroy(...)` |
| Field access | direct (`dog.breed`) | forbidden from client code |
| Upcast | `&dog.base` | `dog_as_animal(dog)` |
| Downcast | anywhere | only inside `.c` files |
| Change internal layout | may break clients | clients usually unaffected |
| Learning value | see all internals | see professional API style |

---

## 9. Advantages

### Encapsulation

Clients cannot write:

```c
dog->breed = "Hacked";
animal->vtable = NULL;
```

Those lines should not compile.

### API stability

You can add fields to `struct Dog` in the `.c` file without forcing every client to recompile against a new public layout.

This matters for shared libraries and long-lived C APIs.

### Real-world familiarity

Many professional C APIs use opaque handles:

- `FILE *`
- `SDL_Window *`
- `SSL *` in OpenSSL
- `sqlite3 *` in SQLite

When you see:

```c
typedef struct Foo Foo;
```

you should recognize the pattern immediately.

---

## 10. Limits

Opaque pointers are not free.

### No stack objects of incomplete type

You cannot write:

```c
Dog dog1;
```

You must use:

```c
Dog *dog = dog_create(...);
```

### Explicit ownership

The client owns the responsibility to destroy what it creates.

### More boilerplate

Every type tends to need:

- `create`
- optional `as_animal`
- shared or type-specific `destroy`

### Debugging is less direct

From client code you no longer inspect fields casually in the debugger unless you look inside the implementation files.

### Embedded contexts

In some embedded systems, open structs on static storage are preferred because memory is fully controlled.

Opaque handles are still useful when wrapping a driver or hiding a complex device struct.

---

## 11. How this lab is organized

This repository keeps experiments separate on purpose:

```text
main demo           -> manual OOP with open structs
container_of demo   -> recover outer struct when base is not first
opaque demo         -> encapsulation and professional-style handles
```

They answer different questions:

| Experiment | Question |
|------------|----------|
| Main demo | How do you build OOP manually in C? |
| `container_of` | How do you recover the container from an inner pointer? |
| Opaque demo | How do you hide implementation details from API users? |

Do not merge them too early. One concept at a time.

---

## 12. Study checklist

After reading the lesson and the source files, you should be able to answer:

1. Why does `Animal a;` fail in client code but `Animal *a;` works?
2. Where is the real `struct Animal` defined?
3. Who is allowed to downcast `Animal *` to `Dog *`?
4. Why does the opaque demo use `malloc` and `free`?
5. What stays the same between the main demo and the opaque demo?
6. Name one real C library that uses opaque handles.
7. Why is `opaque_animal_internal.h` not part of the public API?

Suggested exercise:

- try adding a forbidden line in `opaque_demo.c`, such as `dog->breed`, and confirm the compiler rejects it;
- compare that with the main demo, where the same line is legal.

---

## 13. Suggested reading order in this repository

1. [README.md](../README.md) — open struct demo
2. this lesson
3. `examples/opaque/*.c` and `examples/opaque/*.h`
4. [README.md](../README.md) — `container_of` section
5. `examples/container_of_demo.c`

That path moves from visible internals to hidden APIs to low-level pointer recovery.

---

## 14. Golden rule

The main demo teaches:

```text
Here is exactly how the machine works.
```

The opaque demo teaches:

```text
Here is how you hide the machine behind a safe handle.
```

Both skills matter in systems programming.

C gives you the freedom to choose.

Then it sends the invoice if you choose badly.
