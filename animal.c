#include "animal.h"

void animal_init(Animal *self, const char *name, const AnimalVTable *vtable)
{
    self->name = name;
    self->vtable = vtable;
}

const char *animal_get_name(const Animal *self)
{
    return self->name;
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
