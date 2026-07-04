#ifndef OPAQUE_ANIMAL_INTERNAL_H
#define OPAQUE_ANIMAL_INTERNAL_H

/*
 * INTERNAL HEADER — NOT PART OF THE PUBLIC API.
 *
 * Include this file only from concrete implementation files such as:
 *   - opaque_dog.c
 *   - opaque_cat.c
 *
 * Client code (for example opaque_demo.c) must NOT include this header.
 *
 * WHY THIS FILE EXISTS:
 * Dog and Cat need to:
 *   - initialize the hidden Animal part;
 *   - register their vtables;
 *   - downcast Animal* back to Dog* or Cat* inside virtual methods.
 *
 * Real C libraries often have the same split:
 *   public header      -> safe client-facing API
 *   internal header      -> shared implementation details
 *   .c files             -> real struct definitions
 */

#include "opaque_animal.h"

typedef struct {
    void (*speak)(Animal *self);
    void (*describe)(Animal *self);
} AnimalVTable;

/*
 * Real definition of Animal.
 *
 * This is intentionally NOT in opaque_animal.h.
 * Only translation units that include this internal header can see it.
 */
struct Animal {
    const char *name;
    const AnimalVTable *vtable;
};

void animal_init(Animal *self, const char *name, const AnimalVTable *vtable);

#endif
