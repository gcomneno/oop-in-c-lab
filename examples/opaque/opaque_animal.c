#include <stdlib.h>

#include "opaque_animal_internal.h"

/*
 * Hidden implementation of the Animal "base class".
 *
 * Compare with the main lab:
 *   animal.h exposes struct Animal directly.
 *
 * Here, the struct body lives only in this .c file (via the internal header).
 * Client code never sees these fields.
 */

void animal_init(Animal *self, const char *name, const AnimalVTable *vtable)
{
    self->name = name;
    self->vtable = vtable;
}

const char *animal_get_name(const Animal *self)
{
    return self->name;
}

void animal_speak(Animal *self)
{
    /*
     * Virtual dispatch is identical to the main demo:
     *
     *   self->vtable->speak(self)
     *
     * Opaque pointers change visibility, not polymorphism.
     */
    self->vtable->speak(self);
}

void animal_describe(Animal *self)
{
    self->vtable->describe(self);
}

void animal_destroy(Animal *self)
{
    /*
     * In the main demo, Dog and Cat live on the stack and disappear
     * automatically at the end of the scope.
     *
     * In the opaque demo, create() used malloc(), so destroy() must free().
     *
     * We free through Animal* because every concrete object still begins
     * with an embedded Animal base as its first field.
     *
     * That means the address of the whole Dog/Cat block is the same as
     * the address of its embedded Animal base.
     */
    free(self);
}
