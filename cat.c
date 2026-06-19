#include <stdio.h>

#include "cat.h"

static void cat_speak(Animal *animal)
{
    Cat *self = (Cat *)animal;

    printf("%s has %d lives and says: Meow!\n", self->base.name, self->lives);
}

/*
 * Secondo metodo virtuale concreto per Cat.
 */
static void cat_describe(Animal *animal)
{
    Cat *self = (Cat *)animal;

    printf("%s is a cat with %d lives left.\n", self->base.name, self->lives);
}

/*
 * VTABLE condivisa da tutti i Cat.
 */
static const AnimalVTable cat_vtable = {
    .speak = cat_speak,
    .describe = cat_describe,
};

void cat_init(Cat *self, const char *name, int lives)
{
    animal_init(&self->base, name, &cat_vtable);

    self->lives = lives;
}
