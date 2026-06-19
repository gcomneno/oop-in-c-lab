#include <stdio.h>

#include "dog.h"

static void dog_speak(Animal *animal)
{
    Dog *self = (Dog *)animal;

    printf("%s the %s says: Woof!\n", self->base.name, self->breed);
}

/*
 * Secondo metodo virtuale concreto per Dog.
 *
 * Anche qui riceviamo Animal*, ma sappiamo che dietro c'è un Dog.
 */
static void dog_describe(Animal *animal)
{
    Dog *self = (Dog *)animal;

    printf("%s is a dog of breed %s.\n", self->base.name, self->breed);
}

/*
 * VTABLE condivisa da tutti i Dog.
 *
 * Ora contiene due metodi:
 *
 * - speak
 * - describe
 */
static const AnimalVTable dog_vtable = {
    .speak = dog_speak,
    .describe = dog_describe,
};

void dog_init(Dog *self, const char *name, const char *breed)
{
    animal_init(&self->base, name, &dog_vtable);

    self->breed = breed;
}
