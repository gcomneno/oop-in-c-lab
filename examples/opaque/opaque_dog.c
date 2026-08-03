#include <stdio.h>
#include <stdlib.h>

#include "opaque_animal_internal.h"
#include "opaque_dog.h"

/*
 * REAL Dog definition — hidden from clients.
 *
 * The OOP trick is the same as in the main lab:
 * Animal base is the first field, so:
 *
 *   (Dog *)animal   is safe inside virtual methods
 *   dog_as_animal() is safe for upcast
 *
 * What changed is only who is allowed to know this layout.
 */

struct Dog {
    Animal base;
    const char *breed;
};

static void dog_speak(Animal *animal)
{
    /*
     * Downcast happens here, inside the implementation file.
     *
     * Client code must never do:
     *   Dog *dog = (Dog *)animal;
     *
     * because the client does not know Dog's layout.
     */
    Dog *self = (Dog *)animal;

    printf("%s the %s says: Woof!\n", self->base.name, self->breed);
}

static void dog_describe(Animal *animal)
{
    Dog *self = (Dog *)animal;

    printf("%s is a dog of breed %s.\n", self->base.name, self->breed);
}

static const AnimalVTable dog_vtable = {
    .speak = dog_speak,
    .describe = dog_describe,
};

Dog *dog_create(const char *name, const char *breed)
{
    Dog *self = malloc(sizeof(Dog));

    if (self == NULL) {
        return NULL;
    }

    animal_init(&self->base, name, &dog_vtable);
    self->breed = breed;

    return self;
}

Animal *dog_as_animal(Dog *dog)
{
    /*
     * Public upcast wrapper.
     *
     * Internally this is the same address relationship shown in the
     * memory layout demo of the main lab:
     *
     *   &dog == &dog.base
     */
    return &dog->base;
}
