#include <stddef.h>

#include "animal.h"

void animal_init(
    Animal *self,
    const char *name,
    AnimalType type,
    const AnimalVTable *vtable
)
{
    self->name = name;
    self->vtable = vtable;
    self->type = type;
}

const char *animal_get_name(const Animal *self)
{
    return self->name;
}

AnimalType animal_get_type(const Animal *self)
{
    if (self == NULL) {
        return ANIMAL_TYPE_UNKNOWN;
    }

    return self->type;
}

bool animal_is_dog(const Animal *self)
{
    return animal_get_type(self) == ANIMAL_TYPE_DOG;
}

bool animal_is_cat(const Animal *self)
{
    return animal_get_type(self) == ANIMAL_TYPE_CAT;
}

/*
 * Chiamata virtuale pubblica.
 *
 * Questa funzione nasconde al main il dettaglio:
 *
 *   "per parlare devo passare dalla vtable"
 */
void animal_speak(Animal *self)
{
    self->vtable->speak(self);
}

/*
 * Seconda chiamata virtuale pubblica.
 *
 * Ora la vtable contiene più di un metodo.
 * Questo rende più chiaro il concetto di "tabella dei metodi".
 */
void animal_describe(Animal *self)
{
    self->vtable->describe(self);
}
