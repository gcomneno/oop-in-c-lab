#ifndef ANIMAL_H
#define ANIMAL_H

typedef struct Animal Animal;

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
 * Questo puntatore dice:
 *
 *   "Per questo oggetto, quali funzioni devo usare?"
 */
struct Animal {
    const char *name;
    const AnimalVTable *vtable;
};

void animal_init(Animal *self, const char *name, const AnimalVTable *vtable);
const char *animal_get_name(const Animal *self);

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
