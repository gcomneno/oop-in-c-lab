#include <stdio.h>

#include "cat.h"

Cat *cat_from_animal(Animal *animal)
{
    if (!animal_is_cat(animal)) {
        return NULL;
    }

    return (Cat *)animal;
}

const Cat *cat_from_animal_const(const Animal *animal)
{
    if (!animal_is_cat(animal)) {
        return NULL;
    }

    return (const Cat *)animal;
}

static void cat_speak(Animal *animal)
{
    Cat *self = cat_from_animal(animal);

    if (self == NULL) {
        fprintf(stderr, "cat_speak: runtime type is not Cat\n");
        return;
    }

    printf("%s has %d lives and says: Meow!\n", self->base.name, self->lives);
}

/*
 * Secondo metodo virtuale concreto per Cat.
 */
static void cat_describe(Animal *animal)
{
    Cat *self = cat_from_animal(animal);

    if (self == NULL) {
        fprintf(stderr, "cat_describe: runtime type is not Cat\n");
        return;
    }

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
    animal_init(&self->base, name, ANIMAL_TYPE_CAT, &cat_vtable);

    self->lives = lives;
}
