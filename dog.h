#ifndef DOG_H
#define DOG_H

#include "animal.h"

/*
 * Dog "eredita" da Animal.
 *
 * BARBATRUCCO FONDAMENTALE:
 * Animal base deve essere il PRIMO campo della struct.
 *
 * In C l'indirizzo di una struct coincide con l'indirizzo
 * del suo primo membro.
 *
 * Quindi, se base è il primo campo:
 *
 *     &dog == &dog.base
 *
 * Questo rende possibile trattare un Dog* come Animal*
 * e, con cautela, tornare da Animal* a Dog*.
 *
 * Questa è la base pratica del nostro upcast/downcast manuale.
 */
typedef struct {
    Animal base;
    const char *breed;
} Dog;

/*
 * Anche Dog non ha un costruttore vero.
 * Usiamo una funzione init esplicita.
 */
void dog_init(Dog *self, const char *name, const char *breed);

#endif
