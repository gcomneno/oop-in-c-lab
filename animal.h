#ifndef ANIMAL_H
#define ANIMAL_H

#include <stdbool.h>

typedef struct Animal Animal;

/*
 * Identità concreta mantenuta esplicitamente a runtime.
 *
 * UNKNOWN usa il valore zero apposta:
 * una struct azzerata non deve sembrare accidentalmente un Dog o un Cat.
 */
typedef enum {
    ANIMAL_TYPE_UNKNOWN = 0,
    ANIMAL_TYPE_DOG,
    ANIMAL_TYPE_CAT,
} AnimalType;

/*
 * VTABLE:
 * tabella di puntatori a funzione.
 *
 * Ogni "classe concreta" avrà la propria tabella:
 *
 * - dog_vtable
 * - cat_vtable
 *
 * Ogni oggetto Dog punterà alla dog_vtable.
 * Ogni oggetto Cat punterà alla cat_vtable.
 *
 * In C++ questo meccanismo viene generato dal compilatore.
 * In C lo scriviamo noi, con calma e casco da cantiere.
 */
typedef struct {
    void (*speak)(Animal *self);
    void (*describe)(Animal *self);
} AnimalVTable;

/*
 * "Classe base" Animal.
 *
 * name:
 *   dato comune.
 *
 * vtable:
 *   puntatore alla tabella dei metodi virtuali.
 *
 * type:
 *   identità concreta mantenuta esplicitamente a runtime.
 */
struct Animal {
    const char *name;
    const AnimalVTable *vtable;
    AnimalType type;
};

void animal_init(
    Animal *self,
    const char *name,
    AnimalType type,
    const AnimalVTable *vtable
);

const char *animal_get_name(const Animal *self);
AnimalType animal_get_type(const Animal *self);

bool animal_is_dog(const Animal *self);
bool animal_is_cat(const Animal *self);

/*
 * Metodi pubblici dell'interfaccia Animal.
 *
 * Il chiamante usa queste funzioni e non tocca direttamente la vtable.
 *
 * Dentro animal.c succederà il vero dispatch virtuale:
 *
 *   self->vtable->speak(self)
 *   self->vtable->describe(self)
 */
void animal_speak(Animal *self);
void animal_describe(Animal *self);

#endif
