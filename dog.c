#include <stdio.h>

#include "dog.h"

Dog *dog_from_animal(Animal *animal)
{
    if (!animal_is_dog(animal)) {
        return NULL;
    }

    return (Dog *)animal;
}

const Dog *dog_from_animal_const(const Animal *animal)
{
    if (!animal_is_dog(animal)) {
        return NULL;
    }

    return (const Dog *)animal;
}

static void dog_speak(Animal *animal)
{
    Dog *self = dog_from_animal(animal);

    if (self == NULL) {
        fprintf(stderr, "dog_speak: runtime type is not Dog\n");
        return;
    }

    printf("%s the %s says: Woof!\n", self->base.name, self->breed);
}

/*
 * Secondo metodo virtuale concreto per Dog.
 *
 * Anche qui riceviamo Animal*, ma verifichiamo il tipo concreto
 * prima di interpretare la memoria come Dog.
 */
static void dog_describe(Animal *animal)
{
    Dog *self = dog_from_animal(animal);

    if (self == NULL) {
        fprintf(stderr, "dog_describe: runtime type is not Dog\n");
        return;
    }

    printf("%s is a dog of breed %s.\n", self->base.name, self->breed);
}

/*
 * VTABLE condivisa da tutti i Dog.
 */
static const AnimalVTable dog_vtable = {
    .speak = dog_speak,
    .describe = dog_describe,
};

void dog_init(Dog *self, const char *name, const char *breed)
{
    animal_init(&self->base, name, ANIMAL_TYPE_DOG, &dog_vtable);

    self->breed = breed;
}
