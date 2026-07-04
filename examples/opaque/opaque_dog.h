#ifndef OPAQUE_DOG_H
#define OPAQUE_DOG_H

#include "opaque_animal.h"

/*
 * PUBLIC Dog API.
 *
 * Like Animal, Dog is exposed only as an incomplete type.
 *
 * Client code can hold:
 *   Dog *dog;
 *
 * but cannot see fields such as:
 *   dog->breed
 *   dog->base
 *
 * Try uncommenting forbidden lines in opaque_demo.c to see the compiler
 * protect the internal layout.
 */

typedef struct Dog Dog;

/*
 * Factory function.
 *
 * This replaces:
 *   Dog dog1;
 *   dog_init(&dog1, ...);
 *
 * from the open-struct demo.
 *
 * Because Dog is incomplete in client code, the object must be allocated
 * inside the library, usually with malloc().
 */
Dog *dog_create(const char *name, const char *breed);

/*
 * Explicit upcast from concrete type to base handle.
 *
 * Main demo equivalent:
 *   Animal *animal = &dog1.base;
 *
 * Here the client is not allowed to know that Dog contains a field named
 * "base", so the upcast is provided as an API function.
 */
Animal *dog_as_animal(Dog *dog);

#endif
