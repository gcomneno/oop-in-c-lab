#include <stdio.h>
#include <stdlib.h>

#include "opaque_animal_internal.h"
#include "opaque_cat.h"

struct Cat {
    Animal base;
    int lives;
};

static void cat_speak(Animal *animal)
{
    Cat *self = (Cat *)animal;

    printf("%s has %d lives and says: Meow!\n", self->base.name, self->lives);
}

static void cat_describe(Animal *animal)
{
    Cat *self = (Cat *)animal;

    printf("%s is a cat with %d lives left.\n", self->base.name, self->lives);
}

static const AnimalVTable cat_vtable = {
    .speak = cat_speak,
    .describe = cat_describe,
};

Cat *cat_create(const char *name, int lives)
{
    Cat *self = malloc(sizeof(Cat));

    if (self == NULL) {
        return NULL;
    }

    animal_init(&self->base, name, &cat_vtable);
    self->lives = lives;

    return self;
}

Animal *cat_as_animal(Cat *cat)
{
    return &cat->base;
}
