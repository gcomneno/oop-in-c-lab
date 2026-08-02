#ifndef CAT_H
#define CAT_H

#include "animal.h"

/*
 * Cat eredita da Animal con lo stesso trucco di Dog:
 * Animal base come primo campo.
 *
 * Anche qui vale:
 *
 *     &cat == &cat.base
 *
 * Questo permette di mettere Dog e Cat nello stesso array
 * di Animal* e chiamare speak in modo polimorfico.
 */
typedef struct {
    Animal base;
    int lives;
} Cat;

void cat_init(Cat *self, const char *name, int lives);

/*
 * Downcast controllati.
 *
 * Restituiscono NULL quando Animal non contiene realmente un Cat.
 */
Cat *cat_from_animal(Animal *animal);
const Cat *cat_from_animal_const(const Animal *animal);

#endif
